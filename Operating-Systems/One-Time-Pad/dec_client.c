/**********************************************************************
* Program file name: dec_client.c
* Author: Gabriel Valdez
* Date: 3/18/25
* Description: 
*     -  
***********************************************************************/


#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER_SIZE 1024
#define CHUNK_SIZE 512

/******************************************************************************
 * Name: create_client_socket
 * Description:
 *     Creates client socket
 *     Connects client to encryption server
 * Parameters:
 *     - hostname: argument count
 *     - port: argument values
******************************************************************************/
int create_client_socket(const char* hostname, int port) {
    // Get linked list of addrinfo structures for all TCP/IP server sockets
    // listening on localhost (this machine), port 51728
    struct addrinfo* res = NULL;
    struct addrinfo hints = {0};

    memset(&hints, 0, sizeof hints);
    // Support IPv4
    hints.ai_family = AF_INET;
    // TCP protocol
    hints.ai_socktype = SOCK_STREAM;

    // Buffer for port number to convert into string
    // Gotta convert string for getaddrinfo 
    char port_buff[6];
    sprintf(port_buff, "%d", port);

    // Get address info for server
    int info_result = getaddrinfo(
        hostname,
        port_buff,
        &hints,
        &res
    );

    // Error handling
    if (info_result != 0) {
        printf("Error on getaddrinfo!\n");
        return -1;
    }
    
    // Store fd
    int socket_fd = -1;

    // Pointer for iteration over the server addresses
    struct addrinfo* itr = res;

    // Loop through addys
    while (itr != NULL) {
        // Create socket
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            itr = itr->ai_next;
            continue;
        }

        // Successful connection
        if (connect(socket_fd, itr->ai_addr, itr->ai_addrlen) == 0) {
            break; 
        }

        // Connection failed
        close(socket_fd);
        socket_fd = -1;
        itr = itr->ai_next;
    }

    // Free addy info
    freeaddrinfo(res);
    
    // Error handling
    if (socket_fd == -1) {
        fprintf(stderr, "Error on connect!\n");
        return -1;
    }

    return socket_fd;
}


/******************************************************************************
 * Name: read_file
 * Description:
 *     Reads the file
 *     Stores data into a buffer
 * Parameters:
 *     - fn: file name
******************************************************************************/
char* read_file(const char* fn) {
    FILE* f = fopen(fn, "r");
    char* buffer;
    int file_length;

    // Error handling
    if(!f) {
        fprintf(stderr, "File error: Cannot open %s\n", fn);
        return NULL;
    }

     // Get length of file
     fseek(f, 0, SEEK_END);
     file_length = ftell(f);
     fseek(f, 0, SEEK_SET);
     
    buffer = (char*) calloc(file_length + 1, sizeof(char));
    if(!buffer) {
        fprintf(stderr, "Allocation Error: Failed to allocate memory\n");
        fclose(f);
        return NULL;
    }

    // Read the entire file instead of just one line
    size_t bytes_read = fread(buffer, sizeof(char), file_length, f);
    fclose(f);
    
    if (bytes_read == 0) {
        fprintf(stderr, "Read Error: Failed to read from file %s\n", fn);
        free(buffer);
        return NULL;
    }
    
    // Ensure buffer is null-terminated
    buffer[bytes_read] = '\0';
    
    // Get rid of newlines
    buffer[strcspn(buffer, "\n")] = '\0';

    return buffer;
}


/******************************************************************************
 * Name: filter_bad
 * Description:
 *     Checks that the file contains only valid characters from A-Z and space
 * Parameters:
 *     - s: the string that is being checked
******************************************************************************/
int filter_bad(const char* s) {
    int length = strlen(s);

    int i = 0;
    for (i = 0 ; i < length; i++) {
        // Ignore spaces
        if (s[i] == ' ')
            continue;

        // Bad characters
        if (s[i] < 'A' || s[i] > 'Z')
            return 1;
    }
    return 0;
}


/******************************************************************************
 * Name: send_all
 * Description:
 *     Ensures the entire message is sent properly
 * Parameters:
 *     - socket_fd: socket file descriptor
 *     - message: message to send
 *     - length: length of the message
 ******************************************************************************/
int send_all(int socket_fd, const char* message, int length) {
    // Counter for sent bytes
    int total_bytes_sent = 0;

    // Loop entire message
    while (total_bytes_sent < length) {
        // Send remaining bytes
        int bytes_sent = send(socket_fd, message + total_bytes_sent, length - total_bytes_sent, 0);
        
        // Error handling
        if (bytes_sent == -1) {
            fprintf(stderr, "Send Error: Failed to send message\n");   
            return -1;
        }
        // Update counter
        total_bytes_sent += bytes_sent;
    }

    return 0;
}


/******************************************************************************
 * Name: receive_all
 * Description:
 *     Ensures the entire message is received properly
 * Parameters:
 *     - socket_fd: socket file descriptor
 *     - buffer: buffer to store the received message
 *     - length: length of the expected message
 ******************************************************************************/
int receive_all(int socket_fd, char* buffer, int length) {

    // Counter for received bytes
    int total_bytes_received = 0;

    // Loop entire message
    while (total_bytes_received < length) {
        // Receive in chunks
        int bytes_received = recv(socket_fd, buffer + total_bytes_received, length - total_bytes_received, 0);
       
        // Error handling
        if (bytes_received <= 0) {
            return -1;
        }

        // Update counter
        total_bytes_received += bytes_received;
    }

    // Null terminate buffer
    buffer[length] = '\0'; 
    return 0;
}

/******************************************************************************
 * Name: receive_msg
 * Description:
 *      Receives message from server
 * Parameters:
 *     - socket_fd: socket file descriptor
 *     - length: pointer to store received message length
 ******************************************************************************/
char* receive_msg(int socket_fd, int* length) {
    // Get message length
    // Returns Null if unable to get length
    if (recv(socket_fd, length, sizeof(int), 0) <= 0) {
        return NULL;
    }
    
    // Allocate memeory for message
    char* message_received = (char*) calloc(*length + 1, sizeof(char));
    if (!message_received) {
        return NULL;
    }
    
    // Receive message
    if (receive_all(socket_fd, message_received, *length) == -1) {
        free(message_received);
        return NULL;
    }

    return message_received;
}

/******************************************************************************
 * Name: send_msg
 * Description:
 *     - Sends a message over a socket
 * Parameters:
 *     - socket_fd: the socket file descriptor
 *     - message: the message to send
 *     - length: length of the message
 ******************************************************************************/
int send_msg(int socket_fd, const char* message, int length) {
    if (send(socket_fd, &length, sizeof(int), 0) == -1) {
        return -1;
    }

    return send_all(socket_fd, message, length);
}



/******************************************************************************
 * Name: main
 * Description:
 *     Gets the key length, checks if it is valid, then generates and prints
 *     the specified amount of random characters
 * Parameters:
 *     - argc: argument count
 *     - argv: argument values
******************************************************************************/
int main(int argc, char* argv[]) {
    // Argument handling
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ciphertext> <key> <port>\n", argv[0]);
        return 1;
    }

    // Read ciphertext
    char* ciphertext = read_file(argv[1]);
    if (!ciphertext) return 1;

    // Read key
    char* key = read_file(argv[2]);
    if (!key) {
        free(ciphertext);
        return 1;
    }

    // Convert port string to int
    int port = atoi(argv[3]);

    // Key too short
    if (strlen(key) < strlen(ciphertext)) {
        fprintf(stderr, "Key Error: key is too short\n");
        free(ciphertext);
        free(key);
        return 1;
    }

    // Create client socket
    // Connect
    int socket_fd = create_client_socket("localhost", port);
    if (socket_fd == -1) {
        free(ciphertext);
        free(key);
        return 1;
    }

    // Allocate memory for message
    char* msg = calloc(strlen(ciphertext) + strlen(key) + 3, sizeof(char));
    
    // Format using ^ as seperator
    sprintf(msg, "D%s^%s", ciphertext, key);

    // Send message to server
    if (send_msg(socket_fd, msg, strlen(msg)) == -1) {
        fprintf(stderr, "Send_msg Error: Failed to send data\n");
        free(ciphertext);
        free(key);
        free(msg);
        close(socket_fd);
        return 1;
    }

    // Get the decrypted message from server
    int msg_length;
    char* received_msg = receive_msg(socket_fd, &msg_length);
    if (!received_msg) {
        fprintf(stderr, "Error: failed to receive data\n");
        free(ciphertext);
        free(key);
        free(msg);
        close(socket_fd);
        return 1;
    }

    // Print decrypted message
    printf("%s\n", received_msg);

    // Free data
    free(ciphertext);
    free(key);
    free(msg);
    free(received_msg);
    
    close(socket_fd);
    return 0;
}

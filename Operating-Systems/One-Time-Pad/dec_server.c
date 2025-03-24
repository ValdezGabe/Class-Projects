/**********************************************************************
* Program file name: dec_server.c
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
 * Name: create_server_socket
 * Description:
 *      Creates a listening server socket
 * Parameters:
 *     - Port: Port number to bind server
******************************************************************************/
int create_server_socket(int port){
    // Creates a TCP socket
    // Support IPv4
    // 0 denotes default protocol
    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Error handling
    if(listen_socket_fd == -1) {
        fprintf(stderr, "Socket Error: Error creating a listening socket\n");
        return -1;
    }

    // Define a struct to store server addresses
    struct sockaddr_in bind_addr;

    // Initialize to avoid trash
    memset(&bind_addr, 0, sizeof(bind_addr));

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    // Binds socket to specified address
    int bind_result = bind(
        listen_socket_fd,
        (struct sockaddr*) &bind_addr,
        sizeof(bind_addr)
    );

    // Error handling
    if (bind_result == -1) {
        printf("Error on bind!\n");
        return -1;
    }

    // Listen mode
    int listen_result = listen(listen_socket_fd, 5);

    // Error handling
    if (listen_result == -1) {
        printf("Error on listen!\n");
        return -1;
    }

    return listen_socket_fd;
}


/******************************************************************************
 * Name: receive_msg
 * Description:
 *      Receives message from client
 * Parameters:
 *     - communication_socket_fd: Socket for communication with client
 *     - length: Pointer to store the message length
******************************************************************************/
char* receive_msg(int communication_socket_fd, int* length) {
    int msg_length = 0;

    // Receive the message length first
    if (recv(communication_socket_fd, &msg_length, sizeof(int), 0) <= 0) {
        fprintf(stderr, "Error: Failed to receive message length\n");
        return NULL;
    }

    // Allocate memory for message
    char* message_received = (char*) calloc(msg_length + 1, sizeof(char));
    if (!message_received) {
        fprintf(stderr, "Error: Failed to allocate memory for message\n");
        return NULL;
    }

    // Receive the full message
    int total_received = 0;
    while (total_received < msg_length) {
        int bytes_read = recv(communication_socket_fd, message_received + total_received, msg_length - total_received, 0);
        if (bytes_read <= 0) {
            fprintf(stderr, "Error: Connection closed\n");
            free(message_received);
            return NULL;
        }
        total_received += bytes_read;
    }

    // Null terminator
    message_received[msg_length] = '\0';
    *length = msg_length;
    
    return message_received;
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
 * Name: send_msg
 * Description:
 *     Sends a message to the client with length prefix
 * Parameters:
 *     - socket_fd: the socket file descriptor
 *     - message: the message to send
 *     - length: length of the message
 ******************************************************************************/
int send_msg(int socket_fd, const char* message, int length) {
    if (send(socket_fd, &length, sizeof(int), 0) == -1) {
        fprintf(stderr, "Error: Failed to send message length\n");
        return -1;
    }
    
    return send_all(socket_fd, message, length);
}


/******************************************************************************
 * Name: decrypt_msg
 * Description:
 *      Decrypts message received from client
 * Parameters:
 *     - ciphertext: Text to decrypt
 *     - key: Decryption key
******************************************************************************/
char* decrypt_msg(char* ciphertext, char* key) {
    int length = strlen(ciphertext);
    char* plaintext = (char*)calloc(length + 1, sizeof(char));
    if (!plaintext) {
        fprintf(stderr, "Error: Failed to allocate memory for plaintext\n");
        return NULL;
    }

    // Iterate and decrypt each character
    int i;
    for (i = 0; i < length; i++) {
        // Convert characters to numbers
        int ciphertext_value = (ciphertext[i] == ' ') ? 26 : ciphertext[i] - 'A';
        int key_value = (key[i] == ' ') ? 26 : key[i] - 'A';

        // Convert back into character
        int plaintext_value = (ciphertext_value - key_value + 27) % 27;
        plaintext[i] = (plaintext_value == 26) ? ' ' : 'A' + plaintext_value;
    }

    // Null termination
    plaintext[length] = '\0';
    return plaintext;
}


/******************************************************************************
 * Name: handle_client
 * Description:
 *     Handles client communication - receives, encrypts, and sends back message
 * Parameters:
 *     - communication_socket_fd: Socket for communication with client
******************************************************************************/
int handle_client(int communication_socket_fd){
    int msg_length;
    char* received_message = receive_msg(communication_socket_fd, &msg_length);
    if (!received_message) {
        close(communication_socket_fd);
        return 1;
    }

    // Verify correct client connection
    if (received_message[0] != 'D') {
        fprintf(stderr, "Error: dec_server received invalid client\n");
        free(received_message);
        close(communication_socket_fd);
        return 1;
    }

    // Extract ciphertext and key
    char* message_copy = strdup(received_message + 1);
    if (!message_copy) {
        fprintf(stderr, "Error: Failed to duplicate message\n");
        free(received_message);
        close(communication_socket_fd);
        return 1;
    }

    // Tokenize
    char* ciphertext = strtok(message_copy, "^");
    char* key = strtok(NULL, "^");

    // Error handling for both cipher and key
    if (!ciphertext || !key) {
        fprintf(stderr, "Error: Invalid message format\n");
        free(received_message);
        free(message_copy);
        close(communication_socket_fd);
        return 1;
    }

    // Key too short
    if (strlen(key) < strlen(ciphertext)) {
        fprintf(stderr, "Key Error: Key is too short\n");
        free(received_message);
        free(message_copy);
        close(communication_socket_fd);
        return 1;
    }

    // Decrypt
    char* plaintext = decrypt_msg(ciphertext, key);
    if (!plaintext) {
        free(received_message);
        free(message_copy);
        close(communication_socket_fd);
        return 1;
    }

    // Get length of plaintext
    int plaintext_length = strlen(plaintext);

    // Send back to client
    if (send_msg(communication_socket_fd, plaintext, plaintext_length) < 0) {
        fprintf(stderr, "Error: Failed to send decrypted message\n");
        free(received_message);
        free(message_copy);
        free(plaintext);
        close(communication_socket_fd);
        return 1;
    }

    // Free data
    free(received_message);
    free(message_copy);
    free(plaintext);
    close(communication_socket_fd);
    return 0;
}


/******************************************************************************
 * Name: main
 * Description:
 *     Main server function - creates socket, accepts connections and handles clients
 * Parameters:
 *     - argc: argument count
 *     - argv: argument value
******************************************************************************/
int main(int argc, char* argv[]) {
    // Argument handling
    if(argc != 2) {
        fprintf(stderr, "Not enough arguements");
        return -1;
    }

    // Create server socket
    int listen_socket = create_server_socket(atoi(argv[1]));
    if(listen_socket == -1) {
        fprintf(stderr, "Not enough arguements");
        return -1;
    }

    // Handle connections
    while (1) {
        int communication_socket = accept(listen_socket, NULL, NULL);
        if (communication_socket < 0) {
            fprintf(stderr, "Accept failed\n");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Error\n");
            close(communication_socket);
            continue;
        }
        
        // Child process
        // Exit child process after handling client
        if (pid == 0) {  
            close(listen_socket);
            handle_client(communication_socket);
            exit(EXIT_SUCCESS);
        }

        // Parent process
        close(communication_socket);
    }
    
    close(listen_socket);
    return 0;
}

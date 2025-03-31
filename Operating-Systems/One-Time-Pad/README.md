# One-Time Pad Encryption — Operating Systems Project

## Overview
This project simulates the One-Time Pad (OTP) encryption technique using C. It includes functionality for generating random keys, encrypting and decrypting messages, and transferring encrypted data securely between a client and server using TCP sockets. This was developed as part of an Operating Systems course to demonstrate file I/O, process control, and network communication in C.

## Details
- `keygen.c`: Generates a random key file consisting of uppercase letters and spaces.
- `encrypt.c`: Encrypts plaintext using the OTP method and a key of equal length.
- `decrypt.c`: Decrypts ciphertext using the corresponding key.
- `client.c`: Sends data to a server for encryption or decryption.
- `server.c`: Listens for client connections and performs encryption/decryption.
- `helper.c`: Contains shared helper functions such as character conversion and input validation.

## A. Compiling the Program

### Instructions
1. Open a terminal and navigate to the project directory.
2. Run the following command to compile all components:
   ```bash
   make
   ```
3. To remove compiled files and object files, use:
  ```bash
  make clean
  ```

## B. Using the Components

1. Generating A Key
  ```bash
  ./keygen <length>
  ```
- Replace <length> with the number of characters needed for the key.
2. Encrypting a Message
  ```bash
  ./encrypt plaintext.txt key.txt ciphertext.txt
  ```
- `plaintext.txt`: File containing the original message.

- `key.txt`: File containing the OTP key (must be equal or longer than the message).

- `ciphertext.txt`: Output file for the encrypted message.
3. Decrypting a Message
  ```bash
  ./decrypt ciphertext.txt key.txt decrypted.txt
  ```
- `decrypted.txt`: Output file for the decrypted message.

## C. Running Client and Server

### Instructions

1. Start the server
  ```bash
  ./server <port>
  ```
- Replace <port> with an unused port number (e.g., 5000).
  
2. Connect with the client
  ```bash
  ./client localhost <port> plaintext.txt key.txt
  ```
- Sends data to the server for encryption using the specified key.

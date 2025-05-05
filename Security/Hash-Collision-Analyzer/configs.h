#ifndef CONFIGS_H
#define CONFIGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     
#include <openssl/evp.h>

#define HASH_LENGTH     3
#define BUFF_SIZE       8
#define BUCKETS         16777216
#define MAX_ATTEMPTS    9999999999


// Hash Table Struct
typedef struct HashNode {
    char str[BUFF_SIZE + 1];
    unsigned char hash[HASH_LENGTH];
    struct HashNode* next;
} HashNode;


/******************************************************************************
 * Name: generate_char
 * Description:
 *     Generates a random character.
 * Parameters:
 *     - None
******************************************************************************/
char generate_char();

/******************************************************************************
 * Name: generate_string
 * Description:
 *     Fills a buffer with a random string of uppercase characters and spaces.
 * Parameters:
 *     - str: Pointer to the character buffer 
 *     - length: Number of characters to generate
******************************************************************************/
void generate_string(char *str, size_t length);

/******************************************************************************
 * Name: digest_data
 * Description:
 *     Hashes the input using SHA-256 and truncates the result to 24 bits.
 * Parameters:
 *     - input: String to hash
 *     - hash: The output hash
******************************************************************************/
int digest_data(const char *input, unsigned char *hash);


/******************************************************************************
 * Name: hash_key
 * Description:
 *     Turns hash to key in dic
 * Parameters:
 *     - hash: hash value
******************************************************************************/
unsigned int hash_key(const unsigned char *hash);


/******************************************************************************
 * Name: free_table
 * Description:
 *     Frees data
 * Parameters:
 *     - None
******************************************************************************/
void free_table();


/******************************************************************************
 * Name: weak_trial
 * Description:
 *     Brute forces a weak collision
 * Parameters:
 *     - None
******************************************************************************/
int weak_trial();


/******************************************************************************
 * Name: strong_trial
 * Description:
 *     Brute forces a strong collision
 * Parameters:
 *     - None
******************************************************************************/
int strong_trial();

#endif 

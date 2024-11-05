#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEVEL 16


typedef struct Node {
	char* key;
	char* value;
    struct Node **forward;
} Node;


typedef struct kvs{
	Node* header; // database
	int level;
	int items; // number of data 
} kvs_t;


kvs_t* open();
Node* createNode(int level, const char* key, const char* value);
int close(kvs_t* kvs); // free all memory space 
int put(kvs_t* kvs, const char* key, const char* value); // return -1 if failed.
char* get(kvs_t* kvs, const char* key); // return NULL if not found. 

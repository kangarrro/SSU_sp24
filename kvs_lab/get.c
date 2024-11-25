#include "kvs.h"


// get data from KVS(by key)
char* get(kvs_t* kvs, const char* key) {
    if (kvs == NULL || key == NULL) return NULL;

    node_t *current = kvs->header;

    for (int i = kvs->level-1; i >= 0; i--) {
        while (current->forward[i] && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];
    if (current && strcmp(current->key, key) == 0) {
        return current->value;
    }
    printf("Not found\n");
    return NULL;
}
// char* get(kvs_t* kvs, const char* key) {
//     if (kvs == NULL || key == NULL) {
//         fprintf(stderr, "Invalid KVS or Key\n");
//         return NULL;
//     }

//     node_t *current = kvs->header;

//     // Debugging: print the key being searched
//     printf("Searching for key: %s\n", key);

//     // Traverse the skip list
//     for (int i = kvs->level - 1; i >= 0; i--) {
//         while (current->forward[i] && strcmp(current->forward[i]->key, key) < 0) {
//             current = current->forward[i];
//         }
//     }

//     // Check the next node
//     current = current->forward[0];
//     if (current && strcmp(current->key, key) == 0) {
//         return current->value;
//     }

//     printf("Key not found: %s\n", key);
//     return NULL;
// }

#include "kvs.h"


node_t *create_node(unsigned int max_level, const char *key, const char *value, unsigned int id) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (!node) {
        perror("Failed to allocate node");
        return NULL;
    }
    // id
    node->id = id;

    // set size
    size_t key_len = strlen(key) + 1;
    size_t value_len = strlen(value) + 1;
    node->key_size = key_len;
    node->value_size = value_len;

    // key
    node->key = (char *)malloc(node->key_size);
    if (!node->key) {
        perror("Failed to allocate key");
        goto node_free;
    }
    strcpy(node->key, key);

    // value
    node->value = (char *)malloc(value_len);
    if (!node->value) {
        perror("Failed to allocate value");
        goto key_free;
    }
    strcpy(node->value, value);

    for (int i = 0; i < max_level; i++)
        node->forward[i] = NULL;

    return node;

value_free:
    free(node->value);
key_free:
    free(node->key);
node_free:
    free(node);
    return NULL;
}

kvs_t* kvs_open(char *path, unsigned int max_level) {
    kvs_t *kvs;
    
    if(path != NULL){
#ifndef USE_BASELINE
        kvs = do_recovery(path);
#else
        kvs = do_recovery_baseline(path);
#endif
        printf("Open: kvs has %d items\n", kvs->items);
        return kvs; // if failed to recovery : return NULL
    }


    kvs = (kvs_t *)malloc(sizeof(kvs_t));
    if (!kvs) return NULL;

    kvs->header = create_node(max_level, "\x01", "head", 1);
    if (!kvs->header) {
        free(kvs);
        return NULL;
    }
    kvs->level = max_level;
    kvs->items = 0;
    kvs->total_data_size = 5; // include header
    kvs->is_recovered = 0;

    printf("Open: kvs has %d items\n", kvs->items);
    return kvs;
}

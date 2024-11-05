#include "kvs.h"

Node* createNode(int level, const char* key, const char* value) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) return NULL;

    // key 메모리 할당 및 복사
    node->key = (char *)malloc(strlen(key) + 1);
    if (!node->key) {
        free(node);
        return NULL;
    }
    strcpy(node->key, key);

    // value 메모리 할당 및 복사
    node->value = (char *)malloc(strlen(value) + 1);
    if (!node->value) {
        free(node->key);
        free(node);
        return NULL;
    }
    strcpy(node->value, value);

    // forward 포인터 배열 메모리 할당
    node->forward = (Node **)malloc(sizeof(Node*) * (level + 1));
    if (!node->forward) {
        free(node->value);
        free(node->key);
        free(node);
        return NULL;
    }

    for (int i = 0; i <= level; i++)
        node->forward[i] = NULL;

    return node;
}

kvs_t* open() {
    kvs_t* kvs = (kvs_t *)malloc(sizeof(kvs_t));
    if (!kvs) return NULL;

    kvs->level = 0;
    kvs->items = 0;
    kvs->header = createNode(MAX_LEVEL, "\x01", "");
    
    if (!kvs->header) {
        free(kvs);
        return NULL;
    }

    printf("Open: kvs has %d items\n", kvs->items);

    return kvs;
}

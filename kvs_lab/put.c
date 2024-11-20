#include "kvs.h"


int randomLevel() {
    int level = 0;
    while ((rand() % 2) && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

int put(kvs_t* kvs, const char* key, const char* value) {
    if (kvs == NULL || key == NULL || value == NULL) return -1;

    Node *update[MAX_LEVEL + 1];
    Node *current = kvs->header;

    for (int i = kvs->level; i >= 0; i--) {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current != NULL && strcmp(current->key, key) == 0) {
        free(current->value);
        current->value = (char *)malloc(strlen(value) + 1);
        if (current->value == NULL) return -1;
        strcpy(current->value, value);
        return 0;
    }

    int newLevel = randomLevel();
    if (newLevel > kvs->level) {
        for (int i = kvs->level + 1; i <= newLevel; i++) {
            update[i] = kvs->header;
        }
        kvs->level = newLevel;
    }

    Node *newNode = createNode(newLevel, key, value);
    if (newNode == NULL) return -1;

    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }

    kvs->items++;
    return 0;
}

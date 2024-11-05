#include "kvs.h"


int randomLevel() {
    int level = 0;
    while ((rand() % 2) && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

// key-value 쌍을 삽입하는 함수
int put(kvs_t* kvs, const char* key, const char* value) {
    if (kvs == NULL || key == NULL || value == NULL) return -1;

    Node *update[MAX_LEVEL + 1];
    Node *current = kvs->header;

    // 삽입할 위치 탐색
    for (int i = kvs->level; i >= 0; i--) {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    // 키가 이미 존재하면 값 업데이트
    if (current != NULL && strcmp(current->key, key) == 0) {
        free(current->value);
        current->value = (char *)malloc(strlen(value) + 1);
        if (current->value == NULL) return -1;
        strcpy(current->value, value);
        return 0;
    }

    // 새로운 레벨 생성
    int newLevel = randomLevel();
    if (newLevel > kvs->level) {
        for (int i = kvs->level + 1; i <= newLevel; i++) {
            update[i] = kvs->header;
        }
        kvs->level = newLevel;
    }

    // 새로운 노드 생성
    Node *newNode = createNode(newLevel, key, value);
    if (newNode == NULL) return -1;

    // 노드 연결
    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }

    kvs->items++;
    return 0;
}


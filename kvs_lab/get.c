#include "kvs.h"


// get data from KVS(by key)
char* get(kvs_t* kvs, const char* key) {
    if (kvs == NULL || key == NULL) return NULL;

    Node *current = kvs->header;

    // 레벨별로 탐색을 진행하여 key가 저장된 위치를 찾음
    for (int i = kvs->level; i >= 0; i--) {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
    }

    // 다음 노드를 확인하여 키가 존재하는지 검사
    current = current->forward[0];
    if (current != NULL && strcmp(current->key, key) == 0) {
        return current->value;  // 키가 일치하면 해당 value 반환
    }
    return NULL;  // 키를 찾지 못한 경우
}

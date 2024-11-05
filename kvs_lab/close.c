#include "kvs.h"


// free all memory
int close(kvs_t* kvs) {
    if (kvs == NULL) return -1;

    Node *current = kvs->header->forward[0];
    Node *next;

    while (current != NULL) {
        next = current->forward[0];
        free(current->key);  // key 메모리 해제
        free(current->value);  // value 메모리 해제
        free(current->forward);  // forward 포인터 배열 메모리 해제
        free(current);  // 노드 자체 메모리 해제
        current = next;
    }

    free(kvs->header->forward);
    free(kvs->header);
    free(kvs);

    return 0;
}

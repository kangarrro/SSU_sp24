#include "kvs.h"


// free all memory
int kvs_close(kvs_t* kvs) {
    printf("Close KVS...\n");
    if (kvs == NULL) return -1;

    if(!kvs->is_recovered) {
        node_t *current = kvs->header;
        while (current) {
            free(current->key);  // key 메모리 해제
            free(current->value);  // value 메모리 해제
            free(current);  // 노드 자체 메모리 해제
            current = current->forward[0];
        }
    }
    free(kvs);
    return 0;
}

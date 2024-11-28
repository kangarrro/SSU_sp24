#include "kvs.h"


static int random_level() {
    int level = 0;
    while ((rand() % 2) && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

int put(kvs_t* kvs, const char* key, const char* value) {
    if (!kvs || !key || !value){
        errno = EINVAL;
        perror("Parameter error: put() received NULL arguments");
        return -1;
    }

    size_t key_size = strlen(key) + 1;
    size_t value_size = strlen(value) + 1;

    node_t *update[kvs->level];
    node_t *current = kvs->header;

    // 삽입 위치 탐색
    for (int i = kvs->level - 1; i >= 0; i--) {
        while (current->forward[i] && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 탐색위치에 이미 노드가 존재하는 경우
    current = current->forward[0];
    if (current && strcmp(current->key, key) == 0) {
        char *new_value = (char *)malloc(value_size);
        if (!new_value){
            perror("Error updating existing node value (malloc failed)");
            return -1;   
        }
        free(current->value); // 기존 value 해제
        current->value = new_value;
        strcpy(current->value, value);

        kvs->total_data_size += value_size - current->value_size;
        current->value_size = value_size;
        return 0;
    }

    int new_level = random_level();
    if (new_level > kvs->level) {
        for (int i = kvs->level + 1; i <= new_level; i++)
            update[i] = kvs->header;
        kvs->level = new_level;
    }

    node_t *new_node = create_node(kvs->level, key, value, 0); // 현재의 item개수를 id로 사용
    if (!new_node){
        perror("Error creating new node (malloc failed)");
        return -1;
    }
    for (int i = 0; i <= new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }

    kvs->items++;
    kvs->total_data_size += new_node->key_size + new_node->value_size;
    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct kvs_file_header {
    char kvs_name[4];          // KVS 식별자 (e.g., "KVS\0")
    size_t kvs_size;           // kvs_t 구조체 크기
    size_t node_section_size;  // .node 섹션 크기
    size_t data_section_size;  // .data 섹션 크기
};
typedef struct node { // 164
	unsigned int id; // 노드를 식별하기 위한 id(snapshot 저장시에 메모리주소 대신 활용) 4

	size_t key_size; // 8
	size_t value_size; // 8
	char *key; // 8
	char *value; // 8
	struct node *forward[16]; // 8*16
}__attribute__((packed)) node_t;


void inspect_kvs_img(const char *path) {
    FILE *file = fopen(path, "rb");

    struct kvs_file_header header;
    fread(&header, sizeof(header), 1, file);
    printf("KVS Name: %s\n", header.kvs_name);
    printf("KVS Size: %zu\n", header.kvs_size);
    printf("Node Section Size: %zu\n", header.node_section_size);
    printf("Data Section Size: %zu\n", header.data_section_size);

    size_t node_count = header.node_section_size / sizeof(node_t);
    printf("\nNodes (%zu total):\n", node_count);

    node_t node;
    for (size_t i = 0; i < node_count; i++) {
        fread(&node, sizeof(node), 1, file);
        printf("  Node %zu: ID=%u, Key Size=%zu, Value Size=%zu\n", i, node.id, node.key_size, node.value_size);
    }

    fclose(file);
}

int main() {
    inspect_kvs_img("kvs.img");
    return 0;
}

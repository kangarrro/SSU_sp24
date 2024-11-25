#include "kvs.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * save : 
 * 1. node를 순회하며 node 구조체를 작성. -> .node
 * 2. node를 순회하며 key, value를 작성. -> .data
 * 3. node를 순회하며 .node, .data의 크기를 계산 후 파일헤더에 작성.
 * .node 영역의 모든 포인터는 id나 더미데이터로 대체되어 작성됩니다.
 * 
 * recovery :
 * 1. 파일헤더 할당 및 복사.
 * 2. .node, .data를 메모리 할당 및 복사.
 * 3. id를 이용해, 포인터 복구.
 * 
 *
 * // kvs.img
 * .file_header
 * ["KVS"] [kvs_t size] [.node section_size] [.data section size]
 *  식별자    kvs 크기      .node 영역 전체 크기    .data영역 전체 크기
 * 
 * .kvs_header
 * [header] [level] [item] [is_recovered] // kvs_t
 * 
 * .node
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * ...
 * 복구시에는 하나의 배열로 할당
 * 
 * .data
 * [key] [value]
 * [key] [value]
 * [key] [value]
 * ...
 * .node영역의 key, value에는 더미 데이터(포인터를 대신할)가 기록되므로,
 * 실제 key, value를 기록하고, 복구시에 .node::key, value에 연결.
*/

struct kvs_file_header {
    char kvs_name[4];          // KVS 식별자 (e.g., "KVS\0")
    size_t kvs_size;           // kvs_t 구조체 크기
    size_t node_section_size;  // .node 섹션 크기
    size_t data_section_size;  // .data 섹션 크기
} __attribute__((packed));


// save snapShot(.img) to path
int do_snapshot(kvs_t *kvs, char* path) {
    printf("save to snapshot : %s\n", path);

    node_t *current = kvs->header->forward[0];
    unsigned int new_id = 100; // id가 0일경우, 포인터로 변환하는 과정에서 NULL과 구분할 수 없어 100부터 시작.
    while (current) {
        current->id = new_id++;
        current = current->forward[0];
    }
    

    int fd = open(path, O_WRONLY);

    struct kvs_file_header header = {
        .kvs_name = "KVS",
        .kvs_size = sizeof(kvs_t),
        .node_section_size = (kvs->items + 1)*sizeof(node_t),
        .data_section_size = 0,  // 추후 계산
    };

    write(fd, &header, sizeof(header));
    write(fd, kvs, sizeof(kvs_t));

    // .node 작성
    current = kvs->header;
    uint64_t offset_8;
    while(current) {
        header.data_section_size += current->key_size + current->value_size;

        write(fd, &current->id, sizeof(current->id));
        write(fd, &current->key_size, sizeof(current->key_size));
        write(fd, &current->value_size, sizeof(current->value_size));
        write(fd, &current->key, sizeof(current->key));
        write(fd, &current->value, sizeof(current->value));

        for(int i = 0; i < kvs->level; i++) {
            offset_8 = current->forward[i] ? (uint64_t)current->forward[i]->id : 0;
            write(fd, &offset_8, sizeof(uint64_t));
        }
        current = current->forward[0];
    }

    // .data 작성
    current = kvs->header;
    while(current) {
        write(fd, current->key, current->key_size);
        write(fd, current->value, current->value_size);
        current = current->forward[0];
    }
    lseek(fd, 0, SEEK_SET);
    write(fd, &header, sizeof(header));
    close(fd);

    return 1;
}


// recovery snapShot(.img) by path
kvs_t* do_recovery(char* path) {
    printf("\nRecovery KVS...\n");

    // 파일 열기 및 매핑
    int fd = open(path, O_RDONLY);
    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        perror("Failed to stat file");
        close(fd);
        return NULL;
    }
    // void *memmory = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    // struct kvs_file_header *file_header = (struct kvs_file_header *)memory;

    // KVS File Header
    struct kvs_file_header *file_header = malloc(sizeof(struct kvs_file_header));
    read(fd, file_header, sizeof(struct kvs_file_header));
    lseek(fd, sizeof(struct kvs_file_header), SEEK_SET);

    if (strncmp(file_header->kvs_name, "KVS", 3) != 0) {
        printf("Invalid KVS identifier in file\n");
        free(file_header);
        return NULL;
    }
    // 파일에서 .node, .data의 위치 기록
    size_t node_offset = file_header->kvs_size;
    size_t data_offset = node_offset + file_header->node_section_size;
    free(file_header);


    // kvs
    size_t kvs_all_size = file_stat.st_size - sizeof(struct kvs_file_header);
    void *memory = malloc(kvs_all_size);
    if (memory == NULL) {
        perror("Failed to allocate memory");
        close(fd);
        return NULL;
    }

    if (read(fd, memory, kvs_all_size) != kvs_all_size) {
        perror("Failed to read file");
        free(memory);
        close(fd);
        return NULL;
    }
    close(fd);

    // KVS 복구
    kvs_t *kvs = (kvs_t *)(char *)memory;
    node_t *node_section = (node_t *)((char *)memory + node_offset);
    char *data_section = (char *)memory + data_offset;

    kvs->is_recovered = 1;
    kvs->header = node_section;

    // Node 섹션 복구
    node_t *current = kvs->header;
    for (size_t i = 0; i < kvs->items + 1; i++) {
        current[i].key = data_section;
        data_section += current[i].key_size;

        current[i].value = data_section;
        data_section += current[i].value_size;
        // Forward 복구(id to Mem adress)
        for(int level = 0; level<kvs->level; level++){
            uint64_t id = (uint64_t)node_section[i].forward[level];
            node_section[i].forward[level] = id ? (node_t *) &node_section[id-99] : NULL;
        }
    }

    printf("Recovery done\n");
    return kvs;
}

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "kvs.h"


/*
 * kvs.img format
 * 
 * .file_header
 * ["KVS"] [kvs_t size] [.node section_size] [.data section size]
 *  식별자    kvs 크기      .node 영역 전체 크기   .data 영역 전체 크기
 * 
 * .kvs_header
 * [header] [level] [item] [total_data_size] [is_recovered] // kvs_t
 * 
 * .node
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * [id] [key_size] [value_size] [key] [value] [forward] // node_t
 * ...
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
int do_snapshot(kvs_t *kvs, char *path) {
    printf("save to snapshot : %s\n", path);
    
    struct kvs_file_header header = {
        .kvs_name = "KVS",
        .kvs_size = sizeof(kvs_t),
        .node_section_size = (kvs->items + 1)*sizeof(node_t),
        .data_section_size = kvs->total_data_size
    };

    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    size_t file_size = sizeof(struct kvs_file_header) + sizeof(kvs_t) + header.node_section_size + header.data_section_size;
    ftruncate(fd, file_size);

    void *buff = mmap(NULL, file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(buff, &header, sizeof(struct kvs_file_header ));
    memcpy((char *)buff + sizeof(struct kvs_file_header ), kvs, sizeof(kvs_t));

    node_t *node_section = (node_t *)((char *)buff + sizeof(struct kvs_file_header ) + sizeof(kvs_t));
    char *data_section = ((char *)node_section + header.node_section_size);

    node_t *current = kvs->header->forward[0];
    unsigned int new_id = 100; // id가 0일경우, 포인터로 변환하는 과정에서 NULL과 구분할 수 없어 100부터 시작.
    while (current) {
        current->id = new_id++;
        current = current->forward[0];
    }

    current = kvs->header;
    for(size_t i=0; i < kvs->items+1; i++) {
        node_section[i].id = current->id;
        node_section[i].key_size = current->key_size;
        node_section[i].value_size = current->value_size;
        node_section[i].key = NULL;
        node_section[i].value = NULL;

        for (int lv = 0; lv < kvs->level; lv++) {
            uint64_t id = current->forward[lv] ? current->forward[lv]->id : 0;
            memcpy(&node_section[i].forward[lv], &id, sizeof(uint64_t));
        }
        memcpy(data_section, current->key, current->key_size);
        data_section = (char *)(data_section + current->key_size);
        memcpy(data_section, current->value, current->value_size);
        data_section = (char *)(data_section + current->value_size);
        current = current->forward[0];
    }

    if (msync(buff, file_size, MS_SYNC) < 0) {
        perror("Failed to sync file");
    }
    munmap(buff, file_size);
    fsync(fd);
    close(fd);
    return 0;
}

int do_snapshot_baseline(kvs_t *kvs, char* path) {
    printf("save to snapshot(baseline) : %s\n", path);

    struct kvs_file_header header = {
        .kvs_name = "KVS",
        .kvs_size = sizeof(kvs_t),
        .node_section_size = (kvs->items + 1)*sizeof(node_t),
        .data_section_size = kvs->total_data_size
    };

    FILE *file = fopen(path, "wb");
    fwrite(&header, sizeof(struct kvs_file_header), 1, file);
    fwrite(kvs, sizeof(kvs_t), 1, file);
    
    node_t *current = kvs->header->forward[0];
    unsigned int new_id = 100; // id가 0일경우, 포인터로 변환하는 과정에서 NULL과 구분할 수 없어 100부터 시작.
    while (current) {
        current->id = new_id++;
        current = current->forward[0];
    }
    
    // .node 작성
    current = kvs->header;
    uint64_t offset_8;
    while(current) {
        // fwrite(&current, sizeof(node_t)-sizeof(current->forward), 1, file);
        fwrite(&current->id, sizeof(unsigned int), 1, file);
        fwrite(&current->key_size, sizeof(size_t), 1, file);
        fwrite(&current->value_size, sizeof(size_t), 1, file);
        fwrite(&current->key, sizeof(char *), 1, file);
        fwrite(&current->value, sizeof(char *), 1, file);

        for(int i = 0; i < kvs->level; i++) {
            offset_8 = current->forward[i] ? (uint64_t)current->forward[i]->id : 0;
            fwrite(&offset_8, sizeof(uint64_t), 1, file);
        }
        current = current->forward[0];
    }
    // .data 작성
    current = kvs->header;
    while(current) {
        fwrite(current->key, sizeof(char), current->key_size, file);
        fwrite(current->value, sizeof(char), current->value_size, file);
        current = current->forward[0];
    }
    fsync(fileno(file));
    fclose(file);
    return 0;
}


// recovery snapShot(.img) by path
kvs_t* do_recovery(char* path) {
    printf("\nRecovery KVS : %s\n", path);

    // 파일 열기 및 매핑
    int fd = open(path, O_RDONLY);

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

    // kvs
    size_t kvs_all_size = file_header->kvs_size + file_header->node_section_size + file_header->data_section_size;
    free(file_header);

    void *memory = malloc(kvs_all_size);
    if (!memory) {
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
    return kvs;
}

kvs_t* do_recovery_baseline(char* path) {
    printf("\nRecovery KVS(baseline) : %s\n", path);

    // 파일 열기 및 매핑
    FILE *file = fopen(path, "rb");

    // void *memmory = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    // struct kvs_file_header *file_header = (struct kvs_file_header *)memory;

    // KVS File Header
    struct kvs_file_header *file_header = malloc(sizeof(struct kvs_file_header));
    fread(file_header, sizeof(struct kvs_file_header), 1, file);
    fseek(file, sizeof(struct kvs_file_header), SEEK_SET);

    if (strncmp(file_header->kvs_name, "KVS", 3) != 0) {
        printf("Invalid KVS identifier in file\n");
        free(file_header);
        return NULL;
    }
    // 파일에서 .node, .data의 위치 기록
    size_t node_offset = file_header->kvs_size;
    size_t data_offset = node_offset + file_header->node_section_size;

    // kvs
    size_t kvs_all_size = file_header->kvs_size + file_header->node_section_size + file_header->data_section_size;
    free(file_header);

    void *memory = malloc(kvs_all_size);
    if (!memory) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    if (fread(memory, 1, kvs_all_size, file) != kvs_all_size) {
        perror("Failed to read file");
        free(memory);
        fclose(file);
        return NULL;
    }
    fclose(file);

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
    return kvs;
}

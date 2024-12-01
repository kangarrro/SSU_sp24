#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>


#define MAX_LEVEL 16


typedef struct node {
	unsigned int id; // 노드를 식별하기 위한 id(snapshot 저장시에 메모리주소 대신 활용)

	size_t key_size;
	size_t value_size;
	char *key;
	char *value;
	struct node *forward[MAX_LEVEL];
} __attribute__((packed)) node_t;


typedef struct kvs{
	node_t *header; // haeder
	int level; // max level
	int items; // number of item
	size_t total_data_size;
	char is_recovered;
} __attribute__((packed)) kvs_t;


kvs_t* kvs_open(char *path, unsigned int max_level); // 새로운 kvs 생성
node_t* create_node(unsigned int max_level, const char* key, const char* value, unsigned int id); // 새로운 node 생성
int kvs_close(kvs_t* kvs); // 메모리에서 kvs 해제
int put(kvs_t* kvs, const char* key, const char* value); // key, value로 kvs에 삽입
char* get(kvs_t* kvs, const char* key); // key를 이용해 검색

int do_snapshot(kvs_t *kvs, char* path); // kvs를 저장
kvs_t* do_recovery(char* path); // 저장된 kvs를 메모리로 불러옴.

int do_snapshot_baseline(kvs_t *kvs, char *path);
kvs_t* do_recovery_baseline(char* path);

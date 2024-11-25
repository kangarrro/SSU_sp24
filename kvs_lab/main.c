#include "kvs.h"
#define BUFF_SIZE 512


int main() {
    char *line_buf;
    char *command;
    char *key;
    char *value;

    size_t buf_size = 0;
    size_t line_len;

    FILE* file = fopen("./cluster004.trc", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    kvs_t* kvs = kvs_open(NULL, MAX_LEVEL);
    if (!kvs) {
        perror("Failed to initialize key-value store");
        fclose(file);
        return 1;
    }
    int i = 0;
    while ((line_len = getline(&line_buf, &buf_size, file)) != -1) {
        if (line_buf[line_len - 1] == '\n') {
            line_buf[line_len - 1] = '\0';
        }
        line_buf[strcspn(line_buf, "\n")] = 0;

        // parse line
        command = strtok(line_buf, ",");
        key = strtok(NULL, ",");
        value = strtok(NULL, ",");

        if (strcmp(command, "get") == 0){
            // pass now (search by key)
        }

        if (strcmp(command, "set") == 0){
            put(kvs, key, value);
        }
    }
    fclose(file);


    if(!do_snapshot(kvs, "./kvs.img")){
        perror("failed to save snapshot");
    }

    kvs_t *new_kvs = kvs_open("./kvs.img", MAX_LEVEL);
    // kvs_t *new_kvs = do_recovery("./kvs.img");
    kvs_close(kvs);

    put(new_kvs, "tweet105", "test plz");
    printf("\nItems : %u, Level : %u\n", new_kvs->items, new_kvs->level);

    // node_t *current = new_kvs->header->forward[0];
    // while (current) {
    //     // printf("KEY : %s  ", current->key);
    //     current = current->forward[0];
    // }
    kvs_close(new_kvs);
    return 0;
}


























// int main() {
//     char line_buf[BUFF_SIZE];
//     char* command;
//     char* key;
//     char* value;
  
//     FILE* file = fopen("./query.dat", "r");
//     if (!file) {
//         perror("Failed to open FILE(data file)");
//         return -1;
//     }

//     FILE* result_file = fopen("./answer.dat", "w");
//     if (!result_file) {
//         perror("Failed to open FILE(answer file)");
//         fclose(file);
//         return -1;
//     }

//     kvs_t* kvs = open();
//     if (!kvs) {
//         perror("Failed to open kvs");
//         fclose(file);
//         fclose(result_file);
//         return -1;
//     }

//     while (fgets(line_buf, sizeof(line_buf), file)) {
//         line_buf[strcspn(line_buf, "\n")] = 0;

//         command = strtok(line_buf, ",");
//         key = strtok(NULL, ",");
//         value = strtok(NULL, ",");

//         if (strcmp(command, "get") == 0) {
//             value = get(kvs, key);

//             if (value) { // file write value
//                 fprintf(result_file, "%s\n", value);
//             } else { // file write -1
//                 fprintf(result_file, "-1\n");
//             }
//         } else if (strcmp(command, "set") == 0) {
//             put(kvs, key, value);
//         }
//     }

//     fclose(file);
//     fclose(result_file);
//     close(kvs);

//     return 0;
// }
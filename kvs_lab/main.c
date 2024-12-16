#include "kvs.h"
//define MAX_LEVEL 8

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
        perror("Failed to initialize kvs");
        fclose(file);
        return 1;
    }
    
    while ((line_len = getline(&line_buf, &buf_size, file)) != -1) {
        if (line_buf[line_len - 1] == '\n')
            line_buf[line_len - 1] = '\0';

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

#ifndef USE_BASELINE
    if(do_snapshot(kvs, "./kvs.img")){
        perror("failed to save snapshot");
        return 0;
    }
#else
    if(do_snapshot_baseline(kvs, "./kvs.img")){
        perror("failed to save snapshot");
        return 0;
    }
#endif
    kvs_close(kvs);
    kvs_t *new_kvs = kvs_open("./kvs.img", MAX_LEVEL);

    printf("tweet55 : %s\n\ntweet13843 : %s\n\ntweet3482 : %s\n", get(new_kvs, "tweet55"), get(new_kvs, "tweet13843"), get(new_kvs, "tweet3482"));
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
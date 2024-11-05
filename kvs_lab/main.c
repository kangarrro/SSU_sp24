#include "kvs.h"
#define BUFF_SIZE 256


int main() {
    char line_buf[BUFF_SIZE];
    char* command;
    char* key;
    char* value;

    FILE* file = fopen("./query.dat", "r");
    if (!file) {
        perror("Failed to open FILE(data file)");
        return -1;
    }

    FILE* result_file = fopen("./answer.dat", "w");
    if (!result_file) {
        perror("Failed to open FILE(answer file)");
        fclose(file);
        return -1;
    }

    kvs_t* kvs = open();
    if (!kvs) {
        perror("Failed to open kvs");
        fclose(file);
        fclose(result_file);
        return -1;
    }

    while (fgets(line_buf, sizeof(line_buf), file)) {
        line_buf[strcspn(line_buf, "\n")] = 0;

        command = strtok(line_buf, ",");
        key = strtok(NULL, ",");
        value = strtok(NULL, ",");

        if (strcmp(command, "get") == 0) {
            value = get(kvs, key);

            if (value) { // file write value
                fprintf(result_file, "%s\n", value);
            } else { // file write -1
                fprintf(result_file, "-1\n");
            }
        } else if (strcmp(command, "set") == 0) {
            put(kvs, key, value);
        }
    }

    fclose(file);
    fclose(result_file);
    close(kvs);

    return 0;
}
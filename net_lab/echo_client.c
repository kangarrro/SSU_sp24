#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLINE 1024


int main(int argc, char **argv) {
    int client_fd, port;
    struct sockaddr_in server_addr;

    char *host, buf[MAXLINE];

    host = argv[1];
    port = atoi(argv[2]);

    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned int)port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    if(!connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect");
        return -1;
    }

    int rbytes, wbytes;
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        wbytes = write(client_fd, buf, strlen(buf));
        if(wbytes < strlen(buf)) {
            printf("Failed to send message\n");
        }
        rbytes = read(client_fd, buf, MAXLINE);
        fputs(buf, stdout);
    }
    close(client_fd);
    return 0;
}
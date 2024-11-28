#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#define LISTENQ 1024
#define MAXLINE 1024

int main(int argc, char **argv) {
    int listenfd, connected_fd, port, client_len;
    struct sockaddr_in server_addr, client_addr;
    char buf[MAXLINE];

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        return -1;;
    }

    port = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);

    if(bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind");
        return -1;
    }

    if(listen(listenfd, LISTENQ) < 0) {
        perror("Faild to start listening");
        return -1;
    }

    printf("Waiiing for request...\n");

    client_len = sizeof(client_addr);
    if((connected_fd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0){
        perror("Failed to accept client");
        return -1;
    }

    char *caddrp = inet_ntoa(client_addr.sin_addr);
    printf("Connected : %s\n", caddrp);

    int n;
    while (1) {
        n = read(connected_fd, buf, MAXLINE);
        if(!n) break;
        printf("server received %d bytes\n", n);
        write(connected_fd, buf, MAXLINE);
    }
    close(listenfd);
    close(connected_fd);
    return 0;
}
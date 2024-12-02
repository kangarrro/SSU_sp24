#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM 4

void sig_int_handler(int sig) {
    fprintf(stdout, "Handler : Process %d recieved signal %d\n", getpid(), sig);
    exit(0);
}


int main(int argc, char **argv) {
    pid_t pid[NUM];
    int i, child_status;
    signal(SIGKILL, sig_int_handler);

    for(i = 0; i < NUM; i++) {
        if((pid[i] = fork()) == 0) { // Child(wait)
            while(1);
        }
    }

    for(i = 0; i<NUM; i++)
        printf("PID : %d, ", pid[i]);
    printf("\n");

    for(i = 0; i < NUM; i++) { // Parent kill
        printf("Killing process\n");
        kill(pid[i], SIGKILL);
    }

    for(i = 0; i < NUM; i++) {
        pid_t wpid = wait(&child_status);
        if(WIFEXITED(child_status))
            printf("Child %d terminated with exit status %d\n", wpid, WIFEXITED(child_status));
        else
            printf("Child %d terminated abnormally\n", wpid);
    }
    return 0;
}
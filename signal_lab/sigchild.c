#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM 8

int ccount = 0;

void child_handler1(int sig) {
    /*
     * 자식 프로세스들이 한번에 죽음
     * SIGNUL은 아무리 많은 일이 있어도 1번만 발생
     * -> SIGCHLD가 발생해도 자식 프로세스가 몇인지 모름
     * -> 하나만 죽이고 기다림 
    */
    int status; 

    pid_t pid = wait(&status);
    ccount--;

    fprintf(stdout, "Recieved signal %d from process %d\n", sig, pid);
}

void child_handler2(int sig) {
    /*
     * waidpid()를 여러번 함.
     * waitpid(-1, &status, WNOHANG) 알아보기
    */
    printf("child_handler2\n");
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        ccount--;
        fprintf(stdout, "Recieved signal %d from process %d\n", sig, pid);  
    }
    

}

int main() {
    pid_t pid[NUM];
    int i, status;

    ccount = NUM;

    signal(SIGCHLD, child_handler2);

    for(i = 0; i< NUM; i++) {
        if((pid[i] = fork()) == 0) {
            sleep(2);
            exit(0);
        }
    }
    while (ccount > 0) {
        pause();
    }
    return 0;
}
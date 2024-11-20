#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFF_SIZE 512

int main(int argc, char* argv[])
{
	int ifd, ofd;
	char* buff;
	ssize_t nreads, nwrites;

	int op = 0;

	if(argc < 2) {
		fprintf(stdout, "No arguments\n");
		return 0;
	} else if(argc == 2) { // output to stdout file
		op = 1;
	} else if(argc == 4 && !strcmp(argv[2], "into")) { // output to redirection file
		op = 2;
	} else {
		fprintf(stdout, "Wrong arguments\n");
		return 0;
	}

	// Open input file
	int fd = open(argv[1], O_RDONLY);

	// Allocate buffer 
	if(!(buff = (char*) malloc (sizeof(char) * BUFF_SIZE))){
		fprintf(stdout, "Failed to allocate memory\n");
		exit(-1);
	}

	if(op==1) {
		nreads = read(fd, buff, BUFF_SIZE);
		nwrites = write(STDOUT_FILENO, buff, BUFF_SIZE);
	} else if(op==2) {
		dup2(fd, STDIN_FILENO);

		int fd_dest = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC);		
		dup2(fd_dest, STDOUT_FILENO);
		
		nreads = read(fd, buff, BUFF_SIZE);
		nwrites = write(STDOUT_FILENO, buff, BUFF_SIZE);
		close(fd_dest);
	}
	// Close files
	close(fd); 
	free(buff);
}

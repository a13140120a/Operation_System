#include "stdio.h"
#include "string.h"
#include "sys/types.h"
#include "unistd.h"
#include "errno.h"

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

int main() {
	
	char write_msg[BUFFER_SIZE] = "Greetings";
	char read_msg[BUFFER_SIZE];
	int fd[2];
	pid_t pid;

	if (pipe(fd) == -1) {
		fprintf(stderr, "Pipe create failed!");
		return 1;
	}

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "fork failed!");
		return 1;
	}
	if (pid > 0) {  /* parent */
		close(fd[READ_END]);

		write(fd[WRITE_END], write_msg, strlen(write_msg) + 1); //strlen不計算\0所以要+1
		close(fd[WRITE_END]);
	}else {/* child */
		
		close(fd[WRITE_END]);

		read(fd[READ_END], read_msg, BUFFER_SIZE); //strlen不計算\0所以要+1
		printf("%s", read_msg);
		close(fd[READ_END]);
	}

	return 0;
}




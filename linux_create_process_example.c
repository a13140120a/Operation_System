#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"
int main()
{
	int A;
	A=fork();
	int status;
	if(A==0)
	{
		printf("this is from child process\n");
		execlp("/bin/ls","ls",NULL);

	}else{
		printf("this is from parent process\n");
		pid_t pid = wait(&status);
		printf("child %d complete",pid);
	}

	printf("process end %d\n",A);

	return 0;
}

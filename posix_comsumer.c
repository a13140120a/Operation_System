#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "sys/shm.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/mman.h"
#include "errno.h"
int main()
{
	const int SIZE = 4096;

	const char* name = "OS";
	int fd;

	char* ptr;
	fd = shm_open(name, O_RDONLY, 0666);
	if(fd<0){
	    printf("fail fd: %d!!\n",fd);
            exit(1);
	}

	/*注意這邊因為shm_open是O_RDONLY，所以只能有PROT_READ*/
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(ptr==MAP_FAILED){
		printf("mmapfail!!ptr: %p\n",ptr);
		printf("mmap: %s\n", strerror(errno));   // strerror(errno)在debug的時候非常好用
		exit(1);
	}

	printf("%s\n",ptr);
	printf("done!!\n");
	shm_unlink(name);  //沒有unlink的話這塊memory會一直被占用直到重開機或call了這個function

	return 0;
}

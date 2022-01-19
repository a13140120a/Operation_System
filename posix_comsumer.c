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

	/*�`�N�o��]��shm_open�OO_RDONLY�A�ҥH�u�঳PROT_READ*/
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(ptr==MAP_FAILED){
		printf("mmapfail!!ptr: %p\n",ptr);
		printf("mmap: %s\n", strerror(errno));   // strerror(errno)�bdebug���ɭԫD�`�n��
		exit(1);
	}

	printf("%s\n",ptr);
	printf("done!!\n");
	shm_unlink(name);  //�S��unlink���ܳo��memory�|�@���Q�e�Ϊ��쭫�}����call�F�o��function

	return 0;
}

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "sys/shm.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/mman.h"

int main()
{
	const int SIZE = 4096;

	const char* name = "OS";

	const char* message_0 = "Hello";
	const char* message_1 = "World";

	int fd;

	char* ptr;

	fd = shm_open(name, O_CREATE | O_RDWR, 0666);

	ftruncate(fd, SIZE);

	ptr = (char*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	sprintf(ptr, "%s", message_0);
	ptr += strlen(message_0);
	sprintf(ptr, "%s", message_1);
	ptr += strlen(message_1);

	return 0;
}

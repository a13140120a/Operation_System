#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "sys/shm.h"
#include "unistd.h"
#include <sys/stat.h>
#include <sys/mman.h>

int main()
{
	const int SIZE = 4096;

	const char* name = "OS";

	const char* message_0 = "Hello";
	const char* message_1 = "World\0";

	int fd;

	char* ptr;
	/* ����shared-memory����A�bLinux��������i��/dev/shm�̭��ݨ� */
	fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	/* �]�w����j�p�Aftruncate�i�H�ϥΦb���󥴶}�����W */
	ftruncate(fd, SIZE);
	/* �Nshared-memory���� map��memory*/
	/* �`�N:�p�Gshm_open���ĤG�ӰѼƬOO_RDONLY����mmap���ĤT�ӰѼƴN�u�঳PROT_READ�A�_�h�|��Perrmission denied*/
	ptr = (char*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	/* ��message�����e�g��ptr����}�̭� */ 
	sprintf(ptr, "%s", message_0); 
	ptr += strlen(message_0);
	sprintf(ptr, "%s", message_1);
	ptr += strlen(message_1);

	return 0;
}

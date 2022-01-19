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
	/* 產生shared-memory物件，在Linux中此物件可於/dev/shm裡面看到 */
	/* 會回傳一個file descriptor(檔案描述符) */
	fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	/* 設定物件大小，ftruncate可以使用在任何打開的文件上 */
	ftruncate(fd, SIZE);
	/* 將shared-memory物件 map到memory*/
	/* 注意:如果shm_open的第二個參數是O_RDONLY的話mmap的第三個參數就只能有PROT_READ，否則會有Perrmission denied*/
	ptr = (char*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	/* 把message的內容寫到ptr的位址裡面 */ 
	sprintf(ptr, "%s", message_0); 
	ptr += strlen(message_0);
	sprintf(ptr, "%s", message_1);
	ptr += strlen(message_1);

	return 0;
}

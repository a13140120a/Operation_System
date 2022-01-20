#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 25

int main() {

	HANDLE ReadHandle;
	char buffer[BUFFER_SIZE];
	DWORD read;
	printf("start child\n");
	ReadHandle = GetStdHandle(STD_INPUT_HANDLE);// 取得child的stdinput(也就是從pipe來的資訊)

	if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL))
		printf("child read %s", buffer);
	else
		fprintf(stderr, "Error reading from pipe");

	system("pause");

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

DWORD sum;
DWORD WINAPI Summation(LPVOID Param)
{
	DWORD Upper = *(DWORD*)Param;
	for (DWORD i = 1; i <= Upper; i++)
		sum += 1;
	return 0;
}


int main() {
	DWORD ThreadId;
	HANDLE ThreadHandle;
	int Param = 100;

	ThreadHandle = CreateThread(
		NULL,
		0,
		Summation,
		&Param,
		0,
		&ThreadId);

	WaitForSingleObject(ThreadHandle, INFINITE);

	CloseHandle(ThreadHandle);

	printf("sum = %d\n", sum);

	return 0;
}
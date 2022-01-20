#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 25

int main() {

	HANDLE ReadHandle, WriteHandle;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char message[BUFFER_SIZE] = "Greeting";
	DWORD written;
	/* set up attributes allowing pipes to be inherited */
	SECURITY_ATTRIBUTES sa = {
		sizeof(SECURITY_ATTRIBUTES),
		NULL,  // A pointer to a SECURITY_DESCRIPTOR, If the value of this member is NULL, the object is assigned the default security descriptor associated with the access token of the calling process.
		TRUE // 可否繼承handle
	};

	ZeroMemory(&pi, sizeof(pi));

	if (!CreatePipe(&ReadHandle, &WriteHandle, &sa, 0)) {  // 最後一個參數為pipe buffer的size, 0代表使用預設size
		fprintf(stderr, "Create Pipe failed");
		return 1;
	}
	/* 取得parent create自己時的STARTUPINFO，windows本身create新的process的時候會自動調用這個function */
	GetStartupInfo(&si); //child的STARTUPINFO
	/* 設定child的輸岀為stdoutput */
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	/* 將child的輸入導向pipe */
	si.hStdInput = ReadHandle;
	/* 決定STARTUPINFO中那些成員是有效的 */
	si.dwFlags = STARTF_USESTDHANDLES; // 當值為STARTF_USESTDHANDLES時 hStdInput, hStdOutput, and hStdError為有效，且CreateProcess的bInheritHandles一定要是TRUE
	/* 關閉句炳的繼承FLAG */
	SetHandleInformation(WriteHandle, HANDLE_FLAG_INHERIT, 0);
	/* Create process */
	wchar_t szFileName[] = L"child.exe";
	if (!CreateProcess(NULL, szFileName, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		printf("create process failed");

	CloseHandle(ReadHandle);
	/* write */
	if (!WriteFile(WriteHandle, message, BUFFER_SIZE, &written, NULL))
		fprintf(stderr, "Error writing to pipe");

	CloseHandle(WriteHandle);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}
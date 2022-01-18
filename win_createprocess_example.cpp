#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	char szCommandLine[] = "cmd";  //開啟cmd
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	si.dwFlags = STARTF_USESHOWWINDOW;	// 指定STARTUPINFO 的wShowWindow有效
	si.wShowWindow = TRUE;			// 設定wShowWindow，顯示新process的視窗
						// 設定FALSE則不顯示
	BOOL bRet = ::CreateProcess (
		NULL,			// 要執行szCommandLine 的程式
		szCommandLine,		// 執行的command
		NULL,			// process使用預設的security attr
		NULL,			// thread 使用預設的security attr
		FALSE,			// 不繼承句炳
		CREATE_NEW_CONSOLE,	// 開啟新的console
		NULL,			// 繼承環境變數
		NULL,			// 繼承process的driver跟目錄
		&si,
		&pi);

	if(bRet)
	{
		// 關閉hanhandle
		::CloseHandle (pi.hThread);
		::CloseHandle (pi.hProcess);

		printf(" 新Process ID %d \n", pi.dwProcessId);
		printf(" 新Process的main thread ID %d \n", pi.dwThreadId);	
	}
	return 0;
}

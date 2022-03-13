#include <windows.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    HANDLE hFile, hMapFile;
    LPVOID lpMapAddress;

    hFile = CreateFile("temp.txt", /* file name */
      GENERIC READ | GENERIC WRITE, /* read/write access */
      0, /* no sharing of the file */
      NULL, /* default security */
      OPEN ALWAYS, /* open new or existing file */
      FILE ATTRIBUTE NORMAL, /* routine file attributes */
      NULL); /* no file template */
    /* 建立SharedObject的命名共用記憶體物件(named shared-memory object) */
    hMapFile = CreateFileMapping(hFile, /* file handle */
      NULL, /* default security */
      PAGE READWRITE, /* read/write access to mapped pages */
      0, /* map entire file */
      0,
      TEXT("SharedObject")); /* named shared memory object */
    /* 建立共用記憶體物件的指標 */
    lpMapAddress = MapViewOfFile(hMapFile, /* mapped object handle */
      FILE MAP ALL ACCESS, /* read/write access */
      0, /* mapped view of entire file */
      0,
      0);

    /* write to shared memory */
    sprintf(lpMapAddress,"Shared memory message");

    UnmapViewOfFile(lpMapAddress);
    CloseHandle(hFile);
    CloseHandle(hMapFile);
}

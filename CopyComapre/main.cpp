/*
Author:		Julie
Purpose:	using windows system call to copy file not using cache(using FILE_FLAG_WRITE_THROUGH, FILE_FLAG_NO_BUFFERING)
and to compare the copy file whether correct or not.
State:		(2014.0408) recursive version success

Problem:	how to using non-recursive method?
*/

#include "Header.h"
#include <iostream>
#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
using namespace std;

const int SectorSz = 512;
const int Amount = 128;
int ErrorByte = 0;
DWORD dwWrite = 0;
DWORD dwRead = 0;


typedef	int(*pFunc) (const TCHAR[], const TCHAR[]);


int copyfcn(const TCHAR Src[], const TCHAR Dest[])
{
	HANDLE  hFileDest, hFileSrc;
	BOOL bErrorFlag = FALSE;
	int BufferSz = SectorSz * Amount;
	int fileRem;
	int fileSz;

	BYTE* buffer = new BYTE[BufferSz];	
	if (buffer == NULL)
	{
		std::cout << "new buffer failed\r\n" << std::endl;
		return -1;
	}

	hFileSrc = CreateFile(	Src,
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_FLAG_NO_BUFFERING,
							NULL);

	if (hFileSrc == INVALID_HANDLE_VALUE)
	{
		std::cout << "Create read File fail!\r\n" << std::endl;
	}

	hFileDest = CreateFile(	Dest,
							GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							CREATE_NEW,
							FILE_FLAG_WRITE_THROUGH,
							NULL);

	if (hFileDest == INVALID_HANDLE_VALUE)
	{
		std::cout << "Create write File fail!\r\n" << std::endl;
	}

	
	fileSz = fileRem = GetFileSize(hFileSrc, NULL);
	if (SetFilePointer(hFileSrc, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		std::cout << "SetFilePointer failed\r\n" << std::endl;
		delete[] buffer;
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
		return -2;
	}

	if (SetFilePointer(hFileDest, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		std::cout << "SetFilePointer failed\r\n" << std::endl;
		delete[] buffer;
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
		return -3;
	}
	
	while (fileRem)
	{
		if (!ReadFile(hFileSrc, buffer, BufferSz, &dwRead, NULL))
		{
			std::cout << "ReadFile failed\r\n" << std::endl;
			delete[] buffer;
			CloseHandle(hFileDest);
			CloseHandle(hFileSrc);
			return -4;
		}

		if (!WriteFile(hFileDest, buffer, dwRead, &dwWrite, NULL))
		{
			std::cout << "Terminal failure: Unable to write to file." << std::endl;
			delete[] buffer;
			CloseHandle(hFileDest);
			CloseHandle(hFileSrc);
			return -5;
		}

		if (dwRead != dwWrite) {
			cerr << "[WRITE ERROR] ==> Dst file name: " << Dest << endl;
			cerr << "Written length do not match!!" << endl;
			delete[] buffer;
			CloseHandle(hFileSrc);
			CloseHandle(hFileDest);
			return -6;
		}

		fileRem -= dwRead;
	}

	delete[] buffer;
	CloseHandle(hFileSrc);
	CloseHandle(hFileDest);
	return 0;

}
int compare(const TCHAR Src[], const TCHAR Dest[])
{

	int BufferSz = SectorSz * Amount;
	HANDLE  hFileDest, hFileSrc;
	DWORD dwReadSrc,dwReadDest;
	wcout << "Src" << Src << endl;
	wcout << "Dest" << Dest << endl;
	hFileSrc = CreateFile(	Src,
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if (hFileSrc == INVALID_HANDLE_VALUE)
	{
		std::cout << "Create read File fail!\r\n" << std::endl;
	}

	hFileDest = CreateFile(	Dest,
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if (hFileDest == INVALID_HANDLE_VALUE)
	{
		std::cout << "Create write File fail!\r\n" << std::endl;
	}

	BYTE* Srcbuffer = new BYTE[BufferSz];
	if (Srcbuffer == NULL)
	{
		std::cout << "new buffer failed\r\n" << std::endl;
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
		return -1;
	}

	BYTE* Destbuffer = new BYTE[BufferSz];
	if (Destbuffer == NULL)
	{
		std::cout << "new buffer failed\r\n" << std::endl;
		delete[] Srcbuffer;
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
		return -2;
	}

	if (SetFilePointer(hFileSrc, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		std::cout << "SetFilePointer failed\r\n" << std::endl;
		delete[] Srcbuffer;
		CloseHandle(hFileSrc);
		CloseHandle(hFileDest);
		return -3;
	}
	if(SetFilePointer(hFileDest, 0, NULL, FILE_BEGIN == INVALID_SET_FILE_POINTER))
	{
		std::cout << "SetFilePointer failed\r\n" << std::endl;
		delete[] Srcbuffer;
		delete[] Destbuffer;
		CloseHandle(hFileDest);
		CloseHandle(hFileSrc);
		return -4;
	}
	int fileSz1 = GetFileSize(hFileSrc, NULL);
	int fileSz2 = GetFileSize(hFileSrc, NULL);
	if (fileSz1 != fileSz2) {
		cout << "Two file are difference" << endl;
		delete[] Destbuffer;
		delete[] Srcbuffer;
		CloseHandle(hFileDest);
		CloseHandle(hFileSrc);
		return -5;
	}
	int fileRem1, fileRem2;
	fileRem1 = fileSz1;
	fileRem2 = fileSz2;

	while (fileRem1 && fileRem2)
	{

		if (!ReadFile(hFileSrc, Srcbuffer, BufferSz, &dwReadSrc, NULL))
		{
			std::cout << "ReadFile Src failed\r\n" << std::endl;
			delete[] Srcbuffer;
			delete[] Destbuffer;
			CloseHandle(hFileDest);
			CloseHandle(hFileSrc);
			return -6;
		}
		fileRem1 -= dwReadSrc;
		if (!ReadFile(hFileDest, Destbuffer, BufferSz, &dwReadDest, NULL))
		{
			std::cout << "ReadFile Dest failed\r\n" << std::endl;
			delete[] Srcbuffer;
			delete[] Destbuffer;
			CloseHandle(hFileDest);
			CloseHandle(hFileSrc);
			return -7;
		}
		fileRem2 -= dwReadDest;


		for (DWORD i = 0; i < BufferSz; i++)
		{
			if (Srcbuffer[i] != Destbuffer[i]){
				std::cout << "file content is not equal" << std::endl; 
				ErrorByte++; 
				CloseHandle(hFileSrc);
				CloseHandle(hFileDest);
				delete[] Destbuffer;
				delete[] Srcbuffer; 
				return ErrorByte;
			}
		}
		cout << "file content is equal" << endl;
	}

	CloseHandle(hFileSrc);
	CloseHandle(hFileDest);
	delete[] Destbuffer;
	delete[] Srcbuffer;
	return ErrorByte;
}

int DirTraversal(TCHAR SrcDirname[], TCHAR DestDirname[],const pFunc fcn)
{
	HANDLE hSearch;
	TCHAR NewPath[MAX_PATH];
	TCHAR SrcPath[MAX_PATH];
	TCHAR SearchPath[MAX_PATH];

	WIN32_FIND_DATA FileData;
	BOOL fFinished = FALSE;

	StringCchPrintf(SearchPath, MAX_PATH, TEXT("%s\\*.*"), SrcDirname);
	hSearch = FindFirstFile(SearchPath, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		std::cout << "No text files found." << std::endl;
		return -1;
	}

	do
	{
		
		if ((wcscmp(FileData.cFileName, _T(".")) == 0) | (wcscmp(FileData.cFileName, _T("..")) == 0))
			continue;
	
		if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			StringCchPrintf(SrcPath, MAX_PATH, TEXT("%s\\%s"), SrcDirname, FileData.cFileName);
			StringCchPrintf(NewPath, MAX_PATH, TEXT("%s\\%s"), DestDirname, FileData.cFileName);
			if (fcn(SrcPath, NewPath))
				std::cout << "Could not copy file" << std::endl;
			continue;
		}
		
		TCHAR rSrcDirname[MAX_PATH];
		TCHAR rDestDirname[MAX_PATH];

		StringCchPrintf(rSrcDirname, MAX_PATH, TEXT("%s\\%s"), SrcDirname, FileData.cFileName);
		StringCchPrintf(rDestDirname, MAX_PATH, TEXT("%s\\%s"), DestDirname, FileData.cFileName);
		StringCchPrintf(SearchPath, MAX_PATH, TEXT("%s\\*.*"), rSrcDirname);

		CreateDirectory(rDestDirname, NULL);

		DirTraversal(rSrcDirname, rDestDirname, fcn);

	} while (FindNextFile(hSearch, &FileData));

	FindClose(hSearch);
	return 0;
}



int main(int argc, TCHAR *argv[]){

	TCHAR DestDirname[MAX_PATH];
	TCHAR SrcDirname[MAX_PATH];
	TCHAR SearchPath[MAX_PATH];

	std::cout << "Please input the Src path: " << std::endl;
	std::wcin >> SrcDirname;
	std::cout << "Please input the Dest path: " << std::endl;
	std::wcin >> DestDirname;

	if (!CreateDirectory(DestDirname, NULL))
	{
		cout << "CreateDirectory failed " << GetLastError() << endl;
		return -1;
	}

	StringCchPrintf(SearchPath, MAX_PATH, TEXT("%s\\*.*"), SrcDirname);
	cout <<"SearchPath"<< SearchPath << endl;
	DirTraversal(SrcDirname, DestDirname, &copyfcn);
	cout << "Copy file success" << endl;
	StringCchPrintf(SearchPath, MAX_PATH, TEXT("%s\\*.*"), SrcDirname);
	cout << "SearchPath" << SearchPath << endl;
	DirTraversal(SrcDirname, DestDirname, &compare);
	system("pause");
	return 0;
}











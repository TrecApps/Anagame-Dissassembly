
#include "FileFunctions.hpp"

#ifdef _WINDOWS

void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	if (!ReadFile(fileName, bytes, sizeToRead, &bytesRead, nullptr))
		bytesRead = -1;
}

void TcFileWrite(FILE_HANDLE fileName, const UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	if (!WriteFile(fileName, bytes, sizeToRead, &bytesRead, nullptr))
		bytesRead = -1;
}

void TcFileClose(FILE_HANDLE& fileName)
{
	if (fileName)
		CloseHandle(fileName);
	fileName = nullptr;
}

void TcFileFlush(FILE_HANDLE& fileName)
{
	if (fileName)
		FlushFileBuffers(fileName);
}

UINT TcMultiByteToWideChar(const char* source, UINT sourceSize, WCHAR* target, UINT targetSize)
{
	return MultiByteToWideChar(CP_UTF8, 0, source, sourceSize, target, targetSize);
}

UINT TcWideCharToMultiByte(const WCHAR* source, UINT sourceSize, char* target)
{
	return WideCharToMultiByte(CP_ACP, 0, source, -1, target, sourceSize, nullptr, nullptr);
}


#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
#include <stdlib.h>

void TcFileWrite(FILE_HANDLE fileName, const UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
    bytesRead = write(fileName, bytes, sizeToRead);
}

void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	bytesRead = read(fileName, bytes, sizeToRead);
}



void TcFileClose(FILE_HANDLE& fileName)
{
	if (fileName > 2)
		close(fileName);
	fileName = -1;
}

void TcFileFlush(FILE_HANDLE& fileName)
{
	if (fileName > 2)
		fsync(fileName);
}

UINT TcMultiByteToWideChar(const char* source, UINT sourceSize, WCHAR* target, UINT targetSize)
{
	return mbstowcs(target, source, targetSize);
}

UINT TcWideCharToMultiByte(const WCHAR* source, UINT sourceSize, char* target)
{
	return wcstombs(target, source, sourceSize / 2);
}

#endif

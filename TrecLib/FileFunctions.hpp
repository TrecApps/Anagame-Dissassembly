#pragma once

#ifdef _WINDOWS
#include <Windows.h>
#include <PathCch.h>

using FILE_HANDLE = HANDLE;
using FILE_ATTS = WIN32_FILE_ATTRIBUTE_DATA;
#define INVALID_FILE_HANDLE(handle)							!handle
#define NULL_FILE_HANDLE									0
#define TcFileOpen(fileName,readWrite,sharing,atts)			CreateFileW(fileName.GetConstantBuffer().getBuffer(), readWrite, sharing, nullptr, atts, FILE_ATTRIBUTE_NORMAL, nullptr)
#define TcMakeDirectory(filename)							CreateDirectoryW(filename.GetConstantBuffer().getBuffer(), 0)

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "BaseTypes.h"
#if __APPLE__
#include <mach-o/dyld.h>
#endif

#include <fcntl.h>
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
using FILE_HANDLE = int;

#define FILETIME  timespec
#define FILE_ATTS struct stat

#define NULL_FILE_HANDLE  -1

#define INVALID_FILE_HANDLE(handle)							handle < 0
#define INVALID_HANDLE_VALUE								-1
#define NULL_FILE_HANDLE									-1
#define DWORD ssize_t 
#define TcFileOpen(fileName,readWrite,sharing,atts)			open(fileName.GetRegString().c_str(), readWrite | sharing | atts, S_IRGRP)
#define TcMakeDirectory(filename)							mkdir(filename.GetRegString().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

#endif

void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead);
void TcFileWrite(FILE_HANDLE fileName, const UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead);
void TcFileClose(FILE_HANDLE& fileName);
void TcFileFlush(FILE_HANDLE& fileName);

UINT TcMultiByteToWideChar(const char* source, UINT sourceSize, WCHAR* target, UINT targetSize = 0);
UINT TcWideCharToMultiByte(const WCHAR* source, UINT sourceSize, char* target);

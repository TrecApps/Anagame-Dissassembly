#pragma once

#ifndef AG_BASE_TYPES
#define AG_BASE_TYPES

#ifdef _WINDOWS
#include <Windows.h>
using ThreadBlocker = CRITICAL_SECTION;

#define TcLockObject(section)		EnterCriticalSection(section)
#define TcUnlockObject(section)		LeaveCriticalSection(section)
#define TcInitLock(section)			InitializeCriticalSection(section)
#define TcRemoveLock(section)		LeaveCriticalSection(section)

#elif  defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

#include <pthread.h>
using ThreadBlocker = pthread_mutex_t;

#define TcLockObject(section)		pthread_mutex_lock(section)
#define TcUnlockObject(section)		pthread_mutex_unlock(section)
#define TcInitLock(section)			pthread_mutex_init(section, nullptr)
#define TcRemoveLock(section)		pthread_mutex_destroy(section)

#define UNREFERENCED_PARAMETER(P)          (P)

#define UINT      unsigned int
#define USHORT    unsigned short
#define ULONGLONG unsigned long long
#define ULONG64   unsigned long long
#define UCHAR     unsigned char
#define TCHAR     WCHAR
#define LONG      long
#define LONG64    long long
#define LONGLONG  long long
#define WCHAR     wchar_t
#define BOOL      int
#define TRUE      1
#define FALSE     0
#define CHAR      char
#define BYTE      unsigned char


#endif

#ifdef __linux__
#define byte      BYTE
#endif

#endif // AG_BASE_TYPES

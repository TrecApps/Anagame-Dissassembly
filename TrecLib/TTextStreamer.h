#pragma once
#include "TString.h"
#include "TTextIntercepter.h"

#ifdef _WINDOWS

#include <Windows.h>

using tc_thread_id = DWORD;
#define TcResumeThread(x)		ResumeThread(OpenThread(THREAD_SUSPEND_RESUME, FALSE, x))
#define TcGetCurrentThreadId(x)	x = GetCurrentThreadId()
#define TcSuspendThread(x)		SuspendThread(OpenThread(THREAD_SUSPEND_RESUME, FALSE, x))

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

#include <unistd.h>
#include <pthread.h>

using tc_thread_id = pthread_t;
#define TcResumeThread(x)		pthread_resume_np(x, 0)
#define TcGetCurrentThreadId(x)	x = pthread_self()
#define TcSuspendThread(x)		pthread_suspend(x, 0)

#endif // _WINDOWS


class TTextStreamer :
    public TObject
{
    friend class TTextStreamerIntercepter;
private:
    TString input;
    tc_thread_id frozenThread;

    void OnChar(UINT ch, UINT count, UINT flags);

    TrecPointerSoft<TTextStreamer> self;

public:
    TTextStreamer() = default;

    void SetSelf(TrecPointer<TTextStreamer> selfStreamer);

    static void SetMainThread();

    TString Get();

    TrecPointer<TTextIntercepter> GetTextInterceptor(TrecPointer<TTextIntercepter> interceptor);
};


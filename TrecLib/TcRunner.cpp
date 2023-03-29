#include "TcRunner.h"
#include <chrono>

void ThreadSleep();

#ifdef _WINDOWS
#include <Windows.h>

SECURITY_ATTRIBUTES att{
	sizeof(SECURITY_ATTRIBUTES),
	nullptr,
	TRUE
};

void ThreadSleep()
{
	Sleep(1000);
}

DWORD __stdcall ThreadRoutine(LPVOID params)
{
	TrecPointer<TcAsyncRunner::ThreadBridge> bridge = *reinterpret_cast<TrecPointer<TcAsyncRunner::ThreadBridge>*>(params);

	bridge->Run();
	return 0;
}

DWORD CreateAnagameThread(TrecPointer<TcAsyncRunner::ThreadBridge>& bridge)
{
	DWORD ret = 0;
	CreateThread(&att, 3000000, ThreadRoutine,
		&bridge, 0, &ret);
	return ret;
}

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#include <pthread.h>

void ThreadSleep()
{
	sleep(1000);
}

void* ThreadRoutine(void* params)
{
	TrecPointer<TcAsyncRunner::ThreadBridge> bridge = *reinterpret_cast<TrecPointer<TcAsyncRunner::ThreadBridge>*>(params);

	bridge->Run();
	return 0;
}

DWORD CreateAnagameThread(TrecPointer<TcAsyncRunner::ThreadBridge>& bridge)
{
	pthread_t ret = 0;
	pthread_create(&ret, 0, ThreadRoutine,
		&bridge);
	return static_cast<DWORD>(ret);
}

#endif // _WINDOWS


ReturnObject::ReturnObject()
{
	returnCode = nextCount = 0;
	mode = return_mode::rm_regular;
}

ReturnObject::ReturnObject(const ReturnObject& copy)
{
	errorMessage.Set(copy.errorMessage);
	errorObject = copy.errorObject;
	errorObject2 = copy.errorObject2;
	returnCode = copy.returnCode;
	stackTrace = copy.stackTrace;
	mode = copy.mode;
	nextCount = copy.nextCount;
}

ReturnObject& ReturnObject::operator=(const ReturnObject& copy)
{
	errorMessage.Set(copy.errorMessage);
	errorObject = copy.errorObject;
	errorObject2 = copy.errorObject2;
	returnCode = copy.returnCode;
	stackTrace = copy.stackTrace;
	mode = copy.mode;
	nextCount = copy.nextCount;
	return *this;
}

var_type TcRunner::GetVarType()
{
	return var_type::runner;
}

bool TcRunner::Blocks()
{
	return true;
}

void TcRunner::Run(ReturnObject& ret)
{
}

void TcRunner::Pause()
{
}

void TcRunner::Resume()
{
}

void TcRunner::Stop()
{
}

UINT TcRunner::GetSize(void)
{
	return 0;
}

TrecPointer<TVariable> TcRunner::ToString(TrecPointer<TVariable>)
{
	return ToString();
}

TrecPointer<TVariable> TcRunner::ToString()
{
	return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"async function");
}

UINT TcRunner::Get4Value(void)
{
	return 0;
}



ULONG64 TcAsyncRunner::GetTimeMilli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

TcAsyncRunner::~TcAsyncRunner()
{
	Stop();
}

bool TcAsyncRunner::Blocks()
{
	return false;
}

void TcAsyncRunner::Run(ReturnObject& ret)
{
	if (bridge.Get())
		return;

	bridge = TrecPointerKey::GetNewTrecPointer<ThreadBridge>();
	bridge->SetState(thread_state::pre_run);
	bridge->runner = TrecPointerKey::ConvertPointer<TVariable, TcAsyncRunner>(TrecPointerKey::TrecFromSoft<>(this->vSelf));
	bridge->id = CreateAnagameThread(bridge);
}

void TcAsyncRunner::Pause()
{
	if (bridge.Get() && bridge->GetState() == thread_state::running)
		bridge->SetState(thread_state::paused);

}

void TcAsyncRunner::Resume()
{
	if (bridge.Get() && bridge->GetState() == thread_state::paused)
	{
		bridge->SetState(thread_state::running);
		// To-Do: Resume Thread
	}
}

void TcAsyncRunner::Stop()
{
	if (bridge.Get() && bridge->GetState() != thread_state::finished)
	{
		bridge->SetState(thread_state::stopped);

		while (!bridge->CanDelete());
	}
	bridge.Nullify();
}

tc_async_runner_state TcAsyncRunner::GetCurrentState()
{
	if (!bridge.Get())
		return tc_async_runner_state::inactive;

	thread_state s = bridge->GetState();

	switch (s)
	{
	case thread_state::running:
		return tc_async_runner_state::running;
	case thread_state::paused:
		return tc_async_runner_state::paused;
	default:
		return tc_async_runner_state::inactive;
	}
}

TcAsyncRunner::ThreadBridge::ThreadBridge()
{
	this->state = thread_state::pre_run;
	deletable = false;
	id = 0;
}

void TcAsyncRunner::ThreadBridge::Run()
{
	state = thread_state::running;

	while (GetState() != thread_state::stopped && !runner->RunRound())
	{
		while (GetState() == thread_state::paused)
			ThreadSleep();
	}

	if (GetState() != thread_state::stopped)
		SetState(thread_state::finished);
	TObjectLocker lock(&thread);
	deletable = true;
}

thread_state TcAsyncRunner::ThreadBridge::GetState()
{
	TObjectLocker lock(&thread);
	return this->state;
}

void TcAsyncRunner::ThreadBridge::SetState(thread_state tState)
{
	TObjectLocker lock(&thread);
	this->state = tState;
}

bool TcAsyncRunner::ThreadBridge::CanDelete()
{
	TObjectLocker lock(&thread);
	return deletable;
}

TcProcedure::TcProcedure(const TString& name)
{
	this->name.Set(name);
}

var_type TcProcedure::GetVarType()
{
	return var_type::procedure;
}

TrecPointer<TVariable> TcProcedure::ToString()
{

	if (!stringReturn.Get())
	{
		TString ret;
		ret.Format(L"[ Function %ws ]", name.GetConstantBuffer().getBuffer());
		stringReturn = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(ret);
	}

	if (stringReturn->GetVarType() == var_type::string)
		return stringReturn;

	return stringReturn ->ToString();
}

TrecPointer<TVariable> TcProcedure::ToString(TrecPointer<TVariable> detail)
{
	if (detail.Get())
		stringReturn = detail;
	return ToString();
}

UINT TcProcedure::Get4Value()
{
	return 0;
}

UINT TcProcedure::GetSize()
{
	return 0;
}
#include "TTextStreamer.h"
#include <thread>

static tc_thread_id mainThreadId = 0;

class TTextStreamerIntercepter : public TTextIntercepter
{
	TrecPointer<TTextIntercepter> exisitingInt;
	TrecPointerSoft<TTextStreamer> parent;
public:

	TTextStreamerIntercepter(TrecPointer<TTextIntercepter> exisitingInt, TrecPointerSoft<TTextStreamer> streamer)
	{
		this->exisitingInt = exisitingInt;
		this->parent = streamer;
	}

	void OnChar(UINT ch, UINT count, UINT flags) override{
		if (exisitingInt.Get())
			exisitingInt->OnChar(ch, count, flags);

		auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
		if (!actParent.Get())
			return;
		actParent->OnChar(ch, count, flags);
	}

	//virtual void OnKey(UINT ch, UINT count, UINT flags) = 0;

	void OnLoseFocus() override
	{
		if (exisitingInt.Get())
			exisitingInt->OnLoseFocus();
	}

	void OnCopy() override
	{
		if (exisitingInt.Get())
			exisitingInt->OnCopy();
	}
	void OnCut() override
	{
		if (exisitingInt.Get())
			exisitingInt->OnCut();
	}

	void* GetTarget() override
	{
		void* ret = exisitingInt.Get();
		return ret ? ret : this;
	}

	bool TakesInput() override
	{
		if (exisitingInt.Get())
			return exisitingInt->TakesInput();
		return true;
	}
};


void TTextStreamer::OnChar(UINT ch, UINT count, UINT flags)
{
	if (ch == L'\n' && frozenThread)
	{
		auto tempThread = frozenThread;
		frozenThread = 0;

		TcResumeThread(tempThread);
		return;
	}

	for (UINT Rust = 0; Rust < count; Rust++)
		this->input.AppendChar(ch);
}

void TTextStreamer::SetSelf(TrecPointer<TTextStreamer> selfStreamer)
{
	if (selfStreamer.Get() != this)
		throw 1;
	this->self = TrecPointerKey::SoftFromTrec<>(selfStreamer);
}

void TTextStreamer::SetMainThread()
{
	if (!mainThreadId)
		TcGetCurrentThreadId(mainThreadId);
}

TString TTextStreamer::Get()
{
	if (frozenThread)
		return this->input;


	tc_thread_id curId;
	TcGetCurrentThreadId(curId);
	if (curId != mainThreadId)
	{
		// We are waiting for input
		TcGetCurrentThreadId(frozenThread);
		TcSuspendThread(frozenThread);
	}

	TString ret(input);
	input.Empty();
	return ret;
}

TrecPointer<TTextIntercepter> TTextStreamer::GetTextInterceptor(TrecPointer<TTextIntercepter> interceptor)
{
	TrecPointer<TTextStreamerIntercepter> ret = TrecPointerKey::GetNewTrecPointer<TTextStreamerIntercepter>(interceptor, self);
	return TrecPointerKey::ConvertPointer<TTextStreamerIntercepter, TTextIntercepter>(ret);
}

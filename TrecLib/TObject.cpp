
#include "TString.h"

TObjectLocker::TObjectLocker(ThreadBlocker* section) : section(section)
{
	TcLockObject(section);
}

TObjectLocker::~TObjectLocker()
{
	TcUnlockObject(section);
}

TObject::ObjectType TObject::GetObjectType()
{
	return ObjectType::ot_other;
}

TObject::TObject()
{
	TcInitLock(&thread);
}

TObject::~TObject()
{
	TcRemoveLock(&thread);
}


void TObject::ThreadLock() const
{

	TcLockObject(&thread);
}

void TObject::ThreadRelease() const
{
	TcUnlockObject(&thread);

}

TrecPointer<TVariable> TObject::ToString(TrecPointer<TVariable>)
{
	return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"");
}

void TObject::TVariable::SetSelf(TrecPointer<TVariable> vSelf)
{
	if (this != vSelf.Get()) throw - 1;
	this->vSelf = TrecPointerKey::SoftFromTrec<>(vSelf);
}

TObject::TVariable::TVariable()
{
}

TObject::TVariable::~TVariable()
{
}

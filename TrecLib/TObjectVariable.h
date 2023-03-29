#pragma once
#include "TString.h"

class _TREC_LIB_DLL TObjectVariable :
    public TVariable
{
public:
    TrecPointer<TVariable> Clone();

    TObjectVariable(TrecPointer<TVObject> obj);

    var_type GetVarType() override;

    void Set(TrecPointer<TVObject> obj);

    TrecPointer<TVObject> GetObject();

    TrecPointer<TVariable> ToString() override;

    TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail) override;

    UINT Get4Value()override;

    UINT GetSize()override;


private:
    TrecPointer<TVObject> object;
};


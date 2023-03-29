#pragma once
#include "TObject.h"
/**
 * Class: TTextIntercepter
 * Purpose: Retrieves a character from the message infrastructure and directs it towards some Text-based object
 */
class _TREC_LIB_DLL TTextIntercepter : public TObject
{
public:
    virtual void OnChar(UINT ch, UINT count, UINT flags) = 0;

    //virtual void OnKey(UINT ch, UINT count, UINT flags) = 0;

    virtual void OnLoseFocus() = 0;

    virtual void OnCopy() = 0;
    virtual void OnCut() = 0;

    virtual void* GetTarget() = 0;

    virtual bool TakesInput() = 0;

    virtual TrecPointer<TObject> GetTObject();

    // virtual void OnPaste() = 0;
};


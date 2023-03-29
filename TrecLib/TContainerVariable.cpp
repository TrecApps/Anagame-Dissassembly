#include "TContainerVariable.h"
#include "TString.h"
bool TArrayVariable::GetValueAt(UINT index, TrecPointer<TVariable>& data)
{
    return At(data, index);
}
TrecPointer<TVariable> TArrayVariable::Clone()
{
    TrecPointer<TVariable> ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>(isStack, false);

    TrecPointer<TArrayVariable> arrayRet = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(ret);

    TrecPointer<TVariable> var;
    for (UINT Rust = 0; Rust < this->At(var, Rust); Rust++)
        arrayRet->Push(var);

    arrayRet->isStrict = isStrict;
    return ret;
}

TArrayVariable::TArrayVariable(bool isStack, bool isStrict)
{
    this->isStack = isStack;
    this->isStrict = isStrict;
}

bool TArrayVariable::UsesArray() const
{
    return isStack;
}

bool TArrayVariable::IsStrict() const
{
    return isStrict;
}

bool TArrayVariable::Push(TrecPointer<TVariable> data)
{
    if (isStack)
    {
        arrayList.push_back(data);
        return true;
    }
    if(isStrict)
        return false;
    linkedList.Push(data);
    return true;
}

bool TArrayVariable::Pop(TrecPointer<TVariable>& data)
{
    if (!GetSize())
        return false;
    if (isStack)
    {
        data = arrayList.RemoveAt(arrayList.Size() - 1);
        return true;
    }
    else
    {
        if (isStrict)
            return false;
        return linkedList.DequeueTail(data);
    }

}

bool TArrayVariable::Peek(TrecPointer<TVariable>& data)
{
    UINT curSize = GetSize();
    if (!curSize || (!isStack && isStrict))
        return false;
    if (isStack)
        data = arrayList.at(curSize - 1);
    else
        linkedList.at(curSize - 1);
    return true;
}

bool TArrayVariable::Enqueue(TrecPointer<TVariable>& data)
{
    if (isStack)
    {
        if (isStrict) return false;
        arrayList.push_back(data);
    }
    else linkedList.Push(data);
    return true;
}

bool TArrayVariable::Dequeue(TrecPointer<TVariable>& data)
{
    if (!GetSize()) return false;
    if (isStack)
    {
        if (isStrict) return false;
        data = arrayList.RemoveAt(0);
        return true;
    }
    return linkedList.Dequeue(data);
}

bool TArrayVariable::PeekQueue(TrecPointer<TVariable>& data)
{
    if (!GetSize()) return false;
    if (isStack)
    {
        if (isStrict) return false;
        data = arrayList.RemoveAt(0);
    }
    data = linkedList.at(0);
    return true;
}

bool TArrayVariable::At(TrecPointer<TVariable>& data, UINT index)
{
    if(index >= GetSize() || isStrict)
        return false;
    data = isStack ? arrayList.at(index) : linkedList.at(index);
    return true;
}

bool TArrayVariable::RemoveAt(TrecPointer<TVariable>& data, UINT index)
{
    if (index >= GetSize() || isStrict)
        return false;
    data = isStack ? arrayList.RemoveAt(index) : linkedList.Extract(index);
    return true;
}

bool TArrayVariable::InsertAt(TrecPointer<TVariable> data, UINT index)
{
    if (index > GetSize() || isStrict)
        return false;

    if (isStack)
        arrayList.InsertAt(data, index);
    else
        linkedList.Inject(data, index);
    return true;
}

void TArrayVariable::Clear()
{
    if (isStack)
        arrayList.RemoveAll();
    else while (linkedList.GetSize())
        linkedList.DropHead();
        
}

bool TArrayVariable::Update(TrecPointer<TVariable> data, UINT index)
{
    if (index > GetSize() || isStrict)
        return false;

    if (isStack)
        arrayList[index]=(data);
    else
        linkedList.Update(data, index);
    return true;
}

UINT TArrayVariable::GetSize()
{
    return isStack ? arrayList.Size() : linkedList.GetSize();
}

var_type TArrayVariable::GetVarType()
{
    return var_type::list;
}

TrecPointer<TVariable> TArrayVariable::ToString()
{
    TString ret(L'[');
    bool useStrict = isStrict;
    isStrict = false;
    TrecPointer<TVariable> var;
    for (UINT Rust = 0; Rust < GetSize(); Rust++)
    {
        if (Rust) ret.AppendChar(L',');

        ret.Append(At(var, Rust) && var.Get() ? dynamic_cast<TStringVariable*>(var->ToString().Get())->GetString() : L"null");
    }
    isStrict = useStrict;
    ret.AppendChar(L']');
    return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(ret);
}

TrecPointer<TVariable> TArrayVariable::ToString(TrecPointer<TVariable> detail)
{
    return ToString();
}

UINT TArrayVariable::Get4Value()
{
    return 0;
}

TJsonVariable::TJsonVariable()
{
    allowMultiValues = false;
}

TJsonVariable::TJsonVariable(bool allowMultiValue)
{
    allowMultiValues = allowMultiValue;
}

bool TJsonVariable::HasField(const TString& field)
{
    TrecPointer<TVariable> var;
    return values.retrieveEntry(field, var);
}

bool TJsonVariable::RetrieveField(const TString& field, TrecPointer<TVariable>& value)
{
    return values.retrieveEntry(field, value);;
}

bool TJsonVariable::GetValueAt(UINT index, TrecPointer<TVariable>& data)
{
    TString value;
    return RetrieveFieldAt(index, value, data);
}

bool TJsonVariable::RetrieveFieldAt(UINT location, TString& fieldName, TrecPointer<TVariable>& value) const
{
    TDataEntry<TrecPointer<TVariable>> entry;
    bool ret = values.GetEntryAt(location, entry);
    if (ret)
    {
        fieldName.Set(entry.key);
        value = entry.object;
    }
    return ret;
}

bool TJsonVariable::SetField(const TString& field, TrecPointer<TVariable> value, bool doOverride)
{
    if(!doOverride && !allowMultiValues && HasField(field))
        return false;
    values.addEntry(field, value);
    return true;
}

TrecPointer<TVariable> TJsonVariable::Clone()
{
    TrecPointer<TVariable> ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();

    TrecPointer<TJsonVariable> jsonRet = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(ret);
    jsonRet->values = values;
    return ret;
}

UINT TJsonVariable::GetSize()
{
    return values.count();
}

var_type TJsonVariable::GetVarType()
{
    return var_type::json;
}

TrecPointer<TVariable> TJsonVariable::ToString()
{
    TString ret(L'{');
    TDataEntry<TrecPointer<TVariable>> var;
    for (UINT Rust = 0;values.GetEntryAt(Rust, var); Rust++)
    {
        if (Rust) ret.AppendChar(L',');
        ret.AppendFormat(L"\"%ws\" : %ws", var.key.GetConstantBuffer().getBuffer(), TStringVariable::Extract(var.object, L"null").GetConstantBuffer().getBuffer());
    }
    ret.AppendChar(L'}');
    return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(ret);
}

TrecPointer<TVariable> TJsonVariable::ToString(TrecPointer<TVariable> detail)
{
    return ToString();
}

UINT TJsonVariable::Get4Value()
{
    return 0;
}

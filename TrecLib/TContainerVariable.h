#pragma once
#include "TObject.h"
#include "TDataArray.h"
#include "TLinkedList.h"
#include "TDataMap.h"

class _TREC_LIB_DLL TContainerVariable : public TVariable
{
public:
	virtual bool GetValueAt(UINT index, TrecPointer<TVariable>&) = 0;
};

class _TREC_LIB_DLL TArrayVariable : public TContainerVariable
{
	friend class TArrayVariable;
	TDataArray<TrecPointer<TVariable>> arrayList;
	TLinkedList<TrecPointer<TVariable>> linkedList;

	bool isStack;	// Uses the TDataArray if true, the LinkedList otherwise
	bool isStrict;	// Whether operations must be compatible with the stack/queue structure

public:
	bool GetValueAt(UINT index, TrecPointer<TVariable>&) override;

	TrecPointer<TVariable> Clone() override;
	TArrayVariable(bool isStack = true, bool isStrict = false);

	bool UsesArray() const;

	bool IsStrict()const;

	bool Push(TrecPointer<TVariable> data);

	bool Pop(TrecPointer<TVariable>& data);

	bool Peek(TrecPointer<TVariable>& data);

	bool Enqueue(TrecPointer<TVariable>& data);

	bool Dequeue(TrecPointer<TVariable>& data);

	bool PeekQueue(TrecPointer<TVariable>& data);

	bool At(TrecPointer<TVariable>& data, UINT index);

	bool RemoveAt(TrecPointer<TVariable>& data, UINT index);

	bool InsertAt(TrecPointer<TVariable> data, UINT index);

	void Clear();

	bool Update(TrecPointer<TVariable> data, UINT index);

	UINT GetSize() override;
	var_type GetVarType() override;

	TrecPointer<TVariable> ToString() override;

	TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail) override;

	UINT Get4Value() override;
};


class _TREC_LIB_DLL TJsonVariable : public TContainerVariable 
{
	friend class TJsonVariable;
	TDataMap<TrecPointer<TVariable>> values;
	bool allowMultiValues;
public:
	TJsonVariable();
	TJsonVariable(bool allowMultiValue);


	bool HasField(const TString& field);
	bool RetrieveField(const TString& field, TrecPointer<TVariable>& value) ;

	bool GetValueAt(UINT index, TrecPointer<TVariable>&) override;

	bool RetrieveFieldAt(UINT location, TString& fieldName, TrecPointer<TVariable>& value)const;
	bool SetField(const TString& field, TrecPointer<TVariable> value, bool doOverride = false);

	TrecPointer<TVariable> Clone() override;

	UINT GetSize() override;

	var_type GetVarType() override;

	TrecPointer<TVariable> ToString() override;

	TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail) override;

	UINT Get4Value() override;
};
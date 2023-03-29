#include "pch.h"
#include "TObjectVariable.h"

/**
 * Method: TObjectVariable::TObjectVariable
 * Purpose: Constructor
 * Parameters: TrecPointer<TObject> obj - the object to hold
 * Returns: New TObject Variable
 */
TObjectVariable::TObjectVariable(TrecPointer<TVObject> obj)
{
	Set(obj);
}

TrecPointer<TVariable> TObjectVariable::Clone()
{
	return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TObjectVariable>(this->object);
}
/**
 * Method: TObjectVariable::GetVarType
 * Purpose: Reports the type of varible that this object represents
 * Parameters: void
 * Returns: var_type - the type of variable this represents
 */
var_type TObjectVariable::GetVarType()
{
	return var_type::native_object;
}

/**
 * Method: TObjectVariable::Set
 * Purpose: Sets up the object
 * Parameters: TrecObjectPointer obj - the object to hold
 * Returns: void
 */
void TObjectVariable::Set(TrecPointer<TVObject> obj)
{
	object = obj;
}

/**
 * Method: TObjectVariable::GetObject
 * Purpose: Returns the Object held by the variable, or null if variable is a raw data type
 * Parameters: void
 * Returns: TrecObjectPointer - The Object referered by the variable (or null if not an object)
 *
 * Note: Call "IsObject" first before calling this method as there is no point if the "IsObject" returns false
 */
TrecPointer<TVObject> TObjectVariable::GetObject()
{
	auto ret = object;
	return ret;
}

TrecPointer<TVariable> TObjectVariable::ToString()
{
	return object.Get() ? object->ToString(TrecPointer<TVariable>()) :
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"null");
}

TrecPointer<TVariable> TObjectVariable::ToString(TrecPointer<TVariable> detail)
{
	return object.Get() ? object->ToString(detail) :
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"null");
}

UINT TObjectVariable::Get4Value()
{
	return 0;
}

/**
 * Method: TObjectVariable::GetSize
 * Purpose: Returns the estimated size of the value held
 * Parameters: void
 * Returns: UINT - The estimated size in bytes of the data
 */
UINT TObjectVariable::GetSize()
{
	return 0;
}


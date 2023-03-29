#include "TTypeVariable.h"
#include "TDataArray.h"


static TDataArray<TrecPointer<TPrimitiveType>> primitiveTypes;

void PrepPrimitiveTypes()
{

}

var_type TTypeVariable::GetVarType()
{
    return var_type::type;
}

UINT TTypeVariable::Get4Value()
{
    return 0;
}

UINT TTypeVariable::GetSize()
{
    return 0;
}

TrecPointer<TVariable> TTypeVariable::ToString()
{
    return TrecPointer<TVariable>();
}

TrecPointer<TVariable> TTypeVariable::ToString(TrecPointer<TVariable> detail)
{
    return TrecPointer<TVariable>();
}

TrecPointer<TVariable> TTypeVariable::Clone()
{
    return TrecPointerKey::TrecFromSoft<TVariable>(this->vSelf);
}

type_category TArrayType::GetTypeCategory()
{
    return type_category::array_type;
}

int TArrayType::Size() const
{
    return size;
}

TrecPointer<TTypeVariable> TArrayType::GetVariableType() const
{
    return TrecPointer<TTypeVariable>();
}

TArrayType::TArrayType() : TArrayType(-1)
{
}

TArrayType::TArrayType(int size): size(size)
{
}

TArrayType::TArrayType(TrecPointer<TTypeVariable> varType): size(-1), variableType(varType)
{
}

TArrayType::TArrayType(int size, TrecPointer<TTypeVariable> varType): size(size), variableType(varType)
{
}

type_category TFeatureType::GetTypeCategory()
{
    return type_category::feature;
}

bool TFeatureType::IsMutable() const
{
    return isMut;
}

bool TFeatureType::IsPointer() const
{
    return pointerReference > 0;
}

bool TFeatureType::IsPlain() const
{
    return pointerReference == 0;
}

bool TFeatureType::IsReference() const
{
    return pointerReference < 1;
}

TrecPointer<TTypeVariable> TFeatureType::GetType() const
{
    return type;
}

TrecPointer<TFeatureType> TFeatureType::GetPointerType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = 1;
    ret->type = t;
    return ret;
}

TrecPointer<TFeatureType> TFeatureType::GetPlainType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = 0;
    ret->type = t;
    return ret;
}

TrecPointer<TFeatureType> TFeatureType::GetReferenceType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = -1;
    ret->type = t;
    return ret;
}

TrecPointer<TPrimitiveType> TPrimitiveType::GetPrimitiveType(tc_primitive_size size, tc_primitive_type type)
{
    if (!primitiveTypes.Size())
    {
        for (UINT Rust = 0; Rust <= static_cast<UINT>(tc_primitive_type::pt_bool); Rust++)
        {
            for (UINT C = 0; C <= static_cast<UINT>(tc_primitive_size::flexible_size); Rust++)
            {
                auto var = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveType>();
                auto prim = TrecPointerKey::ConvertPointer<TVariable, TPrimitiveType>(var);
                prim->size = static_cast<tc_primitive_size>(C);
                prim->type = static_cast<tc_primitive_type>(Rust);
                primitiveTypes.push_back(prim);
            }
        }
    }

    return primitiveTypes[ static_cast<UINT>(type) * static_cast<UINT>(tc_primitive_type::pt_bool) + static_cast<UINT>(size) ];
}

type_category TPrimitiveType::GetTypeCategory()
{
    return type_category::primitive;
}

tc_primitive_size TPrimitiveType::GetPrimitiveSize() const
{
    return size;
}

tc_primitive_type TPrimitiveType::GetPrimitiveType() const
{
    return type;
}


TUnionType::TUnionType(const TDataArray<TrecPointer<TTypeVariable>>& newTypes)
{
    for (UINT Rust = 0; Rust < newTypes.Size(); Rust++)
        subTypes.push_back(newTypes[Rust]);
}

type_category TUnionType::GetTypeCategory()
{
    return type_category::union_type;
}

UINT TUnionType::GetTypeCount() const
{
    return subTypes.Size();
}

TrecPointer<TTypeVariable> TUnionType::GetTypeAt(UINT index) const
{
    return index < subTypes.Size() ? subTypes[index] : TrecPointer<TTypeVariable>();
}

TClassType::Field::Field(TrecPointer<TTypeVariable> type, TDataArray<TrecPointer<TVariable>>& metadata, UCHAR access, UCHAR feature)
{
    this->access = access;
    this->feature = feature;
    this->type = type;


    for (UINT Rust = 0; Rust < metadata.Size(); Rust++)
    {
        if (metadata[Rust].Get())
            this->metadata.push_back(metadata[Rust]);
    }
}

TClassType::Field::Field()
{
    access = feature = 0;
}

TClassType::Field::Field(const Field& copy)
{
    this->access = copy.access;
    this->feature = copy.feature;
    this->metadata = copy.metadata;
    this->type = copy.type;
}

TrecPointer<TTypeVariable> TClassType::Field::GetType() const
{
    return type;
}

UINT TClassType::Field::GetMetadataCount() const
{
    return metadata.Size();
}

TrecPointer<TVariable> TClassType::Field::GetMetadata(UINT index) const
{
    return index < metadata.Size() ? metadata[index] : TrecPointer<TVariable>();
}

bool TClassType::Field::IsPublic() const
{
    return !(access & 0b00001100);
}

bool TClassType::Field::IsReadPublic() const
{
    return (access & 0b00000011) != 3;
}

bool TClassType::Field::IsSubclassAccessible() const
{
    return !(access & 0b00001100);
}

bool TClassType::Field::IsSubclassReadAccessible() const
{
    return ((access & 0b00001100) >> 2) != 3;
}

bool TClassType::Field::IsModuleAccessible() const
{
    return !(access & 0b00110000);
}

bool TClassType::Field::IsModuleReadAccessible() const
{
    return ((access & 0b00110000) >> 4) != 3;
}

bool TClassType::Field::IsObjectAccessible() const
{
    return !(access & 0b11000000);
}

bool TClassType::Field::IsObjectReadAccessible() const
{
    return ((access & 0b11000000) >> 6) != 3;;
}

bool TClassType::Field::IsStatic() const
{
    return feature & 0x1;
}

bool TClassType::Field::IsConst() const
{
    return feature & 0x2;
}

bool TClassType::Field::IsMutable() const
{
    return !IsConst() && (feature & 0x4);
}

TClassType::Parameter::Parameter(TrecPointer<TTypeVariable> type, TDataArray<TrecPointer<TVariable>>& metadata, const TString& name, TrecPointer<TVariable> defaultValue)
{
    this->type = type;
    this->name.Set(name);
    this->defaultValue = defaultValue;

    for (UINT Rust = 0; Rust < metadata.Size(); Rust++)
    {
        if (metadata[Rust].Get())
            this->metadata.push_back(metadata[Rust]);
    }
}

TClassType::Parameter::Parameter()
{
}

TClassType::Parameter::Parameter(const Parameter& copy)
{
    this->defaultValue = copy.defaultValue;
    this->name.Set(copy.name);
    this->metadata = copy.metadata;
    this->type = copy.type;
}

UINT TClassType::Parameter::GetMetadataCount() const
{
    return metadata.Size();
}

TrecPointer<TVariable> TClassType::Parameter::GetMetadata(UINT index) const
{
    return index < metadata.Size() ? metadata[index] : TrecPointer<TVariable>();
}

TString TClassType::Parameter::GetName() const
{
    return name;
}

TrecPointer<TTypeVariable> TClassType::Parameter::GetType() const
{
    return type;
}

TrecPointer<TVariable> TClassType::Parameter::GetDefaultValue() const
{
    return defaultValue;
}

TClassType::Method::Method()
{
    accessFeature = 0;
}

TClassType::Method::Method(
    TrecPointer<TTypeVariable> returnType,
    TDataArray<Parameter>& parameters,
    TDataArray<TrecPointer<TTypeVariable>>& throws,
    TDataArray<TrecPointer<TVariable>>& metadata,
    UCHAR accessFeature,
    method_mode methodMode)
{
    this->returnType = returnType;
    this->parameters = parameters;

    this->accessFeature = static_cast<USHORT>(accessFeature) + (static_cast<USHORT>(methodMode) << 8); 

    for (UINT Rust = 0; Rust < metadata.Size(); Rust++)
    {
        if (metadata[Rust].Get())
            this->metadata.push_back(metadata[Rust]);
    }

    for (UINT Rust = 0; Rust < throws.Size(); Rust++)
    {
        if (throws[Rust].Get())
            this->throws.push_back(throws[Rust]);
    }
}

TClassType::Method::Method(const Method& copy)
{
    this->returnType = returnType;
    this->parameters = parameters;
    this->throws = throws;
    this->accessFeature = accessFeature;
    this->metadata = metadata;
}

UINT TClassType::Method::GetMetadataCount() const
{
    return metadata.Size();
}

TrecPointer<TVariable> TClassType::Method::GetMetadata(UINT index) const
{
    return TrecPointer<TVariable>();
}

UINT TClassType::Method::GetThrowsCount() const
{
    return throws.Size();
}

TrecPointer<TTypeVariable> TClassType::Method::GetThrows(UINT index) const
{
    return index < throws.Size() ? throws[index] : TrecPointer<TTypeVariable>();
}

UINT TClassType::Method::GetParametersCount() const
{
    return parameters.Size();
}

TClassType::Parameter TClassType::Method::GetParameter(UINT index) const
{
    return index < parameters.Size() ? parameters[index] : Parameter();
}

TrecPointer<TTypeVariable> TClassType::Method::GetReturnType() const
{
    return returnType;
}

bool TClassType::Method::IsPublic() const
{
    return !(accessFeature & 0x1);
}

bool TClassType::Method::IsSubclassAccessible() const
{
    return !(accessFeature & 0x2);
}

bool TClassType::Method::IsModuleAccessible() const
{
    return !(accessFeature & 0x4);
}

bool TClassType::Method::IsObjectAccessible() const
{
    return !(accessFeature & 0x8);
}

bool TClassType::Method::IsStatic() const
{
    return (accessFeature & 0xf);
}

bool TClassType::Method::IsConst() const
{
    return accessFeature & 0x10;
}

TClassType::method_mode TClassType::Method::GetMethodMode() const
{
    return static_cast<method_mode>(this->accessFeature >> 8);
}

TClassType::TClassType()
{
}

TClassType::TClassType(TDataMap<Field>& fields,
    TDataMap<Method>& methods,
    TDataMap<Method>& constructors,
    TrecPointer<TClassType> mainParent,
    TDataArray<TrecPointer<TClassType>>& otherParents,
    TDataMap<TrecPointer<TTypeVariable>>& nestedTypes,
    TDataArray<TrecPointer<TVariable>>& metadata)
{
    this->constructors = constructors;
    this->fields = fields;
    this->methods = methods;
    this->mainParent = mainParent;
    this->otherParents = otherParents;
    this->nestedTypes = nestedTypes;
    this->metadata = metadata;
}

TClassType::TClassType(const TClassType& copy) :
    constructors(copy.constructors),
    fields(copy.fields),
    methods(copy.methods),
    nestedTypes(copy.nestedTypes)
{
    this->mainParent = copy.mainParent;
    this->otherParents = copy.otherParents;
    this->metadata = metadata;
}

UINT TClassType::GetFieldCount() const
{
    return fields.count();
}

bool TClassType::GetFieldAt(UINT index, TString& name, Field& field) const
{
    TDataEntry<Field> entry;
    bool ret = fields.GetEntryAt(index, entry);
    if (ret)
    {
        name.Set(entry.key);
        field = entry.object;
    }
    return ret;
}

UINT TClassType::GetMethodCount() const
{
    return methods.count();
}

bool TClassType::GetMethodAt(UINT index, TString& name, Method& field) const
{
    TDataEntry<Method> entry;
    bool ret = methods.GetEntryAt(index, entry);
    if (ret)
    {
        name.Set(entry.key);
        field = entry.object;
    }
    return ret;
}

UINT TClassType::GetConstructorCount() const
{
    return constructors.count();
}

bool TClassType::GetConstructorAt(UINT index, TString& name, Method& field) const
{
    TDataEntry<Method> entry;
    bool ret = constructors.GetEntryAt(index, entry);
    if (ret)
    {
        name.Set(entry.key);
        field = entry.object;
    }
    return ret;
}

TrecPointer<TClassType> TClassType::GetMainParent() const
{
    return mainParent;
}

UINT TClassType::GetOtherParentCount() const
{
    return otherParents.Size();
}

TrecPointer<TClassType> TClassType::GetOtherParent(UINT index) const
{
    return index < otherParents.Size() ? otherParents[index] : TrecPointer<TClassType>();
}

UINT TClassType::GetNestedTypeCount() const
{
    return nestedTypes.count();
}

bool TClassType::GetNestedTypeAt(UINT index, TString& name, TrecPointer<TTypeVariable>& field) const
{
    TDataEntry<TrecPointer<TTypeVariable>> entry;
    bool ret = nestedTypes.GetEntryAt(index, entry);
    if (ret)
    {
        name.Set(entry.key);
        field = entry.object;
    }
    return ret;
}

UINT TClassType::GetMetadataCount() const
{
    return metadata.Size();
}

TrecPointer<TVariable> TClassType::GetMetadata(UINT index) const
{
    return index < metadata.Size() ? metadata[index] : TrecPointer<TVariable>();
}
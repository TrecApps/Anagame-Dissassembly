#pragma once
#include "TObject.h"
#include "TDataArray.h"
#include "TDataMap.h"
#include "TString.h"

using type_category = enum class type_category
{
    array_type, // Raw Array
    class_type, // Class Types
    feature,    // holds modifiers such as pointer, reference, mutability
    primitive,  // Primitive variables
    union_type  // holds multiple types, one of which is active
};

class TTypeVariable :
    public TVariable
{
public:

    virtual type_category GetTypeCategory() = 0;

    var_type GetVarType() override;
    UINT Get4Value() override;

    UINT GetSize() override;

    TrecPointer<TVariable> ToString();

    TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail);
        
    TrecPointer<TVariable> Clone();
};

class TArrayType : public TTypeVariable
{
private:
    int size;

    TrecPointer<TTypeVariable> variableType;

public:
    type_category GetTypeCategory() override;


    // if negative, this array's size is dynamic
    int Size() const;

    // If Null, then elements could be of ay type
    TrecPointer<TTypeVariable> GetVariableType() const;

    // Produces a spec of a dynamic array of any type
    TArrayType();
    // Produces a spec of an 'any' type array of the specified type
    explicit TArrayType(int size);
    // Procuces a spec of a dynamic array of the specified type
    explicit TArrayType(TrecPointer<TTypeVariable> varType);
    // Produces a spec of an array of the specific type and size
    explicit TArrayType(int size, TrecPointer<TTypeVariable> varType);
};

class TFeatureType : public TTypeVariable
{
private:
    signed char pointerReference;
    bool isMut;
    TrecPointer<TTypeVariable> type;
public:
    type_category GetTypeCategory() override;
    bool IsMutable()const;
    bool IsPointer()const;
    bool IsPlain()const;
    bool IsReference()const;
    TrecPointer<TTypeVariable> GetType()const;

    static TrecPointer<TFeatureType> GetPointerType(TrecPointer<TTypeVariable>, bool isMut = true);

    static TrecPointer<TFeatureType> GetPlainType(TrecPointer<TTypeVariable>, bool isMut = true);

    static TrecPointer<TFeatureType> GetReferenceType(TrecPointer<TTypeVariable>, bool isMut = true);
};

using tc_primitive_type = enum class tc_primitve_type {
    pt_int,
    pt_uint,
    pt_float,
    pt_char,
    pt_bool
};

using tc_primitive_size = enum class tc_primitive_size {
    one_byte,       // 8-bits
    two_byte,       // 16-bits
    four_byte,      // 32-bits
    eight_byte,     // 64-bits
    twelve_byte,    // 96-bits
    sixteen_byte,   // 128-bits
    flexible_size   // Size varies (should not be used in Binary-Runners)
};

class TPrimitiveType : public TTypeVariable
{
    friend class TrecPointerKey;
    tc_primitive_size size;
    tc_primitive_type type;
    explicit TPrimitiveType() = default;
public:
    static TrecPointer<TPrimitiveType> GetPrimitiveType(tc_primitive_size size, tc_primitive_type type);

    type_category GetTypeCategory() override;
    tc_primitive_size GetPrimitiveSize() const;
    tc_primitive_type GetPrimitiveType() const;
};

class TUnionType : public TTypeVariable
{
    TDataArray<TrecPointer<TTypeVariable>> subTypes;
public:
    explicit TUnionType(const TDataArray<TrecPointer<TTypeVariable>>& newTypes);
    type_category GetTypeCategory() override;
    UINT GetTypeCount() const;
    TrecPointer<TTypeVariable> GetTypeAt(UINT index) const;
};

class TClassType : public TTypeVariable
{
public:
    class Field
    {
        friend class Field;
        TrecPointer<TTypeVariable> type;
        UCHAR access;
        UCHAR feature;
        TDataArray<TrecPointer<TVariable>> metadata;
    public:
        Field(TrecPointer<TTypeVariable> type, TDataArray<TrecPointer<TVariable>>& metadata, UCHAR access = 0, UCHAR feature = 0);
        Field();
        Field(const Field& copy);

        TrecPointer<TTypeVariable> GetType() const;

        UINT GetMetadataCount()const;
        TrecPointer<TVariable> GetMetadata(UINT index) const;

        bool IsPublic() const;
        bool IsReadPublic() const;

        bool IsSubclassAccessible() const;
        bool IsSubclassReadAccessible() const;

        bool IsModuleAccessible() const;
        bool IsModuleReadAccessible() const;

        bool IsObjectAccessible() const;
        bool IsObjectReadAccessible() const;

        bool IsStatic() const;
        bool IsConst() const;   // Needs to be set up in either the constructor/class-set-up
        bool IsMutable() const; // The field can be updated, even if the object itself is considered "const"
    };

    class Parameter
    {
        TrecPointer<TTypeVariable> type;
        TString name;
        TrecPointer<TVariable> defaultValue;
        TDataArray<TrecPointer<TVariable>> metadata;
    public:
        Parameter(TrecPointer<TTypeVariable> type, TDataArray<TrecPointer<TVariable>>& metadata, const TString& name, TrecPointer<TVariable> defaultValue);
        Parameter();
        Parameter(const Parameter& copy);

        UINT GetMetadataCount()const;
        TrecPointer<TVariable> GetMetadata(UINT index) const;

        TString GetName() const;
        TrecPointer<TTypeVariable> GetType() const;
        TrecPointer<TVariable> GetDefaultValue() const;



    };

    using method_mode = enum class method_mode
    {
        fixed,      // Method not virtual
        virtual_,   // Method is virtual (meaning object comes with a pointer to it)
        override_,  // Method is virtual and overrides a virtual method in a base class
        abstract_,  // Method is virtual and requires a subclass to implement it
        final_      // Method is virtual and cannot be overridden
    };

    class Method
    {
        TrecPointer<TTypeVariable> returnType;
        TDataArray<Parameter> parameters;
        USHORT accessFeature;
        TDataArray<TrecPointer<TTypeVariable>> throws;
        TDataArray<TrecPointer<TVariable>> metadata;

    public:
        Method();
        Method(TrecPointer<TTypeVariable> returnType,
            TDataArray<Parameter>& parameters,
            TDataArray<TrecPointer<TTypeVariable>>& throws,
            TDataArray<TrecPointer<TVariable>>& metadata,
            UCHAR accessFeature = 0,
            method_mode methodMode = method_mode::virtual_);
        Method(const Method& copy);

        UINT GetMetadataCount() const;
        TrecPointer<TVariable> GetMetadata(UINT index) const;

        UINT GetThrowsCount() const;
        TrecPointer<TTypeVariable> GetThrows(UINT index) const;

        UINT GetParametersCount() const;
        Parameter GetParameter(UINT index) const;

        TrecPointer<TTypeVariable> GetReturnType() const;

        bool IsPublic() const;
        bool IsSubclassAccessible() const;
        bool IsModuleAccessible() const;
        bool IsObjectAccessible() const;

        bool IsStatic() const;
        bool IsConst() const;   // Whether the 

        method_mode GetMethodMode() const;
        
    };
private:

    TDataMap<Field> fields;
    TDataMap<Method> methods, constructors;

    TrecPointer<TClassType> mainParent;
    TDataArray<TrecPointer<TClassType>> otherParents;

    TDataMap<TrecPointer<TTypeVariable>> nestedTypes;

    TDataArray<TrecPointer<TVariable>> metadata;

public:
    TClassType();
    TClassType(TDataMap<Field>& fields,
        TDataMap<Method>& methods,
        TDataMap<Method>& constructors,
        TrecPointer<TClassType> mainParent,
        TDataArray<TrecPointer<TClassType>>& otherParents,
        TDataMap<TrecPointer<TTypeVariable>>& nestedTypes,
        TDataArray<TrecPointer<TVariable>>& metadata);
    TClassType(const TClassType& copy);

    UINT GetFieldCount() const;
    bool GetFieldAt(UINT index, TString& name, Field& field) const;

    UINT GetMethodCount() const;
    bool GetMethodAt(UINT index, TString& name, Method& field) const;

    UINT GetConstructorCount() const;
    bool GetConstructorAt(UINT index, TString& name, Method& field) const;

    TrecPointer<TClassType> GetMainParent() const;
    UINT GetOtherParentCount() const;
    TrecPointer<TClassType> GetOtherParent(UINT index) const;

    UINT GetNestedTypeCount() const;
    bool GetNestedTypeAt(UINT index, TString& name, TrecPointer<TTypeVariable>& field) const;

    UINT GetMetadataCount() const;
    TrecPointer<TVariable> GetMetadata(UINT index) const;
};
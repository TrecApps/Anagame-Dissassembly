#pragma once
#include "TObject.h"

/**
 * Class: TPrimitiveVariable
 * Purpose: Represents a primitive variable, such as a number or a character
 *
 * SuperClass: TVariable
 */
class _TREC_LIB_DLL TPrimitiveVariable :
    public TVariable
{
    friend class TPrimitiveVariable;
public:

    virtual TrecPointer<TVariable> Clone()override;

    TrecPointer<TObject::TVariable> ToString(TrecPointer<TObject::TVariable>)override;
    TrecPointer<TObject::TVariable> ToString()override;
    /**
     * Method: TPrimitiveVariable::GetVarType
     * Purpose: Reports the type of varible that this object represents
     * Parameters: void
     * Returns: var_type - the type of variable this represents
     *
     * Attributes: override
     */
    virtual var_type GetVarType() override;

    /**
     * Method: TPrimitiveVarible::Get4Value
     * Purpose: Returns the value held by the variable assuming four bytes (it is up to the interpretor to determine if conversion needs to be done)
     * Parameters: void
     * Returns: UINT - The value held as a UINT (0 if not a primitive type
     *
     * Attributes: override
     */
    virtual UINT Get4Value()override;

    ULONG64 Get8Value();


    /**
     * Method: TPrimitiveVarible::GetSize
     * Purpose: Returns the estimated size of the value held
     * Parameters: void
     * Returns: UINT - The estimated size in bytes of the data
     *
     * Attributes: override
     */
    virtual UINT GetSize()override;


    /**
     * Method: TPrimitiveVarible::GetType
     * Purpose: Returns the basic type of the object
     * Parameters: void
     * Returns: UCHAR - The value held as a UINT (0 if not a primitive type)
     *
     * Attributes: override
     */
    virtual UINT GetVType();

    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: float value - the value to set, as a float
     * Returns: New TPrimitive variable that represents a float
     */
    TPrimitiveVariable(float value);

    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: double value - the value to set, as a double
     * Returns: New TPrimitive variable that represents a double
     */
    TPrimitiveVariable(double value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: signed char value - the value to set, as a 1 byte int
     * Returns: New TPrimitive variable that represents a 1 byte int
     */
    TPrimitiveVariable(signed char value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: char - the value to set, as a 1 byte char
     * Returns: New TPrimitive variable that represents a char
     */
    TPrimitiveVariable(char value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: UCHAR value - the value to set, as a UCHAR
     * Returns: New TPrimitive variable that represents a UCHAR
     */
    TPrimitiveVariable(UCHAR value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: short value - the value to set, as a short
     * Returns: New TPrimitive variable that represents a short
     */
    TPrimitiveVariable(short value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: USHORT value - the value to set, as a USHORT
     * Returns: New TPrimitive variable that represents a USHORT
     */
    TPrimitiveVariable(USHORT value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: WCHAR value - the value to set, as a WCHAR
     * Returns: New TPrimitive variable that represents a WCHAR
     */
    TPrimitiveVariable(WCHAR value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: int value - the value to set, as an int
     * Returns: New TPrimitive variable that represents an int
     */
    TPrimitiveVariable(int value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: UINT value - the value to set, as a UINT
     * Returns: New TPrimitive variable that represents a UINT
     */
    TPrimitiveVariable(UINT value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: LONG64 value - the value to set, as a LONG64
     * Returns: New TPrimitive variable that represents a LONG64
     */
    TPrimitiveVariable(LONG64 value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: ULONG64 value - the value to set, as a ULONG64
     * Returns: New TPrimitive variable that represents a ULONG64
     */
    TPrimitiveVariable(ULONG64 value);


    /**
     * Method: TPrimitiveVariable::TPrimitiveVariable
     * Purpose: Constructor
     * Parameters: bool value - the value to set, as a bool
     * Returns: New TPrimitive variable that represents a bool
     */
    TPrimitiveVariable(bool value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: float value - the value to set, as a float
     * Returns: void
     */
    void Set(float value);

    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: double value - the value to set, as a double
     * Returns: void
     */
    void Set(double value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: signed char value - the value to set, as a 1 byte int
     * Returns: void
     */
    void Set(signed char value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: char - the value to set, as a 1 byte char
     * Returns: void
     */
    void Set(char value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: UCHAR value - the value to set, as a UCHAR
     * Returns: void
     */
    void Set(UCHAR value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: short value - the value to set, as a short
     * Returns: void
     */
    void Set(short value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: USHORT value - the value to set, as a USHORT
     * Returns: void
     */
    void Set(USHORT value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: WCHAR value - the value to set, as a WCHAR
     * Returns: void
     */
    void Set(WCHAR value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: int value - the value to set, as an int
     * Returns: void
     */
    void Set(int value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: UINT value - the value to set, as a UINT
     * Returns: void
     */
    void Set(UINT value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: LONG64 value - the value to set, as a LONG64
     * Returns: void
     */
    void Set(LONG64 value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: ULONG64 value - the value to set, as a ULONG64
     * Returns: void
     */
    void Set(ULONG64 value);


    /**
     * Method: TPrimitiveVariable::Set
     * Purpose: Sets the value and type of this Primitive Variable
     * Parameters: bool value - the value to set, as a bool
     * Returns: void
     */
    void Set(bool value);

    /**
     * Method: TPrimitiveVariable::BitShift
     * Purpose: Performs a bitshift operatoin on the variable
     * Parameters: bool rightshift - true for right shift, false for left shift
     *              UINT shiftCount - the number of bits to shoft by
     *              USHORT flags - flags that go into this shift
     * Returns: bool - whether the operation can be applied
     *
     * Note: Flags are as follows
     *
     *  0b0000000000000001 - Applies to boolean
     *  0b0000000000000010 - Applies to float
     *  0b0000000000000100 - Fill in-side with out-bit
     *  0b0000000000001000 - Cut down to 32 bits
     *  0b0000000000010000 - cut down to 16 bits
     *  0b0000000000011000 - cut down to 8 bits
     *  0b0000000000100000 - make unsigned
     *  0b0000000001100000 - make unsigned if float
     */
    bool BitShift(bool rightShift, UINT shiftCount, USHORT flags = 0);

    static TrecPointer<TVariable> GetFalse();
    static TrecPointer<TVariable> GetTrue();

private:
    /**
     * The actual value being held
     */
    ULONG64 value;
    /**
     * The type of data actually being stored
     */
    UCHAR type;

    TPrimitiveVariable();

protected:

public:
    /**
     * indicates this is a boolean variable
     */
    const static UCHAR type_bool = 0b00000001;
    /**
     * indicates this is a floating point
     */
    const static UCHAR type_float = 0b00000010;
    /**
     * indicates this is a char type
     */
    const static UCHAR type_char = 0b00000100;
    /**
     * indicates that it is an unsigned integer
     */
    const static UCHAR type_unsigned = 0b00001000;

    /**
     * indicates this is 1 byte
     */
    const static UCHAR type_one = 0b00010000;
    /**
     * indicates this is 2 bytes
     */
    const static UCHAR type_two = 0b00100000;
    /**
     * indicates this is 4 bytes
     */
    const static UCHAR type_four = 0b00110000;
    /**
     * indicates this is 8 bytes
     */
    const static UCHAR type_eight = 0b01000000;

    /*
     *  0b0000000000000001 - Applies to boolean
 *  0b0000000000000010 - Applies to float
 *  0b0000000000000100 - Fill in-side with out-bit
 *  0b0000000000001000 - Cut down to 32 bits
 *  0b0000000000010000 - cut down to 16 bits
 *  0b0000000000011000 - cut down to 8 bits
 *  0b0000000000100000 - make unsigned
 *  0b0000000001100000 - make unsigned if float
    */
    const static USHORT bit_bool = 0b0000000000000001;
    const static USHORT bit_float = 0b0000000000000010;
    const static USHORT bit_replenish = 0b0000000000000100;
    const static USHORT bit_to_32 = 0b0000000000001000;
    const static USHORT bit_to_16 = 0b0000000000010000;
    const static USHORT bit_to_8 = 0b0000000000011000;
    const static USHORT bit_to_un = 0b0000000000100000;
    const static USHORT bit_to_un_f = 0b0000000001100000;
};


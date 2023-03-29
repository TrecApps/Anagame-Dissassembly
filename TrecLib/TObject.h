#pragma once
#include "BaseTypes.h"
#include "TrecPointer.h"

class _TREC_LIB_DLL TObjectLocker
{
public:
	explicit TObjectLocker(ThreadBlocker* section);
	~TObjectLocker();

private:
	ThreadBlocker* section;
};


using var_type = enum class var_type
{
	expression,          // Can be Operated upon to generate a new variable
	primitive,           // Primitive variable is being held
	json,		          // Mapped Collection of Variables
	list,			 // Regular List
	native_object,       // Variable references a C++ level native Anagame object
	string,              // References a String
	procedure,			// Holds a list of instructions for a given type of Runner
	processor,
	accessor,            // Type of variable that holds a Getter and Setter method
	special_value,       // Holds a special value that doesn't fit in any of the other categories
	iterator,            // Serves as an iterator
	async,               // Serves as a brdge between threads
	runner,               // Function/Procedure that 'Runs' in some form
	type					// helps map out the type of variable being wirked with
};

class _TREC_LIB_DLL TObject: public TCoreObject
{
public:
	using ObjectType = enum class ObjectType
	{
		ot_variable,	// Contians some value
		ot_expression,	// Capable of Yielding some value
		ot_runner,		// Basic Runner (like an object based function)
		ot_compiler,	// Generates Runners
		ot_environment,	// Holds resources used by Other objects
		ot_handler,		// Handles events directed it's way
		ot_page,		// Represents something that can be drawn
		ot_page_builder,// Represents an Object that can build a Page
		ot_writer,		// Writes to some target
		ot_reader,		// Reads from some source
		ot_other		// Does not fit into the previous categories
	};

    class _TREC_LIB_DLL TVariable : public TCoreObject
    {
    public:
		TVariable();
		~TVariable() override;

        virtual TrecPointer<TVariable> Clone() = 0;

        virtual var_type GetVarType() = 0;

        virtual TrecPointer<TVariable> ToString() = 0;

        virtual TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail) = 0;

        virtual UINT Get4Value() = 0;

        virtual UINT GetSize() = 0;


        virtual void SetSelf(TrecPointer<TVariable> vSelf);

    protected:
        /**
         * Self Reference
         */
        TrecPointerSoft<TVariable> vSelf;
    };


protected:
	mutable ThreadBlocker thread;

public:

	virtual ObjectType GetObjectType();

	TObject();

	~TObject() override;

	void ThreadLock() const;

	void ThreadRelease()const;

	TrecPointer<TVariable> ToString(TrecPointer<TVariable>);
};

using TVariable = TObject::TVariable;
#pragma once
#include <memory>
#include "TrecLib.h"
#include "BaseTypes.h"

class _TREC_LIB_DLL TCoreObject
{
public:
	virtual ~TCoreObject();
};

class _TREC_LIB_DLL TrecBoxPointer
{
	friend class TrecPointerKey;
	friend class TrecPointerBase;
	


	/**
	 * The raw pointer to the actual object being used
	 */
	TCoreObject* rawPointer;

	/**
	 * tracks the number of TrecPointers referencing the object
	 */
	UINT counter;
	/**
	 * Keep Track of Soft pointers so they don't become dangling pointers
	 */
	USHORT softCounter;

};

class _TREC_LIB_DLL TrecPointerBase
{
	friend class TrecPointerKey;
protected:
	TrecBoxPointer* pointer;

	TrecPointerBase()
	{
		pointer = nullptr;
	}

	TCoreObject* GetObjectPointer()
	{
		return pointer ? pointer->rawPointer : nullptr;
	}

	TrecBoxPointer* RetrievePointer()
	{
		return pointer;
	}

	void SetNewPointer(TCoreObject* obj)
	{
		pointer = new TrecBoxPointer();
		pointer->rawPointer = obj;
		pointer->counter = 1;
		pointer->softCounter = 0;
	}

	bool Increment()
	{
		bool ret = (pointer && pointer->rawPointer);
		if (ret)
			pointer->counter++;
		return ret;
	}

	bool IncrementSoft()
	{
		bool ret = (pointer && pointer->rawPointer);
		if (ret)
			pointer->softCounter++;
		return ret;
	}

	void Decrement()
	{
		if (pointer && pointer->rawPointer && !(--pointer->counter))
		{
			delete pointer->rawPointer;
			pointer->rawPointer = nullptr;

			if (!pointer->softCounter)
				delete pointer;
		}
		pointer = nullptr;
	}

	void DecrementSoft()
	{
		if (pointer)
		{
			pointer->softCounter--;
			if (!pointer->softCounter && !pointer->rawPointer)
				delete pointer;
		}
		pointer = nullptr;
	}

	
};



template<class T> class _TREC_LIB_DLL TrecPointer : public TrecPointerBase
{
	friend class TrecPointerKey;
	// friend class TrecActivePointer;
	

	explicit TrecPointer(T* raw)
	{
		if (!dynamic_cast<TCoreObject*>(raw))
			throw 1;

		SetNewPointer(dynamic_cast<TCoreObject*>(raw));
	}

public:

	TrecPointer(const TrecPointer<T>& copy)
	{
		pointer = copy.pointer;
		Increment();
	}

	TrecPointer()
	{
		pointer = nullptr;
	}

	~TrecPointer()
	{
		// Make sure we decriment the coutner before deletion is complete
		Nullify();
	}

	void Nullify()
	{
		Decrement();
	}

	TrecPointer<T>& operator=(const TrecPointer<T>& other)
	{
		// First, make sure that the other TrecPointer isn't pointing to the same reference.
		// If it is, we don't want to decrement the reference lest it become 0 (and self-destructs)
		if (other.pointer == pointer)
			return *this;

		Decrement();

		pointer = other.pointer;

		Increment();
		return *this;
	}

	T* Get()
	{
		if (!pointer) return nullptr;
		return dynamic_cast<T*>(GetObjectPointer());
	}

	T* operator->()
	{
		auto ret = Get();
		if (!ret)
			throw 0;
		return ret;
	}
};


template<class T> class _TREC_LIB_DLL TrecPointerSoft : public TrecPointerBase
{
	friend class TrecPointerKey;


public:
	explicit TrecPointerSoft(const TrecPointer<T>& copy)
	{
		pointer = copy.pointer;
		IncrementSoft();
	}

	TrecPointerSoft(const TrecPointerSoft<T>& copy)
	{
		pointer = copy.pointer;
		IncrementSoft();
	}

	TrecPointerSoft()
	{
		pointer = nullptr;
	}

	~TrecPointerSoft()
	{
		// Make sure we decriment the coutner before deletion is complete
		Nullify();
	}

	void Nullify()
	{
		DecrementSoft();
	}

	void operator=(const TrecPointerSoft<T>& other)
	{
		// First, make sure that the other TrecPointer isn't pointing to the same reference.
		// If it is, we don't want to decrement the reference lest it become 0 (and self-destructs)
		if (other.pointer == pointer)
			return;

		DecrementSoft();

		pointer = other.pointer;
		IncrementSoft();
	}
};

template<class T> class _TREC_LIB_DLL TrecActivePointer
{
	friend class TrecPointerKey;
	TrecActivePointer() = default;
protected:
	TrecPointer<T> actPointer;
public:

	TrecPointer<T> GetTrecPointer()const 
	{
		return actPointer;
	}

	TrecActivePointer(const TrecActivePointer<T>& copy)
	{
		actPointer = copy.GetTrecPointer();

	}

	TrecActivePointer<T>& operator=(const TrecPointer<T>& other)
	{
		if (!other.Get())
			throw 0;
		actPointer = other;
		return *this;
	}


	TrecActivePointer(const TrecPointer<T>& copy)
	{
		auto temp = copy;
		if (!temp.Get())
			throw 0;
		actPointer = copy;
	}

};

class _TREC_LIB_DLL TrecPointerKey
{
public:

	template <class T> static TrecPointer<T> TrecFromSoft(TrecPointerSoft<T>& soft)
	{
		TrecPointer<T> ret;
		ret.pointer = soft.pointer;
		ret.Increment();
		return ret;
	}

	template <class T> static TrecPointerSoft<T> SoftFromTrec(TrecPointer<T>& soft)
	{
		TrecPointerSoft<T> ret;
		ret.pointer = soft.pointer;
		ret.IncrementSoft();
		return ret;
	}

	template <class T, class...types> static TrecPointer<T> GetNewSelfTrecPointer(types&& ... args)
	{
		T* raw = new T(args...);
		TrecPointer<T> ret(raw);
		raw->SetSelf(ret);
		return ret;
	}

	template <class t, class...types> static TrecPointer<t> GetNewTrecPointer(types&& ... args)
	{
		t* raw = new t(args...);
		TrecPointer<t> ret(raw);
		return ret;
	}

	template <class t, class u, class...types> static TrecPointer<t> GetNewSelfTrecPointerAlt(types&& ... args)
	{
		TrecPointer<t> ret = GetNewTrecPointerAlt<t, u>(args...);
		if (ret.Get())
			ret->SetSelf(ret);
		return ret;
	}

	/**
	 * Method: static TrecPointerKey::GetNewTrecPointerAlt
	 * Purpose: Creates an object of type u that is then stored in a TrecPointer of type T
	 * Parameters: types&& ... args - the arguments that get passed to the constructor of the object
	 * Returns: TrecPointer<t> - the T TrecPointer holding the U type
	 */
	template <class t, class u, class...types> static TrecPointer<t> GetNewTrecPointerAlt(types&& ... args)
	{
		u* raw = new u(args...);
		t* raw_t = nullptr;


		// Need to make sure that the alternative type 'u' is compatible with this type 't'

		raw_t = dynamic_cast<t*>(raw);


		if (!raw_t)
		{
			delete raw;
			throw 2;
		}
		TrecPointer<t> ret(raw_t);
		return ret;
	}

	template <class t, class u> static TrecPointer<u> ConvertPointer(TrecPointer<t> input)
	{
		t* rawIn = input.Get();

		TrecPointer<u> ret;

		if (dynamic_cast<u*>(rawIn))
		{
			ret.pointer = input.pointer;
			ret.Increment();
		}
		return ret;
	}


};

#pragma once
#include "TrecLib.h"
#include "TrecPointer.h"
#include <vector>
#include "BaseTypes.h"



/*
* Class: TDataArray (Regular Type version)
* Purpose: AnaGame array designed to hold raw data
*
* SuperClass: TDataArrayBase - common base class for all TDataArrays
*/
template<typename T> class _TREC_LIB_DLL TDataArray : public TCoreObject
{
	friend class TDataArray<T>;

	T* array;
	UINT size;
	UINT capacity;

public:

	UINT Size() const
	{
		return size;
	}

	UINT Capacity() const
	{
		return capacity;
	}

	TDataArray(const TDataArray<T>& newArray)//: TDataArrayBase()
	{
		UINT cap = newArray.Capacity();
		array = new T[cap];

		for (UINT c = 0; c < cap; c++)
		{
			if (c < cap)
				array[c] = newArray.array[c];
		}
		size = newArray.Size(), capacity = newArray.Capacity();
	}

	T* data() const
	{
		return array;
	}

	T& operator[](size_t c)const
	{
		if (c >= size)
			throw 2;
		return array[c];
	}

	TDataArray<T>& operator=(const TDataArray<T>& newArray)
	{
		if (array)
			delete[] array;
		capacity = newArray.Capacity();
		array = new T[capacity];
		size = newArray.Size();

		for (UINT c = 0; c < size && c < capacity; c++)
		{
			if (c < capacity)
				array[c] = newArray.array[c];
		}
		return *this;
	}

	T& at(UINT c)
	{
		if (c >= size)
			throw 2;
		return array[c];
	}

	const T& at_const(UINT c) const 
	{
		if (c >= size)
			throw 2;
		return array[c];
	}

	void operator=(const std::vector<T, std::allocator<T>>& vectorSource)
	{
		if (array)
			delete[] array;
		capacity = vectorSource.capacity();
		array = new T[capacity];
		size = vectorSource.size();
		for (UINT c = 0; c < vectorSource.capacity(); c++)
			array[c] = vectorSource[c];
	}

	TDataArray()
	{
		array = new T[5];
		size = 0; 
		capacity = 5;
	}

	virtual ~TDataArray()
	{
		delete[] array;
	}

	UINT push_back(T element)
	{
		if (size >= capacity)
		{
			T* newArray = new T[capacity + 5];
			for (UINT c = 0; c < capacity; c++)
				newArray[c] = array[c];
			
			delete[] array;
			array = newArray;
			capacity += 5;
		}
		array[size++] = element;
		return size - 1;
	}

	UINT InsertAt(T element, UINT index)
	{
		if (index >= size)
			return push_back(element);
		UINT ret = push_back(element);

		for (UINT Rust = size - 1; Rust > index; Rust--)
		{
			array[Rust] = array[Rust - 1];
		}
		array[index] = element;

		return ret;
	}

	T RemoveAt(UINT c)
	{
		if (c >= size)
			throw 2;
		T returnable = array[c];

		for (; c < size - 1; c++)
			array[c] = array[c + 1];
		size--;
		// Set to the Default
		array[size] = T();
		return returnable;
	}

	void RemoveAll()
	{
		if (array)
			delete[] array;
		array = new T[capacity = 5];
		size = 0;
	}

};

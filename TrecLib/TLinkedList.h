#pragma once
#include "TrecLib.h"
#include "TObject.h"
#include "TrecPointer.h"


template<typename T> class _TREC_LIB_DLL ListNode : public TCoreObject
{
public:
	TrecPointer<ListNode<T>> next;
	TrecPointerSoft<ListNode<T>> prev;
	T data;
};

/**
 * Class: TLinkedList
 * Purpose: Template for actual linked lists in Anagame
 * 
 * SuperClass: TLinkedListBase
 */
template<typename T> class _TREC_LIB_DLL TLinkedList 
{
	UINT size;
	UINT currentNode;

	TrecPointer<ListNode<T>> head;
	TrecPointer<ListNode<T>> current;
	TrecPointer<ListNode<T>> tail;

public:
	TLinkedList()
	{
		size = currentNode = 0;
	}

	UINT GetSize() const
	{
		return size;
	}
	UINT GetCurrentIndex() const
	{
		return currentNode;
	}

	UINT Push(T& newData)
	{
		TrecPointer<ListNode<T>> node = TrecPointerKey::GetNewTrecPointer<ListNode<T>>();
		node->data = newData;

		if (!size)
		{
			head = tail = current = node;
		}
		else
		{
			tail->next = node;
			node->prev = TrecPointerKey::SoftFromTrec<ListNode<T>>(tail);
			tail = node;
		}
		size++;
		return size;
	}

	T& at(UINT index)
	{
		if (index >= size)
			throw 2;

		while (currentNode > index)
			Prev();

		while (currentNode < index)
			Next();

		return current->data;
	}

	T Extract(UINT index)
	{
		auto ret = at(index);

		if (current.Get() == head.Get())
			DropHead();
		else if (current.Get() == tail.Get())
			DropTail();
		else
		{
			auto p = TrecPointerKey::TrecFromSoft<>(current->prev);
			auto n = current->next;

			p->next = n;
			n->prev = TrecPointerKey::SoftFromTrec<>(p);
			current = n;
			size--;
		}

		return ret;
	}

	bool Inject(T& newData, UINT index)
	{
		if (index > size)
			return false;

		while (currentNode > index)
			Prev();

		while (currentNode < index && Next());
		

		if (head.Get() == tail.Get() || current.Get() == tail.Get())
			PushHead(newData);
		else
		{
			auto n = current->next;
			TrecPointer<ListNode<T>> node = TrecPointerKey::GetNewTrecPointer<ListNode<T>>();
			node->data = newData;

			node->next = n;
			n->prev = TrecPointerKey::SoftFromTrec<>(node);

			node->prev = TrecPointerKey::SoftFromTrec<>(current);
			current->next = node;
			size++;
		}
		return true;
	}

	bool Update(T& newData, UINT index)
	{
		if (index > size)
			return false;

		while (currentNode > index)
			Prev();

		while (currentNode < index && Next());

		current->data = newData;
	}

	UINT PushHead(T& newData)
	{
		TrecPointer<ListNode<T>> node = TrecPointerKey::GetNewTrecPointer<ListNode<T>>();
		node->data = newData;

		if (!size)
		{
			head = tail = current = node;
		}
		else
		{
			head->prev = TrecPointerKey::SoftFromTrec<>( node);
			node->next = head;
			head = node;
		}
		size++;
		return size;
	}

	TrecPointer<ListNode<T>> GetCurrentNode()
	{
		return current;
	}

	bool Dequeue(T& data)
	{
		if (head.Get())
		{
			data = head->data;
			DropHead();
			return true;
		}
		else
			return false;
	}

	bool DequeueTail(T& data)
	{
		if (tail.Get())
		{
			data = tail->data;
			DropTail();
			return true;
		}
		else
			return false;
	}

	void Flush() {
		head = current;
		size -= currentNode;
		currentNode = 0;
	}

	void DropHead()
	{
		if (!size)
			return;
		if (head.Get() == tail.Get())
		{
			// Size is one, nullify all pointers and set size to 0
			head.Nullify();
			tail.Nullify();
			current.Nullify();
			size = currentNode = 0;
			return;
		}

		if (head.Get() == current.Get())
			current = current->next;
		else
			currentNode--;
		head = head->next;
		size--;
	}

	void DropTail()
	{
		if (!size)
			return;
		if (head.Get() == tail.Get())
		{
			// Size is one, nullify all pointers and set size to 0
			head.Nullify();
			tail.Nullify();
			current.Nullify();
			size = currentNode = 0;
			return;
		}

		if (tail.Get() == current.Get())
			current = TrecPointerKey::TrecFromSoft<>(current->prev);
		
		TrecPointer<ListNode<T>> tailPrev = TrecPointerKey::TrecFromSoft<>(tail->prev);

		if (tailPrev.Get())
			tailPrev->next.Nullify();
		
		tail = tailPrev;
		size--;
	}

	bool Next()
	{
		if (!size)
			return false;
		if (current.Get() == tail.Get())
			return false;
		current = current->next;
		currentNode++;
		return true;
	}

	bool Prev()
	{
		if (!size || !currentNode)
			return false;
		TrecPointerSoft< ListNode<T>> p = current->prev;
		current = TrecPointerKey::TrecFromSoft<>(p);
		return true;
	}
	
	

};

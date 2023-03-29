#include "TObjectNode.h"


/// 
/// Methods for the Base Node Class
/// 

TObjectNode::TObjectNode(UINT l): level(l)
{
}



TString TObjectNode::GetCommand(const TString& info)
{
	UNREFERENCED_PARAMETER(info);
	return L"";
}

UINT TObjectNode::GetLevel()
{
	return level;
}

void TObjectNode::SetParent(TrecPointerSoft<TObjectNode> p)
{
	ThreadLock();
	parentNode = p;
	ThreadRelease();
}

void TObjectNode::SetSelf(TrecPointer<TObjectNode> s)
{
	if (s.Get() != this)
		throw 0;
	self = TrecPointerKey::SoftFromTrec<TObjectNode>(s);
}


/// 
/// Methods for the Blank Node Class
/// 

TBlankNode::TBlankNode(UINT l) : TObjectNode(l)
{
	isExtended = false;
}

TString TBlankNode::GetContent()
{
	TObjectLocker lock(&thread);
		TString ret(content);
	return ret;
}

bool TBlankNode::IsExtendable()
{
	return nodes.Size();
}

bool TBlankNode::IsExtended()
{
	return isExtended;
}

bool TBlankNode::Initialize()
{
	return true;
}

TrecPointer<TObjectNode> TBlankNode::GetNodeAt(UINT target, UINT current)
{
	if (target == current)
		return TrecPointerKey::TrecFromSoft<TObjectNode>(self);

	if (target < current)
		return TrecPointer<TObjectNode>();

	TrecPointer<TObjectNode> ret;

	current++;
	TObjectLocker lock(&thread);
		for (UINT rust = 0; isExtended && rust < nodes.Size(); rust++)
		{
			ret = nodes[rust]->GetNodeAt(target, current);
			if (ret.Get())
			{
				return ret;
			}
			current += nodes[rust]->TotalChildren() + 1;
		}
	return TrecPointer<TObjectNode>();
}

bool TBlankNode::Initialize(TString& value)
{
	TObjectLocker lock(&thread);
		if (content.GetSize())
			return false;
	content.Set(value);
	return true;
}

void TBlankNode::Extend()
{
	isExtended = true;
}

TrecPointer<TObjectNode> TBlankNode::GetChildNodes(UINT index)
{
	TObjectLocker lock(&thread);
		if (index < nodes.Size())
		{
			TrecPointer<TObjectNode> ret = nodes[index];
			return ret;
		}
	return TrecPointer<TObjectNode>();
}

void TBlankNode::DropChildNodes()
{
	isExtended = false;
}

void TBlankNode::AddNode(TrecPointer<TObjectNode> node)
{
	TObjectLocker lock(&thread);
		if (node.Get())
			nodes.push_back(node);
	return;
}

bool TBlankNode::RemoveNode(TrecPointer<TObjectNode> obj)
{
	if (!obj.Get())
		return false;
	TObjectLocker lock(&thread);
		for (UINT Rust = 0; Rust < nodes.Size(); Rust++)
		{
			auto f = nodes[Rust].Get();
			if (!f)
				continue;

			if (f == obj.Get())
			{
				nodes.RemoveAt(Rust);
				return true;
			}

			if (f->RemoveNode(obj))
				return true;
		}
	return false;
}

UINT TBlankNode::TotalChildren()
{
	UINT ret = 0;
	TObjectLocker lock(&thread);
		for (UINT rust = 0; rust < nodes.Size(); rust++)
		{
			ret += nodes[rust]->TotalChildren() + 1;
		}
	return ret;
}
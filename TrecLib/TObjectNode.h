#pragma once
#include "TString.h"
class _TREC_LIB_DLL TObjectNode :
    public TObject
{
public:
	explicit TObjectNode(UINT l);

	~TObjectNode()override = default;

	virtual TString GetContent() = 0;

	virtual TString GetCommand(const TString& info);

	virtual bool IsExtendable() = 0;

	virtual bool IsExtended() = 0;

	virtual bool Initialize() = 0;

	virtual TrecPointer<TObjectNode> GetNodeAt(UINT target, UINT current) = 0;

	virtual UINT TotalChildren() = 0;

	virtual bool Initialize(TString& value) = 0;

	virtual void Extend() = 0;

	virtual TrecPointer<TObjectNode> GetChildNodes(UINT) = 0;

	virtual void DropChildNodes() = 0;

	UINT GetLevel();

	void SetParent(TrecPointerSoft<TObjectNode> p);

	void SetSelf(TrecPointer<TObjectNode> s);

	virtual bool RemoveNode(TrecPointer<TObjectNode> obj) = 0;


protected:
	/**
	 * The distance from the root (0 means this is the root node)
	 */
	UINT level;

	/**
	 * Reference to the parent node
	 */
	TrecPointerSoft<TObjectNode> parentNode;
	/**
	 * Reference to self
	 */
	TrecPointerSoft<TObjectNode> self;
};

class _TREC_LIB_DLL TBlankNode : public TObjectNode
{
public:
	explicit TBlankNode(UINT l);

	~TBlankNode() override = default;

	TString GetContent()override;

	bool IsExtendable()override;

	bool IsExtended()override;

	bool Initialize()override;

	TrecPointer<TObjectNode> GetNodeAt(UINT target, UINT current)override;

	UINT TotalChildren()override;

	bool Initialize(TString& value)override;
	
	void Extend()override;

	TrecPointer<TObjectNode> GetChildNodes(UINT)override;

	void DropChildNodes()override;

	void AddNode(TrecPointer<TObjectNode> node);

	bool RemoveNode(TrecPointer<TObjectNode> obj) override;

protected:

	/**
	 * The 'content' of this node
	 */
	TString content;

	/**
	 * because this is blank, this is an easier way to report whether the node is extended or not
	 */
	bool isExtended;

	/**
	 * list of files held by this node (if object is a TDirectory
	 */
	TDataArray<TrecPointer<TObjectNode>> nodes;

};
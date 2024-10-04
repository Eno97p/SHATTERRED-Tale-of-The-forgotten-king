#pragma once
#include"CNode.h"
BEGIN(Engine)
class CSequence final : public CNode
{
private:
	CSequence();
	virtual ~CSequence() = default;
public:
	HRESULT Initialize();
	virtual void Add_Nodes(CNode* pnode) override {
		m_nodes.push_back(pnode);
	}
private:
	vector<CNode*> m_nodes;
public:
	static CSequence* create();
	virtual NodeStates Evaluate(_float fTimeDelta) override;

	virtual void Free() override;
};

END
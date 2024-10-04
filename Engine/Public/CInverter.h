#pragma once
#include"CNode.h"
BEGIN(Engine)
class CInverter final : public CNode
{
private:
	CInverter();
	virtual ~CInverter() = default;
public:
	HRESULT Initialize();
	CNode* GetNode() const { return m_Node; }
	virtual void Add_Nodes(CNode* pnode) override
	{
		m_Node = pnode;
	}
private:
	CNode* m_Node = nullptr;
public:
	static CInverter* Create();
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;
};
END

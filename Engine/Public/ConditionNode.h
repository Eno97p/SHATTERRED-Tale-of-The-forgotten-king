#pragma once
#include "CNode.h"
BEGIN(Engine)

class CConditionNode : public CNode
{
private:
	CConditionNode();
	virtual ~CConditionNode() = default;
public:
	HRESULT Initialize(_bool* bCondition);
	virtual void Add_Nodes(CNode* pnode) override
	{
		if (m_Node == nullptr)
			m_Node = pnode;
		else
		{
			MSG_BOX("Condition ���� �ϳ��� �ڽĸ� ���� �� �ֽ��ϴ�.");
			return;
		}
	}
private:
	CNode* m_Node = nullptr;
private:
	_bool* m_bCondition = nullptr;
public:
	static CConditionNode* Create(_bool* bCondition);
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;

};

END
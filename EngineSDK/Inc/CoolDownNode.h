#pragma once
#include "CNode.h"

BEGIN(Engine)
class CCoolDownNode : public CNode
{
private:
	CCoolDownNode();
	virtual ~CCoolDownNode() = default;
public:
	HRESULT Initialize(_float fCoolTime);
	virtual void Add_Nodes(CNode* pnode) override
	{
		if(m_Node == nullptr)
			m_Node = pnode;
		else
		{
			MSG_BOX("CoolDown ���� �ϳ��� �ڽĸ� ���� �� �ֽ��ϴ�.");
			return;
		}
	}
private:
	CNode* m_Node = nullptr;
private:
	_float m_MaxTime = 0.f;			// ��ٿ� �ð�
	_float m_CurrentCooldown = 0.f; // ���� ��ٿ� �ð�
public:
	static CCoolDownNode* Create(_float fCoolTime);
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;

};

END
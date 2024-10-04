#pragma once
#include "CNode.h"

BEGIN(Engine)
class CCoolDownPriority : public CNode
{
private:
	CCoolDownPriority();
	virtual ~CCoolDownPriority() = default;
public:
	HRESULT Initialize(_float fCoolTime, _float FirstCool);
	virtual void Add_Nodes(CNode* pnode) override
	{
		if(m_Node == nullptr)
			m_Node = pnode;
		else
		{
			MSG_BOX("CoolDown 노드는 하나의 자식만 가질 수 있습니다.");
			return;
		}
	}
private:
	CNode* m_Node = nullptr;
private:
	_float m_MaxTime = 0.f;			// 쿨다운 시간
	_float m_CurrentCooldown = 0.f; // 현재 쿨다운 시간
public:
	static CCoolDownPriority* Create(_float fCoolTime, _float FirstCool);
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;

};

END
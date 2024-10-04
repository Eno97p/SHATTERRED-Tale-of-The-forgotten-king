#pragma once

#include "Base.h"

BEGIN(Engine)
class CNode abstract : public CBase
{
protected:
	CNode(){};
	virtual ~CNode() = default;

protected:
	NodeStates m_eNodeState = NONE;
public:
	virtual void Add_Nodes(CNode* pnode);
	virtual NodeStates Evaluate(_float fTimeDelta) = 0;

	virtual void Free()override {};
};

END

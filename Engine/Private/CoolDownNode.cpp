#include "CoolDownNode.h"

CCoolDownNode::CCoolDownNode()
{
}

HRESULT CCoolDownNode::Initialize(_float fCoolTime)
{
	m_MaxTime = fCoolTime;
	return S_OK;
}



NodeStates CCoolDownNode::Evaluate(_float fTimeDelta)
{
	if (m_CurrentCooldown > 0.0f)
	{
		m_CurrentCooldown -= fTimeDelta;
		m_eNodeState = NodeStates::COOLING; // 쿨타임 중엔 COOLING을 반환하게된다.
		return m_eNodeState;
	}


	switch (m_Node->Evaluate(fTimeDelta))
	{
	case NodeStates::FAILURE:
		m_eNodeState = NodeStates::FAILURE;
		return m_eNodeState;
	case NodeStates::SUCCESS:
		m_CurrentCooldown = m_MaxTime;
		m_eNodeState = NodeStates::SUCCESS;
		return m_eNodeState;
	case NodeStates::RUNNING:
		m_eNodeState = NodeStates::RUNNING;
		return m_eNodeState;
	case NodeStates::COOLING:
		m_eNodeState = NodeStates::COOLING;
		return m_eNodeState;
	default:
		m_eNodeState = NodeStates::FAILURE;
		return m_eNodeState;
	}

}

CCoolDownNode* CCoolDownNode::Create(_float fCoolTime)
{
	CCoolDownNode* pInstance = new CCoolDownNode();
	if (FAILED(pInstance->Initialize(fCoolTime)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CCoolDownNode");
		return nullptr;
	}
	return pInstance;
}

void CCoolDownNode::Free()
{
	Safe_Release(m_Node);
}

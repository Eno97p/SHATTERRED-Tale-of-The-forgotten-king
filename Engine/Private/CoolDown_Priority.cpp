#include "CoolDown_Priority.h"

CCoolDownPriority::CCoolDownPriority()
{
}

HRESULT CCoolDownPriority::Initialize(_float fCoolTime, _float FirstCool)
{
	m_MaxTime = fCoolTime;
	m_CurrentCooldown = FirstCool;
	return S_OK;
}



NodeStates CCoolDownPriority::Evaluate(_float fTimeDelta)
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

CCoolDownPriority* CCoolDownPriority::Create(_float fCoolTime, _float FirstCool)
{
	CCoolDownPriority* pInstance = new CCoolDownPriority();
	if (FAILED(pInstance->Initialize(fCoolTime, FirstCool)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CCoolDownPriority");
		return nullptr;
	}
	return pInstance;
}

void CCoolDownPriority::Free()
{
	Safe_Release(m_Node);
}

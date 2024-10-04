#include "ConditionNode.h"

CConditionNode::CConditionNode()
{
}

HRESULT CConditionNode::Initialize(_bool* bCondition)
{
	m_bCondition = bCondition;
	return S_OK;
}

NodeStates CConditionNode::Evaluate(_float fTimeDelta)
{
	if(m_bCondition == nullptr)
	{
		m_eNodeState = NodeStates::FAILURE;
		return m_eNodeState;
	}


	if (*m_bCondition == false)
	{
		m_eNodeState = NodeStates::FAILURE;
		return m_eNodeState;
	}
	else
	{
		switch (m_Node->Evaluate(fTimeDelta))
		{
		case NodeStates::FAILURE:
			m_eNodeState = NodeStates::FAILURE;
			return m_eNodeState;
		case NodeStates::SUCCESS:
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
}

CConditionNode* CConditionNode::Create(_bool* bCondition)
{
	CConditionNode* pInstance = new CConditionNode();
	if (FAILED(pInstance->Initialize(bCondition)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CConditionNode");
		return nullptr;
	}
	return pInstance;
}

void CConditionNode::Free()
{
	Safe_Release(m_Node);
}

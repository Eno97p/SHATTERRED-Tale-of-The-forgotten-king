
#include "CActionNode.h"

CActionNode::CActionNode(function<NodeStates(_float fTimeDelta)> action)
    : m_action(action)
{
}

HRESULT CActionNode::Initialize()
{
	return S_OK;
}


NodeStates CActionNode::Evaluate(_float fTimeDelta)
{

	switch (m_action(fTimeDelta))
	{
		case NodeStates::SUCCESS:
			m_eNodeState = NodeStates::SUCCESS;
			return m_eNodeState;
		case NodeStates::FAILURE:
			m_eNodeState = NodeStates::FAILURE;
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
CActionNode* CActionNode::create(function<NodeStates(_float fTimeDelta)> action)
{
    CActionNode* pInstance = new CActionNode(action);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CActionNode");
		return nullptr;
	}
	return pInstance;

}



void CActionNode::Free()
{
}

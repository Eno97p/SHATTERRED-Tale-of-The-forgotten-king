
#include "CSequence.h"

CSequence::CSequence()
{
}

HRESULT CSequence::Initialize()
{

	return S_OK;
}



NodeStates CSequence::Evaluate(_float fTimeDelta)
{

	_bool bChildRunning = false;
	for(auto& node : m_nodes)
	{
		switch (node->Evaluate(fTimeDelta))
		{
		case NodeStates::SUCCESS:
			continue;
		case NodeStates::FAILURE:
			m_eNodeState = NodeStates::FAILURE;
			return m_eNodeState;
		case NodeStates::RUNNING:
			bChildRunning = true;
			continue;
		case NodeStates::COOLING:
			continue;
		default:
			m_eNodeState = NodeStates::SUCCESS;
			return m_eNodeState;
		}

	}
	m_eNodeState = bChildRunning ? NodeStates::RUNNING : NodeStates::SUCCESS;

return m_eNodeState;
	
}


CSequence* CSequence::create()
{
	CSequence* pInstance = new CSequence();
	if(FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CSequence");
		return nullptr;
	}
	return pInstance;
}

void CSequence::Free()
{
	for (auto& node : m_nodes)
	{
		Safe_Release(node);
	}
	m_nodes.clear();
}

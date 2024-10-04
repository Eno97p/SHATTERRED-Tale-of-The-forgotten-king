
#include "CSelector.h"

CSelector::CSelector()
{
}

HRESULT CSelector::Initialize()
{
	return S_OK;
}



NodeStates CSelector::Evaluate(_float fTimeDelta)
{


	for (auto& node : m_nodes)
	{
		switch (node->Evaluate(fTimeDelta))
		{
		case NodeStates::FAILURE:
			continue;
		case NodeStates::SUCCESS:
			m_eNodeState = NodeStates::SUCCESS;
			return m_eNodeState;             //자식 노드들 중 하나라도 ture이면 Selector도 true를 리턴. 그리고 바로 종료
		case NodeStates::RUNNING:
			m_eNodeState = NodeStates::RUNNING;
			return m_eNodeState;
		case NodeStates::COOLING:
			continue;
		default:
			continue;
		}

	}
	m_eNodeState = NodeStates::FAILURE;
	return m_eNodeState;

}

CSelector* CSelector::Create()
{
	CSelector* pInstance = new CSelector();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CSequence");
		return nullptr;
	}
	return pInstance;
}



void CSelector::Free()
{
	for(auto& node : m_nodes)
	{
		Safe_Release(node);
	}
	m_nodes.clear();
}

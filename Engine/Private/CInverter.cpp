
#include "CInverter.h"

CInverter::CInverter()
{
}

HRESULT CInverter::Initialize()
{
    return S_OK;
}

NodeStates CInverter::Evaluate(_float fTimeDelta)
{
	switch (m_Node->Evaluate(fTimeDelta))
	{
		// 자식노드의 반대로 리턴
	case NodeStates::FAILURE:
		m_eNodeState = NodeStates::SUCCESS;
		return m_eNodeState;
	case NodeStates::SUCCESS:
		m_eNodeState = NodeStates::FAILURE;
		return m_eNodeState;
	case NodeStates::RUNNING:
		m_eNodeState = NodeStates::RUNNING;
		return m_eNodeState;
	case NodeStates::COOLING:
		m_eNodeState = NodeStates::COOLING;
		return m_eNodeState;
	}
	m_eNodeState= NodeStates::SUCCESS;
	return m_eNodeState;

}

CInverter* CInverter::Create()
{
	CInverter* pInstance = new CInverter();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CInverter");
		return nullptr;
	}
	return pInstance;

}


void CInverter::Free()
{
	Safe_Release(m_Node);
}

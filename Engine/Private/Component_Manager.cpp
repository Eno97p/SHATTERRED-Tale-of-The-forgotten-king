#include "..\Public\Component_Manager.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	//원형이 이미 있으면 들어온 데이터를 지우고 종료
	if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
	{
		Safe_Release(pPrototype);
		auto& Temp = m_pPrototypes[iLevelIndex];
		return S_OK;
	}

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	auto& Temp = m_pPrototypes[iLevelIndex];


	int temp = 0;

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	CComponent*	pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);

}

CComponent* CComponent_Manager::Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;
	auto& pComponent = m_pPrototypes[iLevelIndex];

	for(auto& Pair : pComponent)
		Safe_Release(Pair.second);

	//for (auto& Pair : m_pPrototypes[iLevelIndex])
	//	Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	auto&   temp = m_pPrototypes[iLevelIndex];
	
	

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

_bool CComponent_Manager::IsPrototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return false;

	return true;
}

CComponent_Manager * CComponent_Manager::Create(_uint iNumLevels)
{
	CComponent_Manager*		pInstance = new CComponent_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed To Created : CComponent_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComponent_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}


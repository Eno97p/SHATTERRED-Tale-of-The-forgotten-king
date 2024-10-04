#include "..\Public\Layer.h"
#include "GameObject.h"
#include "GameInstance.h"


#include"CWorker.h"

CLayer::CLayer()
{

}

CComponent * CLayer::Get_Component(const wstring & strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_Component(strComponentTag);	
}

CGameObject * CLayer::Get_Object(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return *iter;
}

list<class CGameObject* > CLayer::Get_GameObjects_Ref()
{
	//if (m_GameObjects.empty())
	//	return 

	return m_GameObjects;
		

}

HRESULT CLayer::Initialize()
{


	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Delete_All()
{
	if (m_GameObjects.size() < 1)
		return;

	for (auto& iter : m_GameObjects)
	{
		Safe_Release(iter);
	}
	m_GameObjects.clear();
}

void CLayer::Clean()
{
	for (auto& iter : m_GameObjects)
	{
		CGameInstance::GetInstance()->Erase(iter);
	}
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead() == true)
			continue;
		pGameObject->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{


	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead() == true)
			continue;
		pGameObject->Tick(fTimeDelta);
	}

}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead() == true)
			continue;
		pGameObject->Late_Tick(fTimeDelta);
	}
}

void CLayer::Delete_Object(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	for (auto& iter : m_GameObjects)
	{
		if (iter == pGameObject)
		{
			Safe_Release(iter);
			m_GameObjects.remove(iter);
			break;
		}
	}
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Layer.h"
//#include "GameInstance.h"

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if(nullptr == pLayer)	
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

CGameObject * CObject_Manager::Get_Object(_uint iLevelIndex, const wstring & strLayerTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;
	return pLayer->Get_Object(iIndex);
}

list<CGameObject*> CObject_Manager::Get_GameObjects_Ref(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return list<class CGameObject*>();

	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return list<class CGameObject*>();

	return  pLayer->Get_GameObjects_Ref();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const wstring, class CLayer*>[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	//원형이 있으면 들어온 데이터를 지우고 종료
	if (nullptr != Find_Prototype(strPrototypeTag))
	{
		Safe_Release(pPrototype);
		return S_OK;
	}

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	
	pCloneObject->Set_ProtoTypeTag(strPrototypeTag);
	
	CLayer*				pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);	

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		m_iNumLayers++;
	}
	/* 내가 추가할려가하ㅡㄴㄴ 레이어가 있었다!!  */
	else
		pLayer->Add_GameObject(pCloneObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);
		m_Cameras.emplace_back(dynamic_cast<CCamera*>(pCloneObject));
		Safe_AddRef(pCloneObject);
		Set_MainCamera(m_Cameras.size() - 1);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		++m_iNumLayers;
	}
	/* 내가 추가할려가하ㅡㄴㄴ 레이어가 있었다!!  */
	else
	{
		pLayer->Add_GameObject(pCloneObject);
		m_Cameras.emplace_back(dynamic_cast<CCamera*>(pCloneObject));
		Safe_AddRef(pCloneObject);
		Set_MainCamera(m_Cameras.size() - 1);
	}

	return S_OK;
}

void CObject_Manager::Set_MainCamera(_uint iCameraIndex)
{
	if (iCameraIndex >= m_Cameras.size() || iCameraIndex < 0)
	{
		MSG_BOX("Wrong Camera Index");
		return;
	}

	for (auto& camera : m_Cameras)
	{
		camera->Inactivate();
	}

	m_Cameras[iCameraIndex]->Activate();

	m_iMainCameraIdx = iCameraIndex;

	return;
}

CCamera* CObject_Manager::Get_MainCamera()
{
	return m_Cameras[m_iMainCameraIdx];
}

vector<class CCamera*> CObject_Manager::Get_Cameras()
{
	return m_Cameras; 
}

void CObject_Manager::Clear_Cameras()
{
	for (auto& camera : m_Cameras)
	{
		Safe_Release(camera);
	}
	m_Cameras.clear();
}




CGameObject * CObject_Manager::Clone_Object(const wstring & strPrototypeTag, void * pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*		pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return nullptr;

	return pCloneObject;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Tick(fTimeDelta);
	}

	
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		auto& Layers = m_pLayers[i];
		
		for (auto& Pair : Layers)
		{
			//CGameInstance::GetInstance()->AddWork([Pair, fTimeDelta]() {
			//	Pair.second->Tick(fTimeDelta); });
			string test = wstring_to_string(Pair.first);
			PROFILE_CALL(test, Pair.second->Tick(fTimeDelta));

			//Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
#ifdef _DEBUG
#else
		Garbage_Collection(&m_pLayers[i]);
#endif // _DEBUG

	}
	

}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;
	/*Current Layer For.Debug*/
	auto& Layer = m_pLayers[iLevelIndex];
	for (auto& Pair : Layer)
	{
		Safe_Release(Pair.second);
	}

	//for (auto& Pair : m_pLayers[iLevelIndex])
	//{
	//	Safe_Release(Pair.second);
	//}
	m_pLayers[iLevelIndex].clear();
}

HRESULT CObject_Manager::Delete_Event(vector<class CGameObject*> DeadObjs)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& dead : DeadObjs)
		{
			for (auto& Pair : m_pLayers[i])
				Pair.second->Delete_Object(dead);
		}
	}

	return S_OK;
}

void CObject_Manager::Add_Object(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		m_iNumLayers++;
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}
}

void CObject_Manager::Delete_All(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (pLayer == nullptr)
		return;
	pLayer->Delete_All();
}

void CObject_Manager::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (pLayer == nullptr)
		return;
	pLayer->Clean();
}

void CObject_Manager::Get_LayerTags_String(_uint iLevelIndex, vector<const char*>* vecRefLayerName)
{
	//vecRefLayerName->clear();
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (const char* str : *vecRefLayerName)
	{
		delete[] str;
		str = nullptr;
	}
	vecRefLayerName->clear();

	for (const auto& pair : m_pLayers[iLevelIndex])
	{
		const std::wstring& wstr = pair.first;
		std::string str = wstring_to_string(wstr);

		char* strLayerTag = new char[str.length() + 1];
		strcpy_s(strLayerTag, str.length() + 1, str.c_str());

		vecRefLayerName->push_back(strLayerTag);
	}
}



void CObject_Manager::Garbage_Collection(map<const wstring, class CLayer*>* Layer)
{
	for (auto& it = Layer->begin(); it != Layer->end(); )
	{
		if (it->second->Get_LayerSize() == 0)
		{
			Safe_Release(it->second);
			it = Layer->erase(it);
			m_iNumLayers--;
		}
		else
			++it;
	}
}

CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring & strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed To Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	for (auto& camera : m_Cameras)
	{
		Safe_Release(camera);
	}
	m_Cameras.clear();

	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}

extern "C"
{

	void Free_LayerTags_String(std::vector<const char*>* vecRefLayerName)
	{
		for (const char* str : *vecRefLayerName)
		{
			delete[] str;
			str = nullptr;
		}
		vecRefLayerName->clear();

	}

	
}
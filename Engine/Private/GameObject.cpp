#include "..\Public\GameObject.h"
#include "GameInstance.h"

const _tchar* CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CComponent * CGameObject::Get_Component(const wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag.c_str());
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Delete_Component(const wstring& strComponentTag)
{
	auto iter = m_Components.find(strComponentTag.c_str());
	if (iter == m_Components.end())
		return E_FAIL;

	Safe_Release((*iter).second);
	m_Components.erase(iter);

	return S_OK;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr != pArg)
	{
	
		
		

		
	/*	if (((GAMEOBJECT_DESC*)pArg)->pModelName)
		{
			if(sizeof(((GAMEOBJECT_DESC*)pArg)->pModelName)==sizeof(const char*))
				m_wstrMoDelName= const_char_to_wstring(((GAMEOBJECT_DESC*)pArg)->pModelName);
		}*/
		

		// m_iData = ((GAMEOBJECT_DESC*)pArg)->iData;
		//GAMEOBJECT_DESC* desc = ((GAMEOBJECT_DESC*)pArg);
			
		/*if (nullptr != &desc->mWorldMatrix)
		{
			m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&desc->mWorldMatrix));
		}*/
		
		
	}

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(m_pTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Tick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

_float CGameObject::Get_LengthFromCamera()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = m_pGameInstance->Get_CamPosition();
	return XMVectorGetX(XMVector3Length(vPos - vCamPos));
}

_float4x4 CGameObject::Get_worldMat()
{
	_float4x4 matWorld = {};
	XMStoreFloat4x4(&matWorld, m_pTransformCom->Get_WorldMatrix());

	return matWorld;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const wstring& strPrototypeTag, const wstring & strComponentTag, CComponent** ppOut, void* pArg)
{
	if (m_Components.end() != m_Components.find(strComponentTag))
		return E_FAIL;

	//LEVEL_STATIC 제외하고 알아서 자기 레벨 받아오게함.
	if (iPrototypeLevelIndex != 0) iPrototypeLevelIndex = m_pGameInstance->Get_CurrentLevel();
	
	CComponent*	pComponent = m_pGameInstance->Clone_Component(iPrototypeLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;	

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	//if (m_pDesc)
	//{
	//	free(m_pDesc);
	//	m_pDesc = nullptr;
	//}
}

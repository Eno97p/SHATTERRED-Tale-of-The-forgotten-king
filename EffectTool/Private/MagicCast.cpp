#include "MagicCast.h"
#include "GameInstance.h"
CMagicCast::CMagicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CMagicCast::CMagicCast(const CMagicCast& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMagicCast::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagicCast::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_unique<MAGIC_CAST>(*((MAGIC_CAST*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix PMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, PMat.r[3]);

	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

	return S_OK;

}

void CMagicCast::Priority_Tick(_float fTimeDelta)
{
}

void CMagicCast::Tick(_float fTimeDelta)
{
}

void CMagicCast::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMagicCast::Add_Components()
{
	return S_OK;
}

HRESULT CMagicCast::Add_Child_Effects()
{
	if (m_OwnDesc->isHelix)
	{
		m_OwnDesc->HelixDesc.ParentMatrix = m_OwnDesc->ParentMatrix;
		m_OwnDesc->HelixDesc.fMaxLifeTime = m_OwnDesc->fMaxLifeTime;
		m_OwnDesc->HelixDesc.fThreadRatio = m_OwnDesc->fThreadRatio;
		m_OwnDesc->HelixDesc.fSlowStrength = m_OwnDesc->fSlowStrength;

		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_HelixCast"), &m_OwnDesc->HelixDesc);
	}

	if (m_OwnDesc->isBezierCurve)
	{
		m_OwnDesc->BezierDesc.ParentMatrix = m_OwnDesc->ParentMatrix;
		m_OwnDesc->BezierDesc.fMaxLifeTime = m_OwnDesc->fMaxLifeTime;
		m_OwnDesc->BezierDesc.fThreadRatio = m_OwnDesc->fThreadRatio;
		m_OwnDesc->BezierDesc.fSlowStrength = m_OwnDesc->fSlowStrength;

		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BezierCurve"), &m_OwnDesc->BezierDesc);
	}

	if (m_OwnDesc->isAspiration)
	{
		m_OwnDesc->AspDesc.ParentMatrix = m_OwnDesc->ParentMatrix;
		m_OwnDesc->AspDesc.fMaxLifeTime = m_OwnDesc->fMaxLifeTime;
		m_OwnDesc->AspDesc.fThreadRatio = m_OwnDesc->fThreadRatio;
		m_OwnDesc->AspDesc.fSlowStrength = m_OwnDesc->fSlowStrength;

		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_NewAspiration"), &m_OwnDesc->AspDesc);
	}

	m_pGameInstance->Erase(this);
	return S_OK;
}

CMagicCast* CMagicCast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagicCast* pInstance = new CMagicCast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMagicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMagicCast::Clone(void* pArg)
{
	CMagicCast* pInstance = new CMagicCast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMagicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMagicCast::Free()
{
	__super::Free();
}

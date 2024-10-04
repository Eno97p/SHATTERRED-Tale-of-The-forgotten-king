#include "stdafx.h"
#include "..\Public\PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	: CBlendObject{ rhs }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)pArg;

	m_pParentMatrix = pPartObjDesc->pParentMatrix;
	m_pState = pPartObjDesc->pState;
	m_pCurWeapon = pPartObjDesc->pCurWeapon;

	m_eLevel = pPartObjDesc->eLevel;

	m_pCanCombo = pPartObjDesc->pCanCombo;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(_float fTimeDelta)
{
}

void CPartObject::Tick(_float fTimeDelta)
{
}

void CPartObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

_float CPartObject::Get_LengthFromCamera()
{
	_vector vPos = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
	_vector vCamPos = m_pGameInstance->Get_CamPosition();
	return XMVectorGetX(XMVector3Length(vPos - vCamPos));
}


void CPartObject::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pDisolveTextureCom);
}

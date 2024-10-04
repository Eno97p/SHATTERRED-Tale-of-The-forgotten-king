#include "AndrasHead.h"
#include "GameInstance.h"
#include "EffectManager.h"
CAndrasHead::CAndrasHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CAndrasHead::CAndrasHead(const CAndrasHead& rhs)
	:CWeapon(rhs)
{
}

HRESULT CAndrasHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrasHead::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;
	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	EyeTrail = EFFECTMGR->Member_Trail(1, &m_WorldMatrix);


	return S_OK;
}

void CAndrasHead::Priority_Tick(_float fTimeDelta)
{
}

void CAndrasHead::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix		ParentMat = XMLoadFloat4x4(m_pParentMatrix);
	SocketMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	SocketMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	SocketMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CAndrasHead::Late_Tick(_float fTimeDelta)
{
	EyeTrail->Priority_Tick(fTimeDelta);
	EyeTrail->Tick(fTimeDelta);
	EyeTrail->Late_Tick(fTimeDelta);
}

HRESULT CAndrasHead::Add_Components()
{
	return S_OK;
}

CAndrasHead* CAndrasHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrasHead* pInstance = new CAndrasHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndrasHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAndrasHead::Clone(void* pArg)
{
	CAndrasHead* pInstance = new CAndrasHead(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndrasHead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAndrasHead::Free()
{
	__super::Free();
	Safe_Release(EyeTrail);
}



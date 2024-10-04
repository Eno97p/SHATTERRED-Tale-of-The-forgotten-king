#include "PlayerHead.h"
#include "GameInstance.h"

CPlayerHead* CPlayerHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerHead* pInstance = new CPlayerHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerHead::Clone(void* pArg)
{
	CPlayerHead* pInstance = new CPlayerHead(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerHead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHead::Free()
{
	__super::Free();
}

CPlayerHead::CPlayerHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CWeapon(pDevice, pContext)
{
}

CPlayerHead::CPlayerHead(const CPlayerHead& rhs)
	:CWeapon(rhs)
{
}

HRESULT CPlayerHead::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerHead::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;
	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    return S_OK;
}

void CPlayerHead::Priority_Tick(_float fTimeDelta)
{
}

void CPlayerHead::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CPlayerHead::Late_Tick(_float fTimeDelta)
{
}

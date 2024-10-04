#include "stdafx.h"
#include "..\Public\FallPlatform.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"

#include "Monster.h"


CFallPlatform::CFallPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CFallPlatform::CFallPlatform(const CFallPlatform& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CFallPlatform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFallPlatform::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC gameObjDesc = *(GAMEOBJECT_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameObjDesc.mWorldMatrix._41, gameObjDesc.mWorldMatrix._42, gameObjDesc.mWorldMatrix._43, 1.f)); //초기 위치 설정))
	m_pTransformCom->Set_Scale(1000.f, 10.f, 1000.f);


	if (FAILED(Add_Components()))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CFallPlatform::Priority_Tick(_float fTimeDelta)
{
}

void CFallPlatform::Tick(_float fTimeDelta)
{
}

void CFallPlatform::Late_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
	if (m_eColltype == CCollider::COLL_START)
	{
		m_pPlayer->PlayerHit(999999);
	}
}

HRESULT CFallPlatform::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CFallPlatform* CFallPlatform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFallPlatform* pInstance = new CFallPlatform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFallPlatform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFallPlatform::Clone(void* pArg)
{
	CFallPlatform* pInstance = new CFallPlatform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFallPlatform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFallPlatform::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}

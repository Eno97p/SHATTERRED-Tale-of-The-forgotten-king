#include "stdafx.h"
#include "..\Public\Weapon_Homonculus.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"

CWeapon_Homonculus::CWeapon_Homonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Homonculus::CWeapon_Homonculus(const CWeapon_Homonculus& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWeapon_Homonculus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Homonculus::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CWeapon_Homonculus::Priority_Tick(_float fTimeDelta)
{
}

void CWeapon_Homonculus::Tick(_float fTimeDelta)
{

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	// 몬스터 무기와 플레이어 충돌처리
	if (Get_Active())
	{
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{
			if (m_pPlayer->Get_Parry())
			{
				m_pPlayer->Set_ParriedMonsterFloat4x4(m_pParentMatrix);
				m_pPlayer->Parry_Succeed();
				m_bIsParried = true;
			}
			else
			{
				m_pPlayer->PlayerHit(10);
			}

		}
	}
	//_float3 vCollision = m_pColliderCom->Get_Center();
	//PxRaycastBuffer hit;
	//PxQueryFilterData filterData;
	//filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
	//bool isHit = m_pGameInstance->GetScene()->raycast(PxVec3(vCollision.x, vCollision.y, vCollision.z), PxVec3(0.f,-1.f,0.f), 2.f, hit, PxHitFlag::eDEFAULT, filterData);
	//if (isHit)
	//{
	//	// 충돌 지점 가져오기
	//	PxVec3 hitPoint = hit.block.position;
	//}

	Generate_Trail(4);
}

void CWeapon_Homonculus::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

void CWeapon_Homonculus::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Homonculus_Attack.ogg"), SOUND_MONSTER);
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

HRESULT CWeapon_Homonculus::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CWeapon_Homonculus* CWeapon_Homonculus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Homonculus* pInstance = new CWeapon_Homonculus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWeapon_Homonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Homonculus::Clone(void* pArg)
{
	CWeapon_Homonculus* pInstance = new CWeapon_Homonculus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWeapon_Homonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Homonculus::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}

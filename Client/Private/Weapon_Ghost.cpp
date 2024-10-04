#include "stdafx.h"
#include "..\Public\Weapon_Ghost.h"

#include "GameInstance.h"
#include "Player.h"

CWeapon_Ghost::CWeapon_Ghost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Ghost::CWeapon_Ghost(const CWeapon_Ghost& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWeapon_Ghost::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Ghost::Initialize(void* pArg)
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

void CWeapon_Ghost::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 0.5f;
		if (m_fDisolveValue < 0.f)
		{
			m_eDisolveType = TYPE_INCREASE;
		}
		break;
	default:
		break;
	}
}

void CWeapon_Ghost::Tick(_float fTimeDelta)
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

	//Generate_Trail(8);
}

void CWeapon_Ghost::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

void CWeapon_Ghost::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Ghost_Attack.ogg"), SOUND_MONSTER);
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

HRESULT CWeapon_Ghost::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(1.3f, 0.2f, 0.2f);
	ColliderDesc.vCenter = _float3(0.5f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

CWeapon_Ghost* CWeapon_Ghost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Ghost* pInstance = new CWeapon_Ghost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWeapon_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Ghost::Clone(void* pArg)
{
	CWeapon_Ghost* pInstance = new CWeapon_Ghost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWeapon_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Ghost::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}

#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "Inventory.h"

#include "Particle_Rect.h"
#include "UIGroup_MonsterHP.h"
#include "UIGroup_BossHP.h"
#include "TargetLock.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster & rhs)
	: CLandObject{ rhs }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(360.f);

	m_eLevel = pDesc->eLevel;
	m_bPlayerIsFront = pDesc->bPlayerIsFront;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayer);
	Safe_AddRef(m_pPlayerTransform);

	m_vInitialPos = { pDesc->mWorldMatrix._41 , pDesc->mWorldMatrix._42, pDesc->mWorldMatrix._43, 1.f};
	//Create_UI(); >> 자식 객체에서 직접 호출해주기 (Boss와 구분 위해)

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
}

void CMonster::Tick(_float fTimeDelta)
{
}

void CMonster::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

_bool CMonster::CanBackAttack()
{
	if (!m_bPlayerIsFront && m_fLengthFromPlayer < 3.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

_vector CMonster::Get_MonsterPos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CMonster::Add_BehaviorTree()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Add_Nodes()
{
	return S_OK;
}

void CMonster::Create_UI()
{
	CUIGroup::UIGROUP_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	m_pUI_HP = dynamic_cast<CUIGroup_MonsterHP*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_MonsterHP"), &pDesc));
	if (nullptr == m_pUI_HP)
		return;
}

void CMonster::Create_BossUI(CUIGroup_BossHP::BOSSUI_NAME eBossName)
{
	// Boss HP UI 생성 함수
	CUIGroup_BossHP::UIGROUP_BOSSHP_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eBossUIName = eBossName;
	m_pUI_HP = dynamic_cast<CUIGroup_BossHP*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_BossHP"), &pDesc));
	if (nullptr == m_pUI_HP)
		return;
}

HRESULT CMonster::Create_TargetLock(CModel* pBodyModel, string strBoneName, _vector vOffsetPos, _float fScale)
{
	CTargetLock::TARGETLOCK_DESC pDesc{};

	pDesc.fScale = fScale;
	pDesc.vOffsetPos = vOffsetPos;
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
 	pDesc.pCombinedTransformationMatrix = pBodyModel->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());

	m_pTargetLock = dynamic_cast<CTargetLock*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TargetLock"), &pDesc));
	if (nullptr == m_pTargetLock)
		return E_FAIL;

	return S_OK;
}

void CMonster::Update_UI(_float fHeight)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += fHeight;
	dynamic_cast<CUIGroup_MonsterHP*>(m_pUI_HP)->Update_Pos(vPos);
	dynamic_cast<CUIGroup_MonsterHP*>(m_pUI_HP)->Set_Ratio(m_fCurHp / m_fMaxHp);
}

void CMonster::Reward_Soul(_bool isBoss)
{
	_uint iRand;

	if (isBoss)
	{
		iRand = rand() % 300 + 500;
	}
	else
	{
		iRand = rand() % 300 + 200;
	}

	CInventory::GetInstance()->Calcul_Soul(iRand);
}

void CMonster::Free()
{
	__super::Free();
	Safe_Release(m_pUI_HP);
	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBehaviorCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTargetLock);
}

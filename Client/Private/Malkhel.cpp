#include "stdafx.h"
#include "Malkhel.h"
#include "..\Public\Malkhel.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Body_Malkhel.h"
#include "Weapon_Malkhel.h"
#include "RushSword.h"
#include "EffectManager.h"
#include "UI_Manager.h"

#include "UIGroup_BossHP.h"
#include "TargetLock.h"
#include "ThirdPersonCamera.h"
#include "EventTrigger.h"
#include "UIGroup_Portal.h"

CMalkhel::CMalkhel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMalkhel::CMalkhel(const CMalkhel& rhs)
	: CMonster{ rhs }
{
}

HRESULT CMalkhel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMalkhel::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	pDesc->fSpeedPerSec = MALKHELSPEED;
	pDesc->fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPhysXCom->Set_Speed(MALKHELSPEED);

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_fMaxHp = 500.f;
	m_fCurHp = m_fMaxHp;
	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	Create_BossUI(CUIGroup_BossHP::BOSSUI_MALKHEL);

	// Target Lock
	vector<CGameObject*>::iterator body = m_PartObjects.begin();
	if (FAILED(Create_TargetLock(dynamic_cast<CModel*>((*body)->Get_Component(TEXT("Com_Model"))), "Malkhel-Spine", XMVectorSet(-0.13f, -0.4f, 0.f, 1.f), 10.f)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	m_bPlayerIsFront = true;
	
	m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	return S_OK;
}

void CMalkhel::Priority_Tick(_float fTimeDelta)
{

	_vector vPp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//마켈 앰비언트 조명 포지션 업데이트
	m_pGameInstance->Update_LightPos(2, (vPp));

	if (m_fDeadDelay < 2.f)
	{
		m_fDeadDelay -= fTimeDelta;
		if (m_fDeadDelay < 0.f)
		{
			_vector vStartPosition, playerLook;
			vStartPosition =  m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			playerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 10.f;
			_float4 SpawnPos;
			XMStoreFloat4(&SpawnPos, vStartPosition - playerLook);
			
			SpawnPos.y += 5.f;

			EFFECTMGR->Generate_BlackHole(0, SpawnPos, LEVEL_GRASSLAND);

			// Portal UI
			CUIGroup_Portal::UIGROUP_PORTAL_DESC pUIDesc{};
			pUIDesc.eLevel = LEVEL_STATIC;
			pUIDesc.ePortalLevel = LEVEL_ANDRASARENA;
			pUIDesc.isPic = false;
			pUIDesc.vPos = XMVectorSet(SpawnPos.x, SpawnPos.y, SpawnPos.z, 1.f); // -1489.268f, 446.0f, -180.f
			CUI_Manager::GetInstance()->Create_PortalUI(&pUIDesc);

			//씬 전환 트리거 생성
			CMap_Element::MAP_ELEMENT_DESC pDesc{};
			_matrix vMat = { 1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			SpawnPos.x, SpawnPos.y, SpawnPos.z, 1.f };
			XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
			pDesc.TriggerType = CEventTrigger::TRIG_SCENE_CHANGE_FOR_ANDRASARENA;
			m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc);

			m_pGameInstance->LightOff(2);
			m_pGameInstance->Erase(this);
		}
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
	m_isAnimFinished = dynamic_cast<CBody_Malkhel*>(m_PartObjects.front())->Get_AnimFinished();
}

void CMalkhel::Tick(_float fTimeDelta)
{

	if (m_bTrigger)
	{
		m_pBehaviorCom->Update(fTimeDelta);
	}

	if (!m_bDead)
	{
		for (auto& pPartObject : m_PartObjects)
			pPartObject->Tick(fTimeDelta);
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pColliderCom->Reset();
		m_eColltype = CCollider::COLL_NOCOLL;
	}
	else
	{
		m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	}

	m_pPhysXCom->Tick(fTimeDelta);

	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Set_Ratio((m_fCurHp / m_fMaxHp));
	m_pUI_HP->Tick(fTimeDelta);

	if (m_bIsLocked)
		m_pTargetLock->Tick(fTimeDelta);
}

void CMalkhel::Late_Tick(_float fTimeDelta)
{
	m_pPhysXCom->Late_Tick(fTimeDelta);
	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	//if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		for (auto& pPartObject : m_PartObjects)
			pPartObject->Late_Tick(fTimeDelta);
	}

	m_pUI_HP->Late_Tick(fTimeDelta);

	if (m_bIsLocked)
	{
		m_pTargetLock->Late_Tick(fTimeDelta);
	}


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CMalkhel::Render()
{
	return S_OK;
}

void CMalkhel::Chase_Player(_float fTimeDelta)
{
	_float3 fScale = m_pTransformCom->Get_Scaled();

	_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vDir.m128_f32[1] = 0.f;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	_vector vUp = XMVector3Cross(vDir, vRight);

	vDir = XMVector3Normalize(vDir);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir);
	m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);
	m_pPhysXCom->Go_Straight(fTimeDelta);
}

HRESULT CMalkhel::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 10.f;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(m_vInitialPos.x, PhysXDesc.height * 0.5f + PhysXDesc.radius + m_vInitialPos.y, m_vInitialPos.z);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	PhysXDesc.pName = "Malkhel";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CMalkhel::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Malkhel"), &BodyDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

	/* 무기객체를 복제해온다. */
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;

	CModel* pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Malkhel-R-Hand");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Malkhel"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	dynamic_cast<CBody_Malkhel*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

	return S_OK;
}

_bool CMalkhel::Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider)
{
	//CCollider* pPartObjCollider = dynamic_cast<CCollider*>(m_PartObjects[ePartObjID]->Get_Component(strComponetTag));
	//
	//return pTargetCollider->Intersect(pPartObjCollider);

	return false;
}

HRESULT CMalkhel::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CMalkhel::Idle, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CMalkhel::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CMalkhel::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Teleport"), bind(&CMalkhel::Teleport, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Move"), bind(&CMalkhel::Move, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Chase"), bind(&CMalkhel::Chase, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack1"), bind(&CMalkhel::Attack1, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack2"), bind(&CMalkhel::Attack2, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack3"), bind(&CMalkhel::Attack3, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack4"), bind(&CMalkhel::Attack4, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack5"), bind(&CMalkhel::Attack5, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack6"), bind(&CMalkhel::Attack6, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack7"), bind(&CMalkhel::Attack7, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("PatternCool"), 1.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("PatternCool"), TEXT("Select_Pattern"), bind(&CMalkhel::Select_Pattern, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CMalkhel::Dead(_float fTimeDelta)
{
	if (m_iState == STATE_DEAD)
	{
		if (m_isAnimFinished)
		{
			if (!m_bDead)
			{
				m_bDead = true;
				for (_uint i = 0; i < m_PartObjects.size(); i++)
				{
					dynamic_cast<CPartObject*>(m_PartObjects[i])->Set_DisolveType(CPartObject::TYPE_DECREASE);
				}
				m_fDeadDelay -= 0.001f;

				Reward_Soul(true);

				// UI BossText 생성
				CUI_Manager::GetInstance()->Create_BossText(false);
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CMalkhel::Hit(_float fTimeDelta)
{
	switch (m_eColltype)
	{
	case CCollider::COLL_START:
	{
		m_pGameInstance->Disable_Echo();

		
		CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
		m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Hit.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
		
		if (m_pPlayer->Get_State() != CPlayer::STATE_SPECIALATTACK)
		{
			pThirdPersonCamera->Shake_Camera(0.23f, 0.01f, 0.03f, 72.f);
			pThirdPersonCamera->Zoom(50.f, 0.16f, 0.336);
		}

		_matrix vMat = m_pTransformCom->Get_WorldMatrix();
		_float3 vOffset = { 0.f,1.f,0.f };
		_vector vStartPos = XMVector3TransformCoord(XMLoadFloat3(&vOffset), vMat);
		_float4 vResult;
		XMStoreFloat4(&vResult, vStartPos);
		_int Random = RandomSign();
		EFFECTMGR->Generate_Particle(0, vResult, nullptr, XMVector3Normalize(vMat.r[2]), Random * 90.f);
		EFFECTMGR->Generate_Particle(1, vResult, nullptr);
		EFFECTMGR->Generate_Particle(2, vResult, nullptr);
		Add_Hp(-dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon())->Get_Damage());
		m_pUI_HP->Set_Rend(true); // >> 임의로 피격 시 Render 하긴 하는데 나중에 보스 대면 시 Render하는 것으로 변경할 것
		return RUNNING;
		break;
	}
	case CCollider::COLL_CONTINUE:
		break;
	case CCollider::COLL_FINISH:
		break;
	case CCollider::COLL_NOCOLL:
		break;
	}

	return FAILURE;
}

NodeStates CMalkhel::Teleport(_float fTimeDelta)
{
	if (m_iState == STATE_TELEPORT)
	{
		m_pPhysXCom->Set_Gravity(false);
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		_float3 fScale = m_pTransformCom->Get_Scaled();
		if (!m_bTeleport)
		{
			_float4 vParticlePos;
			XMStoreFloat4(&vParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Magic_Cast(3, m_pTransformCom->Get_WorldFloat4x4());
			EFFECTMGR->Generate_Distortion(8, vParticlePos);
			m_pTransformCom->Set_Scale(fScale.x - fTimeDelta * 5.f, fScale.y - fTimeDelta * 5.f, fScale.z - fTimeDelta * 5.f);
			m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, fTimeDelta * 10.f, 0.f, 0.f));
			if (m_pTransformCom->Get_Scaled().x < 0.1f)
			{
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Teleport.ogg"), SOUND_MONSTER);
				m_bTeleport = true;
				_uint i = RandomInt(0, 2);
				switch (i)
				{
				case 0:
					m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
						- XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 10.f);
					break;
				case 1:
					m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
						+ XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 10.f);
					break;
				case 2:
					m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
						- XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 10.f);
					break;
				}
			}
		}
		else
		{
			m_pTransformCom->Set_Scale(fScale.x + fTimeDelta * 5.f, fScale.y + fTimeDelta * 5.f, fScale.z + fTimeDelta * 5.f);
			m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMVectorSet(0.f, fTimeDelta * 10.f, 0.f, 0.f));
			if (m_pTransformCom->Get_Scaled().x > 1.5f)
			{
				m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
				m_bTeleport = false;
				m_iState = STATE_IDLE;
				m_fDashBackDelay = 1.f;
				m_pPhysXCom->Set_Gravity();
				return SUCCESS;
			}
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Move(_float fTimeDelta)
{
	if (m_iState == STATE_DASHBACK || m_iState == STATE_DASHRIGHT || m_iState == STATE_DASHLEFT)
	{
		if (m_fLengthFromPlayer > 8.f || m_fDashBackDelay < 0.f)
		{
			m_iState = STATE_IDLE;
			m_fDashBackDelay = 1.f;
			return SUCCESS;
		}
		else
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			if (m_iState == STATE_DASHBACK)
			{
				m_fDashBackDelay -= fTimeDelta;
				m_pPhysXCom->Go_BackWard(fTimeDelta);
			}
			else if (m_iState == STATE_DASHRIGHT)
			{
				m_pPhysXCom->Go_Right(fTimeDelta);
			}
			else if (m_iState == STATE_DASHLEFT)
			{
				m_pPhysXCom->Go_Left(fTimeDelta);
			}
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Chase(_float fTimeDelta)
{
	if (m_iState == STATE_DASHFRONT)
	{
		if (m_fLengthFromPlayer < 3.f)
		{
			m_iState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			Chase_Player(fTimeDelta * 0.5f);
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack1(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK1)
	{
		if (m_fSpawnCoolTime > 0.f)
		{
			m_fSpawnCoolTime -= fTimeDelta;
		}
		
		if (m_fSpawnCoolTime < 0.f)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Particle(35, fPos);
			m_fSpawnCoolTime = 0.f;
			m_pPlayer->PlayerHit(30.f);
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Attack1.ogg"), SOUND_MONSTER);
		}
		
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack2(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK2)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack3(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK3)
	{
		if (m_iTrippleAttackCount != 0)
		{
			m_fTrippleAttack -= fTimeDelta;
		}
		else
		{
			m_pPhysXCom->Set_Gravity(true);
			m_fTrippleAttack = TRIPPLEATTACK + 1.f;
		}

		if (m_fTrippleAttack < 0.f && m_iTrippleAttackCount > 0)
		{
			m_fTrippleAttack = TRIPPLEATTACK;
			m_iTrippleAttackCount--;
		}
		if (m_fTrippleAttack < TRIPPLEATTACK && m_fTrippleAttack > TRIPPLEATTACK * 0.5f)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pPhysXCom->Set_Gravity(false);
			m_pPhysXCom->Set_JumpSpeed(UPSPEED);
			m_pPhysXCom->Go_Up(fTimeDelta);
		}
		else if (m_fTrippleAttack < TRIPPLEATTACK * 0.5f && m_fTrippleAttack > 0.f)
		{
			m_pPhysXCom->Set_JumpSpeed(-DOWNSPEED);
			m_pPhysXCom->Go_Up(fTimeDelta);
		}
		if (m_isAnimFinished)
		{
			m_iTrippleAttackCount = 3;
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack4(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK4)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack5(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK5)
	{
		m_fSpearAttack -= fTimeDelta;
		if (m_fSpearAttack > 0.7f)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		}
		else if(m_fSpearAttack > 0.f)
		{
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}

		if (m_isAnimFinished)
		{
			m_fSpearAttack = SPEARATTACK;
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack6(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK6)
	{
		m_fSpawnCoolTime -= fTimeDelta;
		if (m_fSpawnCoolTime < 0.f)
		{
			m_fSpawnCoolTime = METEORCOOLTIME;
			_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vRandomDir = XMVectorSet(RandomFloat(-10.f, 10.f), 0.f, RandomFloat(-10.f, 10.f), 0.f);
			_float4 vPos;
			XMStoreFloat4(&vPos, vPlayerPos - vRandomDir);
			EFFECTMGR->Generate_Meteor(vPos);
		}

		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Attack7(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK7)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CMalkhel::Select_Pattern(_float fTimeDelta)
{
	if (!m_bDashBack)
	{
		m_fTurnDelay = 0.5f;

		if (m_fLengthFromPlayer > 20.f)
		{
			m_iState = STATE_DASHFRONT;
		}
		else if (m_fLengthFromPlayer > 5.f)
		{

			switch (m_iPastState)
			{
			case STATE_TELEPORT:
				m_fSpawnCoolTime = EXPLODECOOLTIME;
				m_iState = STATE_ATTACK1;
				break;
			case STATE_ATTACK1:
				m_iState = STATE_ATTACK4;
				break;
			case STATE_ATTACK4:
				m_iState = STATE_ATTACK5;
				break;
			case STATE_ATTACK5:
			{
				m_fSpawnCoolTime = 0.f;
				m_iState = STATE_ATTACK6;
				CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
				pThirdPersonCamera->Zoom(90.f, 1.f, 2.f);
				break;
			}
			case STATE_ATTACK6:
				m_iState = STATE_TELEPORT;
				break;
			default:
				m_iState = STATE_ATTACK5;
				break;
			}
		}
		else
		{
			switch (m_iPastState)
			{
			case STATE_TELEPORT:
				m_fSpawnCoolTime = EXPLODECOOLTIME;
				m_iState = STATE_ATTACK1;
				break;
			case STATE_ATTACK1:
				m_iState = STATE_DASHBACK;
				break;
			case STATE_DASHBACK:
				m_iState = STATE_ATTACK2;
				break;
			case STATE_ATTACK2:
				m_iState = STATE_DASHLEFT;
				break;
			case STATE_DASHLEFT:
				m_iState = STATE_ATTACK3;
				break;
			case STATE_ATTACK3:
				m_iState = STATE_ATTACK4;
				break;
			case STATE_ATTACK4:
				m_iState = STATE_ATTACK5;
				break;
			case STATE_ATTACK5:
			{

				m_fSpawnCoolTime = 0.f;
				m_iState = STATE_DASHRIGHT;
				CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
				pThirdPersonCamera->Zoom(60.f, 0.13f, 0.602f);
				break;
			}
			case STATE_DASHRIGHT:
				m_iState = STATE_ATTACK6;
				break;
			case STATE_ATTACK6:
				m_iState = STATE_TELEPORT;
				break;
			default:
				m_iState = STATE_ATTACK2;
				break;
			}
		}
		m_iPastState = m_iState;
		return SUCCESS;
	}
}

NodeStates CMalkhel::Idle(_float fTimeDelta)
{
	m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_iState = STATE_IDLE;
	return SUCCESS;
}

void CMalkhel::Add_Hp(_int iValue)
{
	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Rend_Damage(iValue);

	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0)
	{
		m_iState = STATE_DEAD;
	}
}

CMalkhel* CMalkhel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMalkhel* pInstance = new CMalkhel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMalkhel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMalkhel::Clone(void* pArg)
{
	CMalkhel* pInstance = new CMalkhel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMalkhel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMalkhel::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
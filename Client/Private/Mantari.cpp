#include "stdafx.h"
#include "Mantari.h"
#include "..\Public\Mantari.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Body_Mantari.h"
#include "Weapon_Mantari.h"
#include "EffectManager.h"
#include "UI_Manager.h"

#include "UIGroup_BossHP.h"
#include "TargetLock.h"
#include "ThirdPersonCamera.h"

CMantari::CMantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMantari::CMantari(const CMantari& rhs)
	: CMonster{ rhs }
{
}

HRESULT CMantari::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMantari::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	pDesc->fSpeedPerSec = 3.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	m_fMaxHp = 200.f;
	m_fCurHp = m_fMaxHp;
	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	Create_BossUI(CUIGroup_BossHP::BOSSUI_MANTARI);

	// Target Lock
	vector<CGameObject*>::iterator body = m_PartObjects.begin();
	if (FAILED(Create_TargetLock(dynamic_cast<CModel*>((*body)->Get_Component(TEXT("Com_Model"))), "Mob_Elite-Head_end_end", XMVectorSet(-0.13f, -0.4f, 0.f, 1.f), 10.f)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	return S_OK;
}

void CMantari::Priority_Tick(_float fTimeDelta)
{
	_vector vPp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//만타리 앰비언트 조명 포지션 업데이트
	m_pGameInstance->Update_LightPos(2, (vPp));

	if (m_fDeadDelay < 2.f)
	{
		m_fDeadDelay -= fTimeDelta;
		if (m_fDeadDelay < 0.f)
		{
			m_pGameInstance->LightOff(2);
			m_pGameInstance->Erase(this);
		}
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
	m_isAnimFinished = dynamic_cast<CBody_Mantari*>(m_PartObjects.front())->Get_AnimFinished();
}

void CMantari::Tick(_float fTimeDelta)
{
	m_fDegreeBetweenPlayerAndMonster = abs(XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)),
		XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))))));

	if (!m_bPlayerIsFront)
	{
		if (m_fDegreeBetweenPlayerAndMonster < 60.f && !m_pPlayer->Get_Cloaking() && m_fLengthFromPlayer < DETECTRANGE)
		{
			m_bPlayerIsFront = true;
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Mantari_Aggro.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
		}
	}
	else
	{
		if (m_pPlayer->Get_Cloaking())
		{
			m_bPlayerIsFront = false;
		}
	}

	m_pBehaviorCom->Update(fTimeDelta);

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

	if(m_bIsLocked)
		m_pTargetLock->Tick(fTimeDelta);
}	

void CMantari::Late_Tick(_float fTimeDelta)
{
	m_pPhysXCom->Late_Tick(fTimeDelta);
	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
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

HRESULT CMantari::Render()
{
	return S_OK;
}

void CMantari::Chase_Player(_float fTimeDelta)
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
	m_pPhysXCom->Go_Straight(fTimeDelta * m_fLengthFromPlayer);
}

HRESULT CMantari::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.7f, 1.2f, 0.7f);
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
	PhysXDesc.radius = 1.f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(m_vInitialPos.x, PhysXDesc.height * 0.5f + PhysXDesc.radius + m_vInitialPos.y, m_vInitialPos.z);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	PhysXDesc.pName= "Mantari";
	PhysXDesc.pGameObject = this;
	PhysXDesc.pFilterCallBack = [this](const PxController& a, const PxController& b) {return this->OnFilterCallback(a, b); };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CMantari::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.pCanCombo = &m_bCanCombo;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Mantari"), &BodyDesc);
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

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Root_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Mantari"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	dynamic_cast<CBody_Mantari*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

	return S_OK;
}

_bool CMantari::Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider)
{
	//CCollider* pPartObjCollider = dynamic_cast<CCollider*>(m_PartObjects[ePartObjID]->Get_Component(strComponetTag));
	//
	//return pTargetCollider->Intersect(pPartObjCollider);

	return false;
}

HRESULT CMantari::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CMantari::Idle, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Revive"), bind(&CMantari::Revive, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CMantari::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CMantari::Hit, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Parried"), bind(&CMantari::Parried, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("JumpAttack"), bind(&CMantari::JumpAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack"), bind(&CMantari::Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("CircleAttack"), bind(&CMantari::CircleAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("DetectCool"), 1.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DetectCool"), TEXT("Detect"), bind(&CMantari::Detect, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("MoveTo"), bind(&CMantari::Move, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CMantari::Revive(_float fTimeDelta)
{
	if (m_bReviving)
	{
		m_iState = STATE_REVIVE;

		if (m_isAnimFinished)
		{
			m_bReviving = false;
			m_iState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CMantari::Dead(_float fTimeDelta)
{
	if (m_iState == STATE_DEAD)
	{
		if (m_fChasingDelay < 2.f)
		{
			m_pPhysXCom->Set_Gravity(false);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.m128_f32[1] += fTimeDelta * 0.5f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pPhysXCom->Set_Position(vPos);
		}
		else
		{
			m_pPhysXCom->Set_Gravity(false);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.m128_f32[1] += fTimeDelta * 0.3f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pPhysXCom->Set_Position(vPos);
		}

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

NodeStates CMantari::Hit(_float fTimeDelta)
{
	
	switch (m_eColltype)
	{
	case CCollider::COLL_START:
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Mantari_Hit.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
		m_pGameInstance->Play_Effect_Sound(TEXT("Mantari_HitVoice.ogg"), SOUND_MONSTER, 0.3f);
		m_fChasingDelay = 0.5f;
		CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_MainCamera());
		if (m_pPlayer->Get_State() != CPlayer::STATE_SPECIALATTACK)
		{
			pThirdPersonCamera->Shake_Camera(0.23f, 0.01f, 0.03f, 72.f);
			pThirdPersonCamera->Zoom(50.f, 0.16f, 0.336);
		}
		
		
		if (m_pPlayer->Get_m_bParry())
		{
			if (m_bParryFirstHit)
			{
				pThirdPersonCamera->StartTilt(18.344f, 0.24f, 0.44f);
				m_bParryFirstHit = !m_bParryFirstHit;
			}
			else
			{
				pThirdPersonCamera->StartTilt(-25.344f, 0.24f, 0.44f);
				m_bParryFirstHit = !m_bParryFirstHit;
			}
		}
		m_pGameInstance->Set_MotionBlur(true);

		if (!m_bPlayerIsFront)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Mantari_Aggro.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
			m_bPlayerIsFront = true;
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
		EFFECTMGR->Generate_Distortion(5, vResult);
		m_iState = STATE_HIT;
		Add_Hp(-dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon())->Get_Damage());

		// UI Damage 띄울 것
		m_pUI_HP->Set_Rend(true); // >> 임의로 피격 시 Render 하긴 하는데 나중에 보스 대면 시 Render하는 것으로 변경할 것
		return RUNNING;
		break;
	}
	case CCollider::COLL_CONTINUE:
		m_iState = STATE_HIT;
		return RUNNING;
		break;
	case CCollider::COLL_FINISH:
		//m_pGameInstance->Set_MotionBlur(false);
		m_iState = STATE_HIT;
		break;
	case CCollider::COLL_NOCOLL:
		break;
	}

	if (m_iState == STATE_HIT && m_isAnimFinished)
	{
		m_iState = STATE_IDLE;
		return SUCCESS;
	}

	return FAILURE;
}

NodeStates CMantari::Parried(_float fTimeDelta)
{
	if (dynamic_cast<CWeapon_Mantari*>(m_PartObjects[1])->Get_IsParried() && m_iState != STATE_PARRIED)
	{
		m_iState = STATE_PARRIED;
		if (m_iAttackCount == 3)
		{
			m_iAttackCount = 0;
		}
		else
		{
			m_iAttackCount++;
		}
	}

	if (m_iState == STATE_PARRIED)
	{
		if (m_isAnimFinished)
		{
			dynamic_cast<CWeapon_Mantari*>(m_PartObjects[1])->Set_IsParried(false);
			m_iState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}


NodeStates CMantari::JumpAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK1 || m_iState == STATE_ATTACK2 || m_iState == STATE_ATTACK3 || m_iState == STATE_CIRCLEATTACK)
	{
		return COOLING;
	}

	if (m_iState == STATE_JUMPATTACK)
	{
		m_fChasingDelay -= fTimeDelta;

		if (m_bChasing && m_fChasingDelay < 0.f)
		{
			Chase_Player(fTimeDelta);
		}

		if (m_fLengthFromPlayer < 5.f)
		{
			m_bChasing = false;
		}

		if (m_isAnimFinished)
		{
			if (m_fLengthFromPlayer < 5.f)
			{
				m_iState = RandomInt(0, 3);
			}
			else
			{
				m_iState = STATE_WALKFRONT;
			}
			m_fChasingDelay = 0.5f;
			m_bChasing = true;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CMantari::Attack(_float fTimeDelta)
{
	if (m_iState == STATE_CIRCLEATTACK)
	{
		return COOLING;
	}

	if (m_iState == STATE_ATTACK1 || m_iState == STATE_ATTACK2 || m_iState == STATE_ATTACK3)
	{
		switch (m_iAttackCount)
		{
		case 1:
			m_iState = STATE_ATTACK1;
			break;
		case 2:
			m_iState = STATE_ATTACK2;
			break;
		case 3:
			m_iState = STATE_ATTACK3;
			break;
		default:
			break;
		}

		if (m_isAnimFinished)
		{
			if (m_iAttackCount == 3)
			{
				m_iAttackCount = 0;
			}
			else
			{
				m_iAttackCount++;
			}
			if (m_fLengthFromPlayer < 6.f)
			{
				m_iState = RandomInt(0, 3);
			}
			else
			{
				m_iState = STATE_WALKFRONT;
			}

			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CMantari::CircleAttack(_float fTimeDelta)
{
	if (m_iState == STATE_CIRCLEATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iAttackCount++;
			if (m_fLengthFromPlayer < 6.f)
			{
				m_iState = rand() % 4;
			}
			else
			{
				m_iState = STATE_WALKFRONT;
			}
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
	return FAILURE;
}

NodeStates CMantari::Detect(_float fTimeDelta)
{
	if (m_pPlayer->Get_Cloaking() || !m_bPlayerIsFront)
	{
		return FAILURE;
	}

	if (m_fLengthFromPlayer > DETECTRANGE)
	{
		m_bPlayerIsFront = false;
		m_iState = STATE_IDLE;
		return SUCCESS;
	}
	else if (m_fLengthFromPlayer > JUMPATTACKRANGE)
	{
		m_iState = STATE_JUMPATTACK;
		return SUCCESS;
	}
	else if (m_fLengthFromPlayer > ATTACKRANGE)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_iState = STATE_WALKFRONT;
		return SUCCESS;
	}
	else
	{
		if (m_iAttackCount == 0)
		{
			m_iState = STATE_CIRCLEATTACK;
		}
		else
		{
			switch (m_iAttackCount)
			{
			case 1:
				m_iState = STATE_ATTACK1;
				break;
			case 2:
				m_iState = STATE_ATTACK2;
				break;
			case 3:
				m_iState = STATE_ATTACK3;
				break;
			default:
				break;
			}
		}
		return SUCCESS;
	}
}

NodeStates CMantari::Move(_float fTimeDelta)
{
	if (m_iState == STATE_IDLE || m_pPlayer->Get_Cloaking())
	{
		return COOLING;
	}

	if (m_iState == STATE_WALKLEFT)
	{
		m_pPhysXCom->Go_OrbitCW(fTimeDelta, m_pPlayerTransform);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			m_iState = STATE_IDLE;
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_WALKRIGHT)
	{
		m_pPhysXCom->Go_OrbitCCW(fTimeDelta, m_pPlayerTransform);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			m_iState = STATE_IDLE;
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_WALKBACK)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_BackWard(fTimeDelta);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			m_iState = STATE_IDLE;
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_WALKFRONT)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_Straight(fTimeDelta);
		if (m_fLengthFromPlayer < 5.f)
		{
			m_iState = STATE_IDLE;
			return FAILURE;
		}
	}

	return RUNNING;
}

NodeStates CMantari::Idle(_float fTimeDelta)
{
	if (!m_pPlayer->Get_Cloaking() && m_bPlayerIsFront)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	}
	m_iState = STATE_IDLE;
	return SUCCESS;
}



void CMantari::Add_Hp(_int iValue)
{
	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Rend_Damage(iValue);

	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0)
	{
		m_iState = STATE_DEAD;
	}
}

bool CMantari::OnFilterCallback(const PxController& Caller, const PxController& Ohter)
{
	void* Test = Caller.getUserData();
	void* temp1 = Ohter.getUserData();

	return true;


}

CMantari* CMantari::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMantari* pInstance = new CMantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMantari::Clone(void* pArg)
{
	CMantari* pInstance = new CMantari(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMantari::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
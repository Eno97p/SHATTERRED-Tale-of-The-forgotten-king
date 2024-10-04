#include "Boss_Juggulus.h"

#include "GameInstance.h"
#include "Body_Juggulus.h"
#include "PartObject.h"
#include "Juggulus_Hammer.h"
#include "Juggulus_HandOne.h"
#include "Juggulus_HandTwo.h"
#include "Juggulus_HandThree.h"
#include "EffectManager.h"
#include "UI_Manager.h"

#include "UIGroup_BossHP.h"
#include "TransitionCamera.h"
#include "EventTrigger.h"
#include "BossDeco.h"
#include "SideViewCamera.h"

CBoss_Juggulus::CBoss_Juggulus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CBoss_Juggulus::CBoss_Juggulus(const CBoss_Juggulus& rhs)
	: CMonster{ rhs }
{
}

HRESULT CBoss_Juggulus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Juggulus::Initialize(void* pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 3.f; // 수정 필요
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_iState = STATE_IDLE_FIRST;
	m_ePhase = PHASE_ONE;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(3.f, 3.f, 3.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vInitialPos.x, m_vInitialPos.y, m_vInitialPos.z, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_fMaxHp = 300.f;
	m_fCurHp = m_fMaxHp;

	Create_BossUI(CUIGroup_BossHP::BOSSUI_JUGGULUS);

	// 보스 석상 소환
	const _float4x4 statueMatrices[STATUECOUNT] = {
		{ 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -410.189f, 67.966f, -2.195f, 1.f },
		{ -0.91f, 0.f, -0.415f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.415f, 0.f, -0.91f, 0.f, -420.326f, 67.976f, -17.686f, 1.f },
		{ 0.845f, 0.f, -0.536f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.536f, 0.f, 0.845f, 0.f, -420.068f, 67.932f, 13.209f, 1.f }
	};

	for (const auto& matrix : statueMatrices)
	{
		CMap_Element::MAP_ELEMENT_DESC StatueDesc{};
		StatueDesc.mWorldMatrix = matrix;
		m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_BossStatue"), &StatueDesc);
	}


	list<CGameObject*> StatueList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Statue"));
	_uint i = 0;
	for (auto iter : StatueList)
	{
		m_pBossStatues[i] = dynamic_cast<CBossStatue*>(iter);
		Safe_AddRef(m_pBossStatues[i]);
		i++;
	}

	m_bPlayerIsFront = true;

	return S_OK;
}

void CBoss_Juggulus::Priority_Tick(_float fTimeDelta)
{
	_vector vPp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//마켈 앰비언트 조명 포지션 업데이트
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
		pPartObject.second->Priority_Tick(fTimeDelta);
	m_isAnimFinished = dynamic_cast<CBody_Juggulus*>((*m_PartObjects.find("Body")).second)->Get_AnimFinished();
}

void CBoss_Juggulus::Tick(_float fTimeDelta)
{
	m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	Check_AnimFinished();


	if (m_bTrigger)
	{
		m_pBehaviorCom->Update(fTimeDelta);
	}

	if (!m_bDead)
	{
		for (auto& pPartObject : m_PartObjects)
			pPartObject.second->Tick(fTimeDelta);
	}

	if (m_pGameInstance->Key_Down(DIK_P))
		m_fCurHp = 10.f;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_ePhase == PHASE_TWO)
	{
		// 플레이어 무기와 몬스터의 충돌 여부

		CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
		if (!pPlayerWeapon->Get_Active())
		{
			m_pColliderCom->Reset();
		}
		else
		{
			m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
			if (m_eColltype == CCollider::COLL_START)
			{
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Hit.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
				Add_Hp(-dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon())->Get_Damage());
			}
		}
	}

	//m_pPhysXCom->Tick(fTimeDelta);

	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Set_Ratio((m_fCurHp / m_fMaxHp));
	m_pUI_HP->Tick(fTimeDelta);

	Spawn_CircleSphere(fTimeDelta);
	Spawn_Lightning(fTimeDelta);
	Spawn_Flame(fTimeDelta);
}

void CBoss_Juggulus::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Late_Tick(fTimeDelta);
	//m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pUI_HP->Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CBoss_Juggulus::Render()
{
	return S_OK;
}

void CBoss_Juggulus::Add_Hp(_int iValue)
{
	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Rend_Damage(iValue);

	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0.f)
	{
		m_iState = STATE_DEAD;
	}
}

void CBoss_Juggulus::Add_Hands()
{
	// Juggulus Body
	CPartObject::PARTOBJ_DESC PartDesc{};
	PartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	PartDesc.fSpeedPerSec = 0.f;
	PartDesc.fRotationPerSec = 0.f;
	PartDesc.pState = &m_iState;
	PartDesc.eLevel = m_eLevel;
	PartDesc.pCurHp = &m_fCurHp;
	PartDesc.pMaxHp = &m_fMaxHp;

	// Hand One
	CGameObject* pHandOne = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandOne"), &PartDesc);
	m_PartObjects.emplace("Hand_One", pHandOne);

	// Hand Two
	CGameObject* pHandTwo = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandTwo"), &PartDesc);
	m_PartObjects.emplace("Hand_Two", pHandTwo);

	// Hand Three
	CGameObject* pHandThree = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandThree"), &PartDesc);
	m_PartObjects.emplace("Hand_Three", pHandThree);
}

void CBoss_Juggulus::Juggulus_Activate()
{
	if (!m_bTrigger)
	{
		m_pUI_HP->Set_Rend(true);
		Add_Hands();
	}
	m_bTrigger = true;
}

HRESULT CBoss_Juggulus::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(3.f, 15.f, 3.f);
	ColliderDesc.vCenter = _float3(5.f, ColliderDesc.vExtents.y - 15.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_vInitialPos = {-429.212f, 45.232f, -3.151f, 1.f};
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Juggulus::Add_PartObjects()
{
	// Juggulus Body
	CPartObject::PARTOBJ_DESC PartDesc{};
	PartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	PartDesc.fSpeedPerSec = 0.f;
	PartDesc.fRotationPerSec = 0.f;
	PartDesc.pState = &m_iState;
	PartDesc.eLevel = m_eLevel;
	PartDesc.pCurHp = &m_fCurHp;
	PartDesc.pMaxHp = &m_fMaxHp;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Juggulus"), &PartDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace("Body", pBody);

	return S_OK;
}

HRESULT CBoss_Juggulus::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);

	//m_pBehaviorCom->Add_CoolDown(TEXT("Top_Selector"), TEXT("AttackCool"), 3.f); 	// Attack 간 coolTime
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("OneP_Attack"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("TwoP_Attack"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CBoss_Juggulus::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Groggy"), bind(&CBoss_Juggulus::Groggy, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CBoss_Juggulus::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("NextPhase"), bind(&CBoss_Juggulus::NextPhase, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("CreateHammer"), bind(&CBoss_Juggulus::CreateHammer, this, std::placeholders::_1));

	// 2Phase Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("Select_PatternCool"), 5.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("Select_PatternCool"), TEXT("Select_Pattern"), bind(&CBoss_Juggulus::Select_Pattern, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("HammerAttack"), bind(&CBoss_Juggulus::HammerAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("FlameAttack"), bind(&CBoss_Juggulus::FlameAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("SphereAttack"), bind(&CBoss_Juggulus::SphereAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("ThunderAttack"), bind(&CBoss_Juggulus::ThunderAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("TornadoAttack"), bind(&CBoss_Juggulus::TornadoAttack, this, std::placeholders::_1));

	return S_OK;
}

HRESULT CBoss_Juggulus::Create_Hammer()
{
	CWeapon::WEAPON_DESC WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.eLevel = m_eLevel;

	map<string, CGameObject*>::iterator body = m_PartObjects.find("Body");
	CModel* pModelCom = dynamic_cast<CModel*>((*body).second->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Root_Hammer");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_Hammer"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_PartObjects.emplace("Hammer", pWeapon);

	dynamic_cast<CBody_Juggulus*>((*body).second)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

	return S_OK;
}

void CBoss_Juggulus::Check_AnimFinished()
{
	map<string, CGameObject*>::iterator hand = m_PartObjects.find("Hand_One");
	if (hand != m_PartObjects.end())
	{
		m_isHandAnimFinished = dynamic_cast<CJuggulus_HandOne*>((*hand).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator hand_two = m_PartObjects.find("Hand_Two");
	if (hand_two != m_PartObjects.end())
	{
		m_isHandTwoAnimFinished = dynamic_cast<CJuggulus_HandTwo*>((*hand_two).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator hand_three = m_PartObjects.find("Hand_Three");
	if (hand_three != m_PartObjects.end())
	{
		m_isHandThreeAnimFinished = dynamic_cast<CJuggulus_HandThree*>((*hand_three).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator body = m_PartObjects.find("Body");
	if (body != m_PartObjects.end())
	{
		m_isAnimFinished = dynamic_cast<CBody_Juggulus*>((*body).second)->Get_AnimFinished();
	}
}

void CBoss_Juggulus::Spawn_CircleSphere(_float fTimeDelta)
{
	if (m_fCircleSphereSpawnTime < CIRCLESPHERESPAWNTIME)
	{
		m_fCircleSphereSpawnTime -= fTimeDelta;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CGameObject::GAMEOBJECT_DESC gameObjDesc;
		gameObjDesc.fSpeedPerSec = 3.f;
		gameObjDesc.mWorldMatrix._41 = XMVectorGetX(vPos) + 20.f;
		gameObjDesc.mWorldMatrix._42 = XMVectorGetY(vPos) + 15.f;
		gameObjDesc.mWorldMatrix._43 = XMVectorGetZ(vPos);

		if (m_fCircleSphereSpawnTime < 2.f && m_iCircleSphereCount == 0)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &gameObjDesc);
			m_iCircleSphereCount++;
		}
		else if (m_fCircleSphereSpawnTime < 1.33f && m_iCircleSphereCount == 1)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &gameObjDesc);
			m_iCircleSphereCount++;
		}
		else if (m_fCircleSphereSpawnTime < 0.66f && m_iCircleSphereCount == 2)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &gameObjDesc);
			m_iCircleSphereCount = 0;
			m_fCircleSphereSpawnTime = CIRCLESPHERESPAWNTIME;
		}
	}
}

void CBoss_Juggulus::Spawn_Lightning(_float fTimeDelta)
{
	if (m_fLightningSpawnTime < LIGHTNINGSPAWNTIME)
	{
		m_fLightningSpawnTime -= fTimeDelta;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CGameObject::GAMEOBJECT_DESC gameObjDesc;
		gameObjDesc.fSpeedPerSec = 3.f;
		gameObjDesc.mWorldMatrix._41 = XMVectorGetX(vPos);
		gameObjDesc.mWorldMatrix._42 = XMVectorGetY(vPos) + 7.f;
		gameObjDesc.mWorldMatrix._43 = XMVectorGetZ(vPos);

		if (m_fLightningSpawnTime < 8.f && m_iLightningCount == 0)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_Lightning"), &gameObjDesc);
			m_iLightningCount++;
		}
		else if (m_fLightningSpawnTime < 6.f && m_iLightningCount == 1)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_Lightning"), &gameObjDesc);
			m_iLightningCount++;
		}
		else if (m_fLightningSpawnTime < 4.f && m_iLightningCount == 2)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_Lightning"), &gameObjDesc);
			m_iLightningCount++;
		}
		else if (m_fLightningSpawnTime < 2.f && m_iLightningCount == 3)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_Lightning"), &gameObjDesc);
			m_iLightningCount = 0;
			m_fLightningSpawnTime = LIGHTNINGSPAWNTIME;
		}
	}
}

void CBoss_Juggulus::Spawn_Flame(_float fTimeDelta)
{
	if (m_fFlameSpawnTime < FLAMESPAWNTIME)
	{
		m_fFlameSpawnTime -= fTimeDelta;

		if (m_fFlameSpawnTime < 5.f && m_iFlameCount == 0)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			fPos.y = 68.f;
			EFFECTMGR->Generate_FirePillar(fPos);
			m_iFlameCount++;
		}
		else if (m_fFlameSpawnTime < 4.f && m_iFlameCount == 1)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			fPos.y = 68.f;
			EFFECTMGR->Generate_FirePillar(fPos);
			m_iFlameCount++;
		}
		else if (m_fFlameSpawnTime < 3.f && m_iFlameCount == 2)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			fPos.y = 68.f;
			EFFECTMGR->Generate_FirePillar(fPos);
			m_iFlameCount++;
		}
		else if (m_fFlameSpawnTime < 2.f && m_iFlameCount == 3)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_FirePillar(fPos);
			m_iFlameCount++;
		}
		else if (m_fFlameSpawnTime < 1.f && m_iFlameCount == 4)
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			fPos.y = 68.f;
			EFFECTMGR->Generate_FirePillar(fPos);
			m_iFlameCount = 0;
			m_fFlameSpawnTime = FLAMESPAWNTIME;
		}
	}
}

NodeStates CBoss_Juggulus::Dead(_float fTimedelta)
{
	if (0.f >= m_fCurHp)
	{
		if (m_iState != STATE_DEAD)
		{
			m_pPlayer->Set_Position(XMVectorSet(-407.f, 68.f, -1.f, 1.f));
			EFFECTMGR->Generate_Particle(132, _float4(-407.f, 69.f, -1.f, 1.f));
			EFFECTMGR->Generate_Particle(133, _float4(-407.f, 68.f, -1.f, 1.f), nullptr, XMVectorSet(1.f,0.f,0.f,0.f),90.f );
			EFFECTMGR->Generate_Particle(134, _float4(-407.f, 68.f, -1.f, 1.f), nullptr, XMVectorSet(1.f,0.f,0.f,0.f),-90.f );


			//컷씬 트렌지션
			CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

			pTCDesc.fFovy = XMConvertToRadians(60.f);
			pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
			pTCDesc.fNear = 0.1f;
			pTCDesc.fFar = 3000.f;

			pTCDesc.fSpeedPerSec = 40.f;
			pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

			pTCDesc.iStartCam = CAM_THIRDPERSON;
			pTCDesc.iEndCam = CAM_SIDEVIEW;
			pTCDesc.fTransitionTime = 1.f;
			if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
			{
				MSG_BOX("FAILED to Add Transition Cam");
			}

			m_pGameInstance->Set_MainCamera(CAM_TRANSITION);
		}

		m_pTransformCom->Go_Up(-fTimedelta * 0.5f);

		if (!m_bDeadSound)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_Dead.ogg"), SOUND_MONSTER);
			m_bDeadSound = true;
		}

		m_iState = STATE_DEAD;

		if (m_isAnimFinished)
		{
			Reward_Soul(true);

			// UI BossText 생성
			CUI_Manager::GetInstance()->Create_BossText(false);

			//엘베 하강 트리거 생성
			CMap_Element::MAP_ELEMENT_DESC pDesc{};
			_matrix vMat = { 1.f, 0.f, 0.f, 0.f,
			0.f, 3.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			-310.531f, 69.022f, -1.225f, 1.f };
			XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
			pDesc.TriggerType = CEventTrigger::TRIG_DESCEND_ELEVATOR;

			(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc));

			//카메라 전환
			CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};
			pTCDesc.fFovy = XMConvertToRadians(60.f);
			pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
			pTCDesc.fNear = 0.1f;
			pTCDesc.fFar = 3000.f;

			pTCDesc.fSpeedPerSec = 40.f;
			pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

			pTCDesc.iStartCam = CAM_SIDEVIEW;
			pTCDesc.iEndCam = CAM_THIRDPERSON;
			pTCDesc.fTransitionTime = 0.7f;

			if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
			{
				MSG_BOX("FAILED");
			}

			//보스데코 떨구기
			list<CGameObject*> bossDecos = m_pGameInstance->Get_GameObjects_Ref(LEVEL_JUGGLAS, TEXT("Layer_BossDeco"));
			for (auto& bossDeco : bossDecos)
			{
				dynamic_cast<CBossDeco*>(bossDeco)->Play_DeadAnimation();
			}

			m_pGameInstance->Erase(this);

		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::NextPhase(_float fTimedelta)
{
	if (m_iState == STATE_CREATE_HAMMER)
	{
		return COOLING;
	}

	if (m_fMaxHp * 0.5f >= m_fCurHp && PHASE_ONE == m_ePhase) // || m_pGameInstance->Key_Down(DIK_P)
	{
		if (m_iState != STATE_NEXTPHASE)
		{
			CSideViewCamera* pSVC = dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW]);
			pSVC->Phase_Two_Height_Offset();
			m_pGameInstance->Get_MainCamera()->Zoom(75.f, 0.5f, 10000.f);

			// 손 삭제, 몬스터 삭제
			Safe_Release((*m_PartObjects.find("Hand_One")).second);
			Safe_Release((*m_PartObjects.find("Hand_Two")).second);
			Safe_Release((*m_PartObjects.find("Hand_Three")).second);
			m_PartObjects.erase("Hand_One");
			m_PartObjects.erase("Hand_Two");
			m_PartObjects.erase("Hand_Three");
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.m128_f32[1] += 15.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_Phase2.ogg"), SOUND_MONSTER);
		}

		m_iState = STATE_NEXTPHASE;

		if (m_isAnimFinished)
		{
			for (_uint i = 0; i < STATUECOUNT; i++)
			{
				m_pBossStatues[i]->Set_Active(true);
			}
			m_iState = STATE_CREATE_HAMMER;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::CreateHammer(_float fTimeDelta)
{
	if (m_iState == STATE_CREATE_HAMMER)
	{
		if (!m_isHammerCreate)
		{
			Create_Hammer();
			m_isHammerCreate = true;
		}

		if (m_isAnimFinished)
		{
			m_pGameInstance->Get_MainCamera()->Zoom(60.f, 0.5f, 0.5f);
			CSideViewCamera* pSVC = dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW]);
			pSVC->Phase_Two_Back_To_Origin();

			m_ePhase = PHASE_TWO;
			m_iState = STATE_IDLE_SEC;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::Groggy(_float fTimeDelta)
{
	if (m_ePhase != PHASE_TWO)
	{
		return FAILURE;
	}

	_uint count = 0;
	for (_uint i = 0; i < STATUECOUNT; i++)
	{
		if (!m_pBossStatues[i]->Get_Active())
		{
			count++;
		}
	}
	if (count == STATUECOUNT)
	{
		m_iState = STATE_GROGGY;
	}

	if (m_iState == STATE_GROGGY)
	{
		if (!m_bGroggyCamChange)
		{
			m_bGroggyCamChange = true;
			_float4 ShieldBreakPos;
			XMStoreFloat4(&ShieldBreakPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Particle(130, ShieldBreakPos);

			//컷씬 트렌지션
			CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

			pTCDesc.fFovy = XMConvertToRadians(60.f);
			pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
			pTCDesc.fNear = 0.1f;
			pTCDesc.fFar = 3000.f;

			pTCDesc.fSpeedPerSec = 40.f;
			pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

			pTCDesc.iStartCam = CAM_SIDEVIEW;
			pTCDesc.iEndCam = CAM_THIRDPERSON;
			pTCDesc.fTransitionTime = 1.f;
			if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
			{
				MSG_BOX("FAILED to Add Transition Cam");
			}

			m_pGameInstance->Set_MainCamera(CAM_TRANSITION);

		}

		m_fGroggyTime -= fTimeDelta;

		if (m_fGroggyTime < 0.f)
		{
			m_bGroggyCamChange = false;

			m_pPlayer->Set_Position(XMVectorSet(-407.f, 68.f, -1.f, 1.f));
			EFFECTMGR->Generate_Particle(132, _float4(-407.f, 69.f, -1.f, 1.f));
			EFFECTMGR->Generate_Particle(133, _float4(-407.f, 68.f, -1.f, 1.f), nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
			EFFECTMGR->Generate_Particle(134, _float4(-407.f, 68.f, -1.f, 1.f), nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), -90.f);

			//컷씬 트렌지션
			CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

			pTCDesc.fFovy = XMConvertToRadians(60.f);
			pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
			pTCDesc.fNear = 0.1f;
			pTCDesc.fFar = 3000.f;

			pTCDesc.fSpeedPerSec = 40.f;
			pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);
			
			pTCDesc.iStartCam = CAM_THIRDPERSON;
			pTCDesc.iEndCam = CAM_SIDEVIEW;
			pTCDesc.fTransitionTime = 1.f;
			if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
			{
				MSG_BOX("FAILED to Add Transition Cam");
			}

			m_pGameInstance->Set_MainCamera(CAM_TRANSITION);

			for (_uint i = 0; i < STATUECOUNT; i++)
			{
				m_pBossStatues[i]->Set_Active(true);
			}
			m_fGroggyTime = 10.f;
			m_iState = STATE_IDLE_SEC;
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

NodeStates CBoss_Juggulus::Idle(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase)
	{
		m_iState = STATE_IDLE_FIRST;
	}
	else if (PHASE_TWO == m_ePhase)
	{
		m_iState = STATE_IDLE_SEC;
	}
	return RUNNING; // SUCCESS
}

NodeStates CBoss_Juggulus::Select_Pattern(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_FLAME_ATTACK == m_iState || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState
		|| STATE_HAMMER_ATTACK == m_iState || STATE_TORNADO_ATTACK == m_iState)
	{
		return FAILURE;
	}

	switch (m_iPastState)
	{
	case STATE_TORNADO_ATTACK:
		m_iState = STATE_HAMMER_ATTACK;
		break;
	case STATE_HAMMER_ATTACK:
		m_iState = STATE_SPHERE_ATTACK;
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_PatternStart.ogg"), SOUND_MONSTER);
		break;
	case STATE_SPHERE_ATTACK:
		m_iState = STATE_FLAME_ATTACK;
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_PatternStart.ogg"), SOUND_MONSTER);
		break;
	case STATE_FLAME_ATTACK:
		m_iState = STATE_THUNDER_ATTACK;
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_PatternStart.ogg"), SOUND_MONSTER);
		break;
	case STATE_THUNDER_ATTACK:
		m_iState = STATE_TORNADO_ATTACK;
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_PatternStart.ogg"), SOUND_MONSTER);
		break;
	default:
		m_iState = STATE_HAMMER_ATTACK;
		break;
	}
	m_iPastState = m_iState;
	return SUCCESS;
}

NodeStates CBoss_Juggulus::HammerAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_FLAME_ATTACK == m_iState || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState
		|| STATE_TORNADO_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_HAMMER_ATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::FlameAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState || STATE_TORNADO_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_FLAME_ATTACK)
	{
		if (m_fFlameSpawnTime == FLAMESPAWNTIME)
		{
			m_fFlameSpawnTime -= 0.01f;
		}
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;

			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::SphereAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_SPHERE_ATTACK)
	{
		if (m_fCircleSphereSpawnTime == CIRCLESPHERESPAWNTIME)
		{
			m_fCircleSphereSpawnTime -= 0.01f;
		}
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;

			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::ThunderAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_TORNADO_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_THUNDER_ATTACK)
	{
		if (m_fLightningSpawnTime == LIGHTNINGSPAWNTIME)
		{
			m_fLightningSpawnTime -= 0.01f;
		}
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;

			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::TornadoAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase)
	{
		return FAILURE;
	}

	if (m_iState == STATE_TORNADO_ATTACK)
	{
		if (m_isAnimFinished)
		{
			_float4 TorPos;
			XMStoreFloat4(&TorPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			TorPos.x += 43.f;
			TorPos.y = 68.f;
			TorPos.z += 4.f;
			EFFECTMGR->Generate_Tornado(0, TorPos, m_pPlayer);
			m_iState = STATE_IDLE_SEC;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}



CBoss_Juggulus* CBoss_Juggulus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Juggulus* pInstance = new CBoss_Juggulus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBoss_Juggulus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Juggulus::Clone(void* pArg)
{
	CBoss_Juggulus* pInstance = new CBoss_Juggulus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBoss_Juggulus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Juggulus::Free()
{
	__super::Free();

	Safe_Release(m_pBehaviorCom);


	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject.second);

	for (auto statue : m_pBossStatues)
		Safe_Release(statue);

	m_PartObjects.clear();

}

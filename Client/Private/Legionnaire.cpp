#include "stdafx.h"
#include "Legionnaire.h"
#include "..\Public\Legionnaire.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Clone.h"
#include "Body_Legionnaire.h"

CLegionnaire::CLegionnaire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CLegionnaire::CLegionnaire(const CLegionnaire& rhs)
	: CMonster{ rhs }
{
}

HRESULT CLegionnaire::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLegionnaire::Initialize(void* pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 3.f, 30.f, 1.f));


	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	return S_OK;
}

void CLegionnaire::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
	m_bAnimFinished = dynamic_cast<CBody_Legionnaire*>(m_PartObjects.front())->Get_AnimFinished();
}

void CLegionnaire::Tick(_float fTimeDelta)
{
	m_pPhysXCom->Tick(fTimeDelta);

	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
}

void CLegionnaire::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
	m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CLegionnaire::Render()
{
	return S_OK;
}

HRESULT CLegionnaire::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 10.f;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(71.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 525.f, 98.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
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

	return S_OK;
}



HRESULT CLegionnaire::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.pCanCombo = &m_bCanCombo;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Legionnaire"), &BodyDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

	///* 무기객체를 복제해온다. */
	//CWeapon::WEAPON_DESC			WeaponDesc{};
	//WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	//WeaponDesc.pState = &m_iState;

	//CModel* pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	//if (nullptr == pModelCom)
	//	return E_FAIL;

	//WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Root_Weapon");
	//if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
	//	return E_FAIL;

	//CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Legionnaire"), &WeaponDesc);
	//if (nullptr == pWeapon)
	//	return E_FAIL;
	//m_PartObjects.emplace_back(pWeapon);

	return S_OK;
}

_bool CLegionnaire::Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider)
{
	//CCollider* pPartObjCollider = dynamic_cast<CCollider*>(m_PartObjects[ePartObjID]->Get_Component(strComponetTag));
	//
	//return pTargetCollider->Intersect(pPartObjCollider);

	return false;
}

HRESULT CLegionnaire::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CLegionnaire::Idle, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Revive"), bind(&CLegionnaire::Revive, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CLegionnaire::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CLegionnaire::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("JumpAttack"), bind(&CLegionnaire::JumpAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack"), bind(&CLegionnaire::Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("CircleAttack"), bind(&CLegionnaire::CircleAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("DetectCool"), 3.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DetectCool"), TEXT("Detect"), bind(&CLegionnaire::Detect, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("MoveTo"), bind(&CLegionnaire::Move, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CLegionnaire::Revive(_float fTimeDelta)
{
	if (m_bReviving)
	{
		m_iState = STATE_REVIVE;

		if (m_bAnimFinished)
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

NodeStates CLegionnaire::Dead(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_K))
	{
		m_bDying = true;
	}

	if (m_bDying)
	{
		m_iState = STATE_DEAD;

		if (m_bAnimFinished)
		{
			// 디졸브, 사망처리
			//m_bDying = false;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CLegionnaire::Hit(_float fTimeDelta)
{
	return FAILURE;
}

NodeStates CLegionnaire::JumpAttack(_float fTimeDelta)
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
			_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vDir = XMVector3Normalize(vDir);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir * m_pTransformCom->Get_Scaled().z);
			m_pPhysXCom->Go_Straight(fTimeDelta * m_fLengthFromPlayer);
		}

		if (m_fLengthFromPlayer < 5.f)
		{
			m_bChasing = false;
		}

		if (m_bAnimFinished)
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

NodeStates CLegionnaire::Attack(_float fTimeDelta)
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

		if (m_bAnimFinished)
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

NodeStates CLegionnaire::CircleAttack(_float fTimeDelta)
{
	if (m_iState == STATE_CIRCLEATTACK)
	{
		if (m_bAnimFinished)
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

NodeStates CLegionnaire::Detect(_float fTimeDelta)
{
	if (m_fLengthFromPlayer > DETECTRANGE)
	{
		return FAILURE;
	}
	else if (m_fLengthFromPlayer > JUMPATTACKRANGE)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_iState = STATE_WALKFRONT;
		return SUCCESS;
	}
	else if (m_fLengthFromPlayer > ATTACKRANGE)
	{
		m_iState = STATE_JUMPATTACK;
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

NodeStates CLegionnaire::Move(_float fTimeDelta)
{
	if (m_iState == STATE_IDLE)
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
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
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
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_Straight(fTimeDelta);
		if (m_fLengthFromPlayer < 5.f)
		{
			m_iState = STATE_IDLE;
			return FAILURE;
		}
	}

	return RUNNING;
}

NodeStates CLegionnaire::Idle(_float fTimeDelta)
{
	m_iState = STATE_IDLE;
	return SUCCESS;
}

void CLegionnaire::Get_Hp(_float iValue)
{
	m_iCurHp += iValue;
	m_iCurHp = max(0, min(m_iCurHp, m_iMaxHp));
}

CLegionnaire* CLegionnaire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLegionnaire* pInstance = new CLegionnaire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLegionnaire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLegionnaire::Clone(void* pArg)
{
	CLegionnaire* pInstance = new CLegionnaire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLegionnaire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLegionnaire::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
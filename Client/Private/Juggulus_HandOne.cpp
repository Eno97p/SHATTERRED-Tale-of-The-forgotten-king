#include "Juggulus_HandOne.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"
#include "EffectManager.h"
#include "SideViewCamera.h"
CJuggulus_HandOne::CJuggulus_HandOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CJuggulus_HandOne::CJuggulus_HandOne(const CJuggulus_HandOne& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CJuggulus_HandOne::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJuggulus_HandOne::Initialize(void* pArg)
{
	CPartObject::PARTOBJ_DESC* pDesc = (CPartObject::PARTOBJ_DESC*)pArg;
	m_pCurHp = pDesc->pCurHp;
	m_pMaxHp = pDesc->pMaxHp;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(1, true));

	m_pTransformCom->Scaling(3.f, 3.f, 3.f);
	m_vParentPos = XMVectorSet(pDesc->pParentMatrix->m[3][0], pDesc->pParentMatrix->m[3][1], pDesc->pParentMatrix->m[3][2], 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vParentPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

void CJuggulus_HandOne::Priority_Tick(_float fTimeDelta)
{
}

void CJuggulus_HandOne::Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 2.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 3.f;
		if (m_fDisolveValue < 0.f)
		{
			//m_pGameInstance->Disable_Echo();
			//m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandAppear.ogg"), SOUND_MONSTER);
			m_eDisolveType = TYPE_INCREASE;
		}
		break;
	default:
		break;
	}

	Change_Animation(fTimeDelta);

	m_pBehaviorCom->Update(fTimeDelta);

	m_pHitColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pAttackColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 몬스터 무기와 플레이어 충돌처리
	if (m_bIsActive)
	{
		if (m_pAttackColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
		{
			m_pPlayer->PlayerHit(10);
		}
	}
	else
	{
		m_pAttackColliderCom->Reset();
	}

	if (m_iState == STATE_ATTACK)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}
}

void CJuggulus_HandOne::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pHitColliderCom);
		m_pGameInstance->Add_DebugComponent(m_pAttackColliderCom);
	}
#endif
}

HRESULT CJuggulus_HandOne::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CJuggulus_HandOne::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CJuggulus_HandOne::Render_LightDepth()
{
	return S_OK;
}

HRESULT CJuggulus_HandOne::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CJuggulus_HandOne::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CJuggulus_HandOne::Hit, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("ChaseCool"), 10.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("ChaseCool"), TEXT("Chase"), bind(&CJuggulus_HandOne::Chase, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack"), bind(&CJuggulus_HandOne::Attack, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CJuggulus_HandOne::Hit(_float fTimeDelta)
{
	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pHitColliderCom->Reset();
		m_eColltype = CCollider::COLL_NOCOLL;
	}
	else if (m_bIsActive)
	{
		m_eColltype = m_pHitColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	}

	switch (m_eColltype)
	{
	case CCollider::COLL_START:
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Hit.ogg"), SOUND_MONSTER, 0.f, 1.f, 0.3f);
		_float4 ParticlePs = XM3TO4(m_pHitColliderCom->Get_Center());
		EFFECTMGR->Generate_Particle(1, ParticlePs, nullptr);
		EFFECTMGR->Generate_Particle(2, ParticlePs, nullptr);
		EFFECTMGR->Generate_Particle(7, ParticlePs);
		Add_Hp(-dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon())->Get_Damage());
		dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.3f, 0.15f);
		break;
	default:
		break;
	}

	return COOLING;
}

NodeStates CJuggulus_HandOne::Chase(_float fTimeDelta)
{
	if (XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_vParentPos)) > 50.f)
	{
		return FAILURE;
	}

	if (m_eDisolveType == TYPE_IDLE && m_iState != STATE_CHASE)
	{
		//m_pGameInstance->Disable_Echo();
		//m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandAppear.ogg"), SOUND_MONSTER);
		m_eDisolveType = TYPE_DECREASE;
	}

	if (m_eDisolveType == TYPE_INCREASE)
	{
		m_iState = STATE_CHASE;
	}

	if (m_iState == STATE_CHASE && m_eDisolveType != TYPE_DECREASE)
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		// 높이값 하드코딩
		vPlayerPos.m128_f32[1] = 21.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
		m_fAttackDelay -= fTimeDelta;
		{
			if (m_fAttackDelay < 0.f)
			{
				m_fColliderActiveTime -= 0.01f;
				m_iState = STATE_ATTACK;
				return SUCCESS;
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CJuggulus_HandOne::Attack(_float fTimeDelta)
{
	if (XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_vParentPos)) > 50.f)
	{
		return FAILURE;
	}

	if (m_iAttackCount == 0 && m_fColliderActiveTime != 0.5f)
	{
		m_fColliderActiveTime -= fTimeDelta;
	}

	if (m_fColliderActiveTime < 0.f)
	{
		/*_float3 vGetCenter = m_pAttackColliderCom->Get_Center();
		_float4 vStartPos = { vGetCenter.x,vGetCenter.y,vGetCenter.z, 1.f };
		EFFECTMGR->Generate_Particle(21, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);*/
		m_bIsActive = true;
		m_fColliderActiveTime = 0.5f;
	}

	if (m_iState == STATE_ATTACK)
	{
		if (m_isAnimFinished)
		{
			if (m_eDisolveType == TYPE_IDLE)
			{
				if (m_iAttackCount == 0)
				{
					_float3 vGetCenter = m_pAttackColliderCom->Get_Center();
					_float4 vStartPos = { vGetCenter.x,vGetCenter.y ,vGetCenter.z, 1.f };
					EFFECTMGR->Generate_Particle(21, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
					EFFECTMGR->Generate_Distortion(9, vStartPos);
					m_pGameInstance->Disable_Echo();
					m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandOne.ogg"), SOUND_MONSTER);
					m_bIsActive = true;

					dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.5f, 0.2f);
				}

				if (m_fDamageTime == 3.f)
				{
					m_iAttackCount++;
				}

				if (m_iAttackCount != 3 || m_fDamageTime < 0.f)
				{
					m_fAttackDelay = 2.f;
					m_eDisolveType = TYPE_DECREASE;
				}
				else if (m_iAttackCount == 3)
				{
					m_fDamageTime -= fTimeDelta;
				}
			}
			else if (m_eDisolveType == TYPE_INCREASE)
			{
				if (m_iAttackCount >= 3)
				{
					m_fColliderActiveTime = 0.5f;
					m_fDamageTime = 3.f;
					m_iAttackCount = 0;
					m_iState = STATE_IDLE;
				}
				else
				{
					_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
					vPlayerPos.m128_f32[1] = 21.5f;
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
					m_iPastAnimIndex = 6;
					m_fColliderActiveTime = 0.5f;
				}
				return SUCCESS;
			}
			else if (m_eDisolveType == TYPE_DECREASE)
			{
				m_bIsActive = false;
				m_fColliderActiveTime = 0.5f;
			}
		}
		else if (m_eDisolveType == TYPE_IDLE && m_iState == STATE_ATTACK && m_iAttackCount != 0 && !m_bIsActive)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandOne.ogg"), SOUND_MONSTER);
			_float3 vGetCenter = m_pAttackColliderCom->Get_Center();
			_float4 vStartPos = { vGetCenter.x,vGetCenter.y ,vGetCenter.z, 1.f };
			EFFECTMGR->Generate_Particle(21, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
			EFFECTMGR->Generate_Distortion(9, vStartPos);
			m_bIsActive = true;

			dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.5f, 0.2f);
		}

		return RUNNING;
	}
	else return FAILURE;
}

NodeStates CJuggulus_HandOne::Idle(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vParentPos);
	m_iState = STATE_IDLE;
	return SUCCESS;
}


void CJuggulus_HandOne::Add_Hp(_int iValue)
{
	*m_pCurHp = min(*m_pMaxHp, max(0, *m_pCurHp + iValue));
}

HRESULT CJuggulus_HandOne::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandOne"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(1.f, 0.3f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y + 15.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pHitColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc2{};

	ColliderDesc2.eType = CCollider::TYPE_AABB;
	ColliderDesc2.vExtents = _float3(0.5f, 0.3f, 0.5f);
	ColliderDesc2.vCenter = _float3(0.f, ColliderDesc2.vExtents.y + 15.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColliderDesc2)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CJuggulus_HandOne::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CJuggulus_HandOne::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 9, true };
	_float fAnimSpeed = 1.f;

	if (m_iState == STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 9;
		fAnimSpeed = 1.f;
	}
	else if (m_iState == STATE_CHASE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		fAnimSpeed = 1.f;
	}
	else if (m_iState == STATE_ATTACK)
	{
		if (m_iPastAnimIndex < 6 || m_iPastAnimIndex > 7)
		{
			m_iPastAnimIndex = 6;
		}



		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true; // false
	if ((m_iPastAnimIndex >= 7 && m_iPastAnimIndex < 8) || AnimDesc.iAnimIndex == 3 || AnimDesc.iAnimIndex == 9)
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_isAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		if (AnimDesc.iAnimIndex >= 6 && AnimDesc.iAnimIndex < 7)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_isAnimFinished = true;
		}
	}
}

CJuggulus_HandOne* CJuggulus_HandOne::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJuggulus_HandOne* pInstance = new CJuggulus_HandOne(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CJuggulus_HandOne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJuggulus_HandOne::Clone(void* pArg)
{
	CJuggulus_HandOne* pInstance = new CJuggulus_HandOne(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CJuggulus_HandOne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJuggulus_HandOne::Free()
{
	__super::Free();

	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pBehaviorCom);
}

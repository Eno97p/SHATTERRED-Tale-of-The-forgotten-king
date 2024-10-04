#include "Juggulus_HandThree.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"

CJuggulus_HandThree::CJuggulus_HandThree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CJuggulus_HandThree::CJuggulus_HandThree(const CJuggulus_HandThree& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CJuggulus_HandThree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJuggulus_HandThree::Initialize(void* pArg)
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

void CJuggulus_HandThree::Priority_Tick(_float fTimeDelta)
{
}

void CJuggulus_HandThree::Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 5.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 5.f;
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
}

void CJuggulus_HandThree::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
}

HRESULT CJuggulus_HandThree::Render()
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

HRESULT CJuggulus_HandThree::Render_Bloom()
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

HRESULT CJuggulus_HandThree::Render_LightDepth()
{
	return S_OK;
}

HRESULT CJuggulus_HandThree::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CJuggulus_HandThree::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown_Priority(TEXT("Attack_Selector"), TEXT("AttackCool"), 15.f, 15.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("AttackCool"), TEXT("Attack"), bind(&CJuggulus_HandThree::Attack, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CJuggulus_HandThree::Attack(_float fTimeDelta)
{
	if (XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_vParentPos)) > 50.f)
	{
		return FAILURE;
	}

	if (m_eDisolveType == TYPE_IDLE && m_iState != STATE_ATTACK)
	{
		//m_pGameInstance->Disable_Echo();
		//m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandAppear.ogg"), SOUND_MONSTER);
		m_eDisolveType = TYPE_DECREASE;
	}

	if (m_eDisolveType == TYPE_INCREASE && m_iState != STATE_ATTACK)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos += XMVectorSet(15.f, -40.f, 0.f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_iState = STATE_ATTACK;
	}

	if (m_iState == STATE_ATTACK)
	{
		//m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_fAspirationDelay -= fTimeDelta;
		if (m_fAspirationDelay < 0.f)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			CGameObject::GAMEOBJECT_DESC gameObjDesc;
			gameObjDesc.fSpeedPerSec = 3.f;
			gameObjDesc.mWorldMatrix._41 = XMVectorGetX(vPos) - 1.f;
			gameObjDesc.mWorldMatrix._42 = XMVectorGetY(vPos) + 70.f;
			gameObjDesc.mWorldMatrix._43 = XMVectorGetZ(vPos);
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Aspiration"), &gameObjDesc);
			m_fAspirationDelay = 100.f;
		}
		if (m_isAnimFinished)
		{
			m_fAspirationDelay = ASPIRATIONDELAY;
			if (m_iAttackCount == 3)
			{
				if (m_eDisolveType == TYPE_IDLE)
				{
					//m_pGameInstance->Disable_Echo();
					//m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandAppear.ogg"), SOUND_MONSTER);
					m_eDisolveType = TYPE_DECREASE;
				}
				if (m_eDisolveType == TYPE_INCREASE)
				{
					m_iState = STATE_IDLE;
					m_iAttackCount = 0;
					return SUCCESS;
				}
			}
			else
			{
				m_iAttackCount++;
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CJuggulus_HandThree::Idle(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vParentPos);
	m_iState = STATE_IDLE;
	return SUCCESS;
}


void CJuggulus_HandThree::Add_Hp(_int iValue)
{
	*m_pCurHp = min(*m_pMaxHp, max(0, *m_pCurHp + iValue));
}

HRESULT CJuggulus_HandThree::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandThree"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

HRESULT CJuggulus_HandThree::Bind_ShaderResources()
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

void CJuggulus_HandThree::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 9, true };
	_float fAnimSpeed = 1.f;

	if (m_iState == STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 1;
		fAnimSpeed = 1.f;
	}
	else if (m_iState == STATE_ATTACK)
	{
		AnimDesc.isLoop = false;
		if (m_iAttackCount != 3 && m_isAnimFinished)
		{
			AnimDesc.iAnimIndex = 3;
		}
		else
		{
			AnimDesc.iAnimIndex = 2;
		}
		fAnimSpeed = 0.7f;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false; // false

	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

CJuggulus_HandThree* CJuggulus_HandThree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJuggulus_HandThree* pInstance = new CJuggulus_HandThree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CJuggulus_HandThree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJuggulus_HandThree::Clone(void* pArg)
{
	CJuggulus_HandThree* pInstance = new CJuggulus_HandThree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CJuggulus_HandThree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJuggulus_HandThree::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pBehaviorCom);
}

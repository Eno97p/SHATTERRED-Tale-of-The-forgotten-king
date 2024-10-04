#include "Andras.h"
#include "GameInstance.h"
#include "Particle_Trail.h"
#include "Weapon.h"

CAndras::CAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAndras::CAndras(const CAndras& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CAndras::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndras::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Add_Nodes()))
		return E_FAIL;
	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(15, true));

	return S_OK;
}

void CAndras::Priority_Tick(_float fTimeDelta)
{
}

void CAndras::Tick(_float fTimeDelta)
{
	m_pBehaviorCom->Update(fTimeDelta);
	Set_AnimIndex(fTimeDelta);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (m_PartObjects.size() >= 1)
	{
		for (auto& iter : m_PartObjects)
			iter->Tick(fTimeDelta);
	}

	m_Head->Tick(fTimeDelta);
}

void CAndras::Late_Tick(_float fTimeDelta)
{
	if (m_PartObjects.size() >= 1)
	{
		for (auto& iter : m_PartObjects)
			iter->Late_Tick(fTimeDelta);
	}
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CAndras::Render()
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

		if (i != 2)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;
		
		if (i > 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		if (i == 2)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CAndras::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_CoolDown(TEXT("Selector"), TEXT("Attack_Cool"), 0.5f);
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Cool"), TEXT("ATTACK"), bind(&CAndras::ATTACK, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Selector"), TEXT("Magic0_Cool"), 0.5f);
	m_pBehaviorCom->Add_Action_Node(TEXT("Magic0_Cool"), TEXT("Magic0"), bind(&CAndras::MAGIC0, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Selector"), TEXT("Magic1_Cool"), 0.5f);
	m_pBehaviorCom->Add_Action_Node(TEXT("Magic1_Cool"), TEXT("Magic1"), bind(&CAndras::MAGIC1, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("WALK"), bind(&CAndras::WALK, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("IDLE"), bind(&CAndras::IDLE, this, std::placeholders::_1));

	return S_OK;
}

HRESULT CAndras::Add_PartObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &CurState;
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword1Start");
	CWeapon* pWeapon =  static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword1"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);


	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword2Start");
	pWeapon  = static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword2"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword3Start");
	pWeapon = static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword3"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword4Start");
	pWeapon = static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword4"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);


	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword5Start");
	pWeapon = static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword5"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Andras-Head");
	m_Head = static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AndrasHead"), &WeaponDesc));

	return S_OK;
}

void CAndras::Set_AnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 33, true };
	_float fAnimSpeed = 1.f;

	switch (CurState)
	{
	case Effect::CAndras::STATE_ATTACK1:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
		break;
	}
	case Effect::CAndras::STATE_ATTACK2:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 16;
		break;
	}
	case Effect::CAndras::STATE_ATTACK3:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 17;
		break;
	}
	case Effect::CAndras::STATE_ATTACK4:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 18;
		break;
	}
	case Effect::CAndras::STATE_ATTACK5:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 19;
		break;
	}
	case Effect::CAndras::STATE_ATTACK6:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 20;
		break;
	}
	case Effect::CAndras::STATE_ATTACK7:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 21;
		break;
	}
	case Effect::CAndras::STATE_ATTACK8:
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 22;
		break;
	}
	case Effect::CAndras::STATE_MAGIC0:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 23;
		break;
	case Effect::CAndras::STATE_MAGIC1:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 24;
		break;
	case Effect::CAndras::STATE_IDLE:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 33;
		break;
	case Effect::CAndras::STATE_WALK:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 35;
		break;
	case Effect::CAndras::STATE_END:
		break;
	default:
		break;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed);

}

const _float4x4* CAndras::Get_WorldMat()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

const _float4x4* CAndras::Get_WeaponMat(_int Index)
{
	if (Index >= m_PartObjects.size())
	{
		MSG_BOX("Out_Of_Size");
		return nullptr;
	}
	else
	{
		m_PartObjects[Index]->Get_WorldMat();
	}
}

const _float4x4* CAndras::Get_HeadMat()
{
	return static_cast<CWeapon*>(m_Head)->Get_WorldMat();
}

NodeStates CAndras::IDLE(_float fTimeDelta)
{
	CurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CAndras::WALK(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		CurState = STATE_WALK;
		return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CAndras::ATTACK(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_1) && CurState > 7)
	{
		CurState = STATE_ATTACK1;
		return RUNNING;
	}
	else if (CurState >= 0 && CurState < 7)
	{
		if (m_pModelCom->Get_AnimFinished())
			CurState++;
		return RUNNING;
	}
	else if (CurState == 7)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			CurState = STATE_IDLE;
			return SUCCESS;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CAndras::MAGIC0(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_2) && CurState != STATE_MAGIC0)
	{
		CurState = STATE_MAGIC0;
		return RUNNING;
	}
	else if (CurState == STATE_MAGIC0)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			CurState = STATE_IDLE;
			return SUCCESS;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CAndras::MAGIC1(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_3) && CurState != STATE_MAGIC1)
	{
		CurState = STATE_MAGIC1;
		return RUNNING;
	}
	else if (CurState == STATE_MAGIC1)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			CurState = STATE_IDLE;
			return SUCCESS;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

HRESULT CAndras::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CAndras::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
#pragma endregion 모션블러

	return S_OK;
}

CAndras* CAndras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndras* pInstance = new CAndras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAndras::Clone(void* pArg)
{
	CAndras* pInstance = new CAndras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndras::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBehaviorCom);
	for (auto& iter : m_PartObjects)
		Safe_Release(iter);

	Safe_Release(m_Head);
}

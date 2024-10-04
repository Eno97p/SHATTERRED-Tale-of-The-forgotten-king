#include "Player.h"
#include "GameInstance.h"
#include "Particle_Trail.h"
#include "Weapon.h"


CPlayerDummy::CPlayerDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayerDummy::CPlayerDummy(const CPlayerDummy& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CPlayerDummy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerDummy::Initialize(void* pArg)
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

void CPlayerDummy::Priority_Tick(_float fTimeDelta)
{
}

void CPlayerDummy::Tick(_float fTimeDelta)
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

	for (auto& iter : m_PartObjects)
		iter->Tick(fTimeDelta);
}

void CPlayerDummy::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_PartObjects)
		iter->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayerDummy::Render()
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

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		else if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
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

HRESULT CPlayerDummy::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Selector"), CBehaviorTree::Selector);
	//m_pBehaviorCom->Add_Node(TEXT("Root"), TEXT("Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("ATTACK"), bind(&CPlayerDummy::ATTACK, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("WALK"), bind(&CPlayerDummy::WALK, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("IDLE"), bind(&CPlayerDummy::IDLE, this, std::placeholders::_1));

	return S_OK;
}

HRESULT CPlayerDummy::Add_PartObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &CurState;
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Bip001-R-Finger3");

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_WhisperSword"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_PartObjects.emplace_back(pWeapon);

	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Bip001-HeadNub_end");
	pWeapon  = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_PlayerHead"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_PartObjects.emplace_back(pWeapon);

	return S_OK;
}

void CPlayerDummy::Set_AnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 15, true };
	_float fAnimSpeed = 1.f;

	switch (CurState)
	{
	case Effect::CPlayerDummy::STATE_ATTACK:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 19;
		break;
	case Effect::CPlayerDummy::STATE_IDLE:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 15;
		break;
	case Effect::CPlayerDummy::STATE_WALK:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 44;
		break;
	case Effect::CPlayerDummy::STATE_END:
		break;
	default:
		break;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed);

}

const _float4x4* CPlayerDummy::Get_WorldMat()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

const _float4x4* CPlayerDummy::Get_HeadMAt()
{
	return static_cast<CPartObject*>(m_PartObjects[1])->Get_WorldMat();
}


const _float4x4* CPlayerDummy::Get_WeaponMat()
{
	return static_cast<CPartObject*>(m_PartObjects[0])->Get_WorldMat();
}

NodeStates CPlayerDummy::IDLE(_float fTimeDelta)
{
	CurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CPlayerDummy::WALK(_float fTimeDelta)
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

NodeStates CPlayerDummy::ATTACK(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_A) && CurState != STATE_ATTACK)
	{
		CurState = STATE_ATTACK;
		return RUNNING;
	}

	if (CurState == STATE_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
			return FAILURE;
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

HRESULT CPlayerDummy::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
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

HRESULT CPlayerDummy::Bind_ShaderResources()
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

CPlayerDummy* CPlayerDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerDummy* pInstance = new CPlayerDummy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerDummy::Clone(void* pArg)
{
	CPlayerDummy* pInstance = new CPlayerDummy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerDummy::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBehaviorCom);
	for (auto& iter : m_PartObjects)
		Safe_Release(iter);
}

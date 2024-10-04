#include "HoverBoard.h"
#include "GameInstance.h"
#include "Particle_Trail.h"
#include "Weapon.h"

CHoverBoard::CHoverBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CHoverBoard::CHoverBoard(const CHoverBoard& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CHoverBoard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHoverBoard::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(360.f);
	desc.fSpeedPerSec = 10.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Add_Nodes()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

void CHoverBoard::Priority_Tick(_float fTimeDelta)
{
}

void CHoverBoard::Tick(_float fTimeDelta)
{
	m_pBehaviorCom->Update(fTimeDelta);

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
}

void CHoverBoard::Late_Tick(_float fTimeDelta)
{
	if (m_PartObjects.size() >= 1)
	{
		for (auto& iter : m_PartObjects)
			iter->Late_Tick(fTimeDelta);
	}
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CHoverBoard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CHoverBoard::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("MOVE"), bind(&CHoverBoard::MOVE, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Selector"), TEXT("IDLE"), bind(&CHoverBoard::IDLE, this, std::placeholders::_1));
	return S_OK;
}

HRESULT CHoverBoard::Add_PartObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &CurState;
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Sword1Start");
	CWeapon* pWeapon =  static_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Andras_Sword1"), &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	return S_OK;
}



const _float4x4* CHoverBoard::Get_WorldMat()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

const _float4x4* CHoverBoard::Get_WeaponMat(_int Index)
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



NodeStates CHoverBoard::IDLE(_float fTimeDelta)
{
	CurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CHoverBoard::MOVE(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		CurState = STATE_MOVE;
		return RUNNING;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
		CurState = STATE_MOVE;
		return RUNNING;
	}
	else
		return FAILURE;
}




HRESULT CHoverBoard::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hoverboard"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CHoverBoard::Bind_ShaderResources()
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

CHoverBoard* CHoverBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHoverBoard* pInstance = new CHoverBoard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHoverBoard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHoverBoard::Clone(void* pArg)
{
	CHoverBoard* pInstance = new CHoverBoard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHoverBoard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHoverBoard::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBehaviorCom);
	for (auto& iter : m_PartObjects)
		Safe_Release(iter);

}

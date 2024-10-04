#include "stdafx.h"
#include "..\Public\Clone.h"

#include "GameInstance.h"
#include "Player.h"

CClone::CClone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CClone::CClone(const CClone& rhs)
	: CBlendObject{ rhs }
{
}

HRESULT CClone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CClone::Initialize(void* pArg)
{

	CLONE_DESC* pCloneDesc = (CLONE_DESC*)pArg;

	m_pParentMatrix = pCloneDesc->pParentMatrix;
	m_pState = pCloneDesc->pState;
	m_fAnimDelay = pCloneDesc->fAnimDelay;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vParentMatrix = *m_pParentMatrix;

	CModel::ANIMATION_DESC		AnimDesc{ 3, true };

	if (*m_pState == CPlayer::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 15;
	}

	if (*m_pState == CPlayer::STATE_RUN)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 38;
	}

	if (*m_pState == CPlayer::STATE_ROLL)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 32;
	}

	if (*m_pState == CPlayer::STATE_DASH)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 59;
	}
	else if (*m_pState == CPlayer::STATE_DASH_FRONT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
	}
	else if (*m_pState == CPlayer::STATE_DASH_BACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
	}
	else if (*m_pState == CPlayer::STATE_DASH_LEFT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 8;
	}
	else if (*m_pState == CPlayer::STATE_DASH_RIGHT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 166;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	m_pModelCom->Play_Animation(m_fAnimDelay, false);
	m_fAnimDelay = 0.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_vParentMatrix));
	return S_OK;
}

void CClone::Priority_Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0.1f)
	{
		m_pGameInstance->Erase(this);
	}
}

void CClone::Tick(_float fTimeDelta)
{


}

void CClone::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CClone::Render()
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
		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(5);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CClone::Render_Bloom()
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
		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(5);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CClone::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CClone::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fLifeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CClone* CClone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CClone* pInstance = new CClone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CClone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CClone::Clone(void* pArg)
{
	CClone* pInstance = new CClone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CClone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClone::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
#include "stdafx.h"
#include "..\Public\Body_Legionnaire.h"

#include "GameInstance.h"
#include "Legionnaire.h"

CBody_Legionnaire::CBody_Legionnaire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Legionnaire::CBody_Legionnaire(const CBody_Legionnaire& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Legionnaire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Legionnaire::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody_Legionnaire::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Legionnaire::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	*m_pCanCombo = false;
	if (*m_pState == CLegionnaire::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_WALKLEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 28;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_WALKRIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 29;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_WALKFRONT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 27;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_WALKBACK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 26;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_JUMPATTACK)
	{
		if (m_iPastAnimIndex < 17 || m_iPastAnimIndex > 25)
		{
			m_iPastAnimIndex = 17;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_ATTACK1)
	{
		if (m_iPastAnimIndex < 0 || m_iPastAnimIndex > 1)
		{
			m_iPastAnimIndex = 0;
		}
		if (m_iPastAnimIndex == 1) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_ATTACK2)
	{
		if (m_iPastAnimIndex < 2 || m_iPastAnimIndex > 4)
		{
			m_iPastAnimIndex = 2;
		}
		if (m_iPastAnimIndex == 4) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_ATTACK3)
	{
		if (m_iPastAnimIndex < 5 || m_iPastAnimIndex > 6)
		{
			m_iPastAnimIndex = 5;
		}
		if (m_iPastAnimIndex == 6) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_CIRCLEATTACK)
	{
		if (m_iPastAnimIndex < 7 || m_iPastAnimIndex > 10)
		{
			m_iPastAnimIndex = 7;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CLegionnaire::STATE_REVIVE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 12;
		fAnimSpeed = 1.f;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	// 여러 애니메이션을 이어서 재생할 때는 보간하지 않음
	if ((m_iPastAnimIndex >= 8 && m_iPastAnimIndex < 11) || (m_iPastAnimIndex >= 18 && m_iPastAnimIndex < 26) ||
		(m_iPastAnimIndex >= 1 && m_iPastAnimIndex < 2) || (m_iPastAnimIndex >= 3 && m_iPastAnimIndex < 5) ||
		(m_iPastAnimIndex >= 6 && m_iPastAnimIndex < 7))
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		if (AnimDesc.iAnimIndex >= 7 && AnimDesc.iAnimIndex < 10)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 0 && AnimDesc.iAnimIndex < 1)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 2 && AnimDesc.iAnimIndex < 4)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 5 && AnimDesc.iAnimIndex < 6)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 17 && AnimDesc.iAnimIndex < 25)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 99999;
			m_bAnimFinished = true;
		}
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Legionnaire::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//}


}

HRESULT CBody_Legionnaire::Render()
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
		
		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Legionnaire::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Legionnaire::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Legionnaire::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CBody_Legionnaire* CBody_Legionnaire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Legionnaire* pInstance = new CBody_Legionnaire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Legionnaire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Legionnaire::Clone(void* pArg)
{
	CBody_Legionnaire* pInstance = new CBody_Legionnaire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Legionnaire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Legionnaire::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

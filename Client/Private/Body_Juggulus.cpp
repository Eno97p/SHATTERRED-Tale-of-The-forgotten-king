#include "Body_Juggulus.h"

#include "GameInstance.h"
#include "Boss_Juggulus.h"

CBody_Juggulus::CBody_Juggulus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Juggulus::CBody_Juggulus(const CBody_Juggulus& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Juggulus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Juggulus::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(16, true));

	return S_OK;
}

void CBody_Juggulus::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 0.5f;
		break;
	default:
		break;

	}
}

void CBody_Juggulus::Tick(_float fTimeDelta)
{
	Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Juggulus::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

HRESULT CBody_Juggulus::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 1; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR)))
			return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CBody_Juggulus::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 1; i < iNumMeshes; i++)
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

HRESULT CBody_Juggulus::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_Juggulus::Render_LightDepth()
{
	return S_OK;
}

HRESULT CBody_Juggulus::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Juggulus"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Juggulus::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

void CBody_Juggulus::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 16, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CBoss_Juggulus::STATE_IDLE_FIRST)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_IDLE_SEC)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 17;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_NEXTPHASE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		fAnimSpeed = 0.5f;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_CREATE_HAMMER)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		fAnimSpeed = 0.5f;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_FLAME_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_HAMMER_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 18;
		if (m_pModelCom->Get_Ratio_Betwin(0.5f, 0.7f))
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CBoss_Juggulus::STATE_SPHERE_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 19;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_THUNDER_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 19;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_TORNADO_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 19;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 14;
	}
	else if (*m_pState == CBoss_Juggulus::STATE_GROGGY)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 17;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true; // false
	//// 여러 애니메이션을 재생할 때 마지막 애니메이션은 보간 필요
	//if (m_iPastAnimIndex == 37 || m_iPastAnimIndex == 28 || (m_iPastAnimIndex == 0 && *m_pState != CPlayer::STATE_ATTACK))
	//{
	//	isLerp = true;
	//}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);
}

CBody_Juggulus* CBody_Juggulus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Juggulus* pInstance = new CBody_Juggulus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Juggulus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Juggulus::Clone(void* pArg)
{
	CBody_Juggulus* pInstance = new CBody_Juggulus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Juggulus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Juggulus::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}

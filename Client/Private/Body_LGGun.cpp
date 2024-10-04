#include "Body_LGGun.h"

#include "GameInstance.h"
#include "Legionnaire_Gun.h"
#include "Weapon.h"

CBody_LGGun::CBody_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_LGGun::CBody_LGGun(const CBody_LGGun& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_LGGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_LGGun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(4, true));

	return S_OK;
}

void CBody_LGGun::Priority_Tick(_float fTimeDelta)
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

void CBody_LGGun::Tick(_float fTimeDelta)
{
	Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_LGGun::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
}

HRESULT CBody_LGGun::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
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

HRESULT CBody_LGGun::Render_Bloom()
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

HRESULT CBody_LGGun::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_LGGun::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4 fPos = m_pGameInstance->Get_PlayerPos();

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPos.x, fPos.y + 10.f, fPos.z - 10.f, 1.f), XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
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

HRESULT CBody_LGGun::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
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

HRESULT CBody_LGGun::Bind_ShaderResources()
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

void CBody_LGGun::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 16, true };
	_float fAnimSpeed = 1.f;

	*m_pCanCombo = false;
	if (*m_pState == CLegionnaire_Gun::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_IDLE_GUN)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 14;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_IDLE_MELEE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 17;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(2.0);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 46;
		fAnimSpeed = 2.f; // 1
		m_pModelCom->Set_LerpTime(1.0); // 1.2
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_PARRIED)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 46;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_WAKEUP)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 12;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_KNOCKDOWN)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 10;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 10;
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_WALK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 21;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_RUN)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 19;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_BACK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 20;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_LEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 22;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_RIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 24;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pSword->Set_Active(false);
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_GUNATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 1.3f && m_fDamageTiming < 1.4f)
		{
			m_pGun->Set_Active();
			m_fDamageTiming += 0.2f;
		}
		else
		{
			m_pGun->Set_Active(false);
		}
		m_pSword->Set_Active(false);
		
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_MELEEATTACK1)
	{
		if (m_iPastAnimIndex < 26 || m_iPastAnimIndex > 28)
		{
			m_iPastAnimIndex = 27;
		}
		if(m_iPastAnimIndex == 27)
			fAnimSpeed = 1.3f;
		else if(m_iPastAnimIndex == 28)
			fAnimSpeed = 1.5f;
		else
			fAnimSpeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(2.2);
		if (m_iPastAnimIndex == 27)
		{
			m_pSword->Set_Active();
		}
		else
		{
			m_pSword->Set_Active(false);
		}
		m_pGun->Set_Active(false);
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_MELEEATTACK2)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 32;
		fAnimSpeed = 1.4f;
		m_pModelCom->Set_LerpTime(1.2);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 0.5f && m_fDamageTiming < 0.8f)
		{
			m_pSword->Set_Active();
		}
		else
		{
			m_pSword->Set_Active(false);
		}
		m_pGun->Set_Active(false);
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	// 여러 애니메이션을 이어서 재생할 때는 보간하지 않음
	if ((m_iPastAnimIndex >= 27 && m_iPastAnimIndex < 29) || AnimDesc.iAnimIndex == 32)
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_isAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		if (AnimDesc.iAnimIndex >= 26 && AnimDesc.iAnimIndex < 28)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 0;
			m_isAnimFinished = true;
		}
	}
	if (m_isAnimFinished)
	{
		m_fDamageTiming = 0.f;
	}
}

CBody_LGGun* CBody_LGGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_LGGun* pInstance = new CBody_LGGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_LGGun::Clone(void* pArg)
{
	CBody_LGGun* pInstance = new CBody_LGGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_LGGun::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

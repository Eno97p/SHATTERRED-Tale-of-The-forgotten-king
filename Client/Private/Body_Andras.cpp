#include "stdafx.h"
#include "..\Public\Body_Andras.h"

#include "GameInstance.h"
#include "Andras.h"
#include "Weapon.h"
#include "EffectManager.h"

CBody_Andras::CBody_Andras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Andras::CBody_Andras(const CBody_Andras& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Andras::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Andras::Initialize(void* pArg)
{
	BODY_ANDRAS_DESC* pDesc = (BODY_ANDRAS_DESC*)pArg;
	m_bSprint = pDesc->bSprint;
	m_bKick = pDesc->bKick;
	m_bSlash = pDesc->bSlash;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(rand() % 20, true));

	return S_OK;
}

void CBody_Andras::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
	{
		m_fDisolveValue -= fTimeDelta * 0.2f;
		EFFECTMGR->Generate_Particle(115, _float4(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f));
		break;
	}
	default:
		break;
	}
}

void CBody_Andras::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	*m_pCanCombo = false;
	if (*m_pState == CAndras::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 33;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CAndras::STATE_DASHLEFT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 36;
		fAnimSpeed = 7.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dash.ogg"), SOUND_MONSTER, 0.f, 1.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_DASHRIGHT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 37;
		fAnimSpeed = 7.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dash.ogg"), SOUND_MONSTER, 0.f, 1.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_DASHFRONT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 35;
		fAnimSpeed = 7.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dash.ogg"), SOUND_MONSTER, 0.f, 1.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_DASHBACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 34;
		fAnimSpeed = 7.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dash.ogg"), SOUND_MONSTER, 0.f, 1.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_ATTACK1)
	{
		if (m_iPastAnimIndex < 10 || m_iPastAnimIndex > 13)
		{
			m_iPastAnimIndex = 10;
		}
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 11 || m_iPastAnimIndex == 12)
		{
			m_pWeapon[2]->Set_Active();
		}
		else
		{
			m_pWeapon[2]->Set_Active(false);
		}
	}
	else if (*m_pState == CAndras::STATE_ATTACK2)
	{
		if (m_iPastAnimIndex < 0 || m_iPastAnimIndex > 4)
		{
			m_iPastAnimIndex = 0;
		}
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 1)
		{
			m_pWeapon[0]->Set_Active();
		}
		else
		{
			m_pWeapon[0]->Set_Active(false);
		}
	}
	else if (*m_pState == CAndras::STATE_ATTACK3)
	{
		if (m_iPastAnimIndex < 10 || m_iPastAnimIndex > 13)
		{
			m_iPastAnimIndex = 10;
		}
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 11 || m_iPastAnimIndex == 12)
		{
			m_pWeapon[2]->Set_Active();
		}
		else
		{
			m_pWeapon[2]->Set_Active(false);
		}
	}
	else if (*m_pState == CAndras::STATE_ATTACK4)
	{
		if (m_iPastAnimIndex < 0 || m_iPastAnimIndex > 4)
		{
			m_iPastAnimIndex = 0;
		}
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 1)
		{
			m_pWeapon[0]->Set_Active();
		}
		else
		{
			m_pWeapon[0]->Set_Active(false);
		}
	}
	else if (*m_pState == CAndras::STATE_SPRINTATTACK)
	{
		if (m_iPastAnimIndex < 25 || m_iPastAnimIndex > 30)
		{
			m_iPastAnimIndex = 25;
		}
		if (m_iPastAnimIndex == 27 && m_pModelCom->Check_CurDuration(0.01f))
		{
			*m_bSprint = true;
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dash.ogg"), SOUND_MONSTER, 0.f, 1.5f);
		}
		// 애니메이션 속도가 빠르면 플레이어에 도달하기도 전에 칼을 휘두를 수 있기 때문에 주의
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 28 || m_iPastAnimIndex == 29)
		{
			m_pWeapon[0]->Set_Active();
			m_pWeapon[4]->Set_Active();
		}
		else
		{
			m_pWeapon[0]->Set_Active(false);
			m_pWeapon[4]->Set_Active(false);
		}
	}
	else if (*m_pState == CAndras::STATE_GROUNDATTACK)
	{
		if (m_pModelCom->Check_CurDuration(0.63f))
		{
			*m_bSlash = true;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 23;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Charge.ogg"), SOUND_MONSTER, 0.f, 0.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_KICKATTACK)
	{
		if (m_iPastAnimIndex < 15 || m_iPastAnimIndex > 22)
		{
			m_iPastAnimIndex = 15;
		}
		if (m_iPastAnimIndex == 19 && m_pModelCom->Check_CurDuration(0.4f))
		{
			*m_bSprint = true;
		}
		if (m_iPastAnimIndex == 19 && m_pModelCom->Check_CurDuration(0.9f))
		{
			*m_bKick = true;
		}
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
	}
	else if (*m_pState == CAndras::STATE_LASERATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 24;
		if (m_pModelCom->Get_Ratio_Betwin(0.5f, 1.f))
		{
			fAnimSpeed = 0.5f;
		}
		else
		{
			fAnimSpeed = 1.f;
		}
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CAndras::STATE_BABYLONATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 24;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Charge.ogg"), SOUND_MONSTER, 0.f, 0.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_SHOOTINGSTARATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 23;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Charge.ogg"), SOUND_MONSTER, 0.f, 0.5f);
		}
	}
	else if (*m_pState == CAndras::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 32;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Dead.ogg"), SOUND_MONSTER);
		}
	}

	if (*m_pState == CAndras::STATE_DASHLEFT || *m_pState == CAndras::STATE_DASHRIGHT ||
		*m_pState == CAndras::STATE_DASHFRONT || *m_pState == CAndras::STATE_DASHBACK ||
		*m_pState == CAndras::STATE_SPRINTATTACK || *m_pState == CAndras::STATE_KICKATTACK)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	// 여러 애니메이션을 이어서 재생할 때는 보간하지 않음
	if ((m_iPastAnimIndex >= 1 && m_iPastAnimIndex < 6) || (m_iPastAnimIndex >= 26 && m_iPastAnimIndex < 31) || (m_iPastAnimIndex >= 7 && m_iPastAnimIndex < 10) ||
		(m_iPastAnimIndex >= 11 && m_iPastAnimIndex < 15) || (m_iPastAnimIndex >= 16 && m_iPastAnimIndex < 23))
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		if (AnimDesc.iAnimIndex >= 0 && AnimDesc.iAnimIndex < 4)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 6 && AnimDesc.iAnimIndex < 8)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 10 && AnimDesc.iAnimIndex < 13)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 15 && AnimDesc.iAnimIndex < 22)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 25 && AnimDesc.iAnimIndex < 30)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 99999;
			m_bAnimFinished = true;
		}
	}
	if (m_bAnimFinished)
	{
		m_pWeapon[0]->Set_Active(false);
		m_pWeapon[1]->Set_Active(false);
		m_pWeapon[2]->Set_Active(false);
		m_pWeapon[3]->Set_Active(false);
		m_pWeapon[4]->Set_Active(false);
		m_fDamageTiming = 0.f;
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Andras::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//}


}

HRESULT CBody_Andras::Render()
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

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CBody_Andras::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Andras::Render_LightDepth()
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

HRESULT CBody_Andras::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras"),
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

HRESULT CBody_Andras::Bind_ShaderResources()
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
	_float4 fDisolveColor = _float4(1.f, 0.5f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveColor", &fDisolveColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBody_Andras* CBody_Andras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Andras* pInstance = new CBody_Andras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Andras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Andras::Clone(void* pArg)
{
	CBody_Andras* pInstance = new CBody_Andras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Andras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Andras::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
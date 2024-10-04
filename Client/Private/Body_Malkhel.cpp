#include "stdafx.h"
#include "..\Public\Body_Malkhel.h"

#include "GameInstance.h"
#include "Malkhel.h"
#include "Weapon.h"
#include "EffectManager.h"
#include "ThirdPersonCamera.h"

CBody_Malkhel::CBody_Malkhel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Malkhel::CBody_Malkhel(const CBody_Malkhel& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Malkhel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Malkhel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(12, true));

	return S_OK;
}

void CBody_Malkhel::Priority_Tick(_float fTimeDelta)
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

void CBody_Malkhel::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CMalkhel::STATE_TELEPORT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 12;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Teleport.ogg"), SOUND_MONSTER);
		}
		m_pWeapon->Set_Active(false);
	}
	if (*m_pState == CMalkhel::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 12;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_DASHLEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Dash.ogg"), SOUND_MONSTER);
		}
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_DASHRIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Dash.ogg"), SOUND_MONSTER);
		}
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_DASHFRONT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 14;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Dash.ogg"), SOUND_MONSTER);
		}
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_DASHBACK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 13;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Dash.ogg"), SOUND_MONSTER);
		}
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK1)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		m_pWeapon->Set_Active(false);
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK2)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		if (m_pModelCom->Get_Ratio_Betwin(0.45f, 0.8f) && m_bAttacking)
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK3)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		if (m_pModelCom->Get_Ratio_Betwin(0.3f, 0.4f))
		{
			m_pWeapon->Set_Active();
		}
		else if (m_pModelCom->Get_Ratio_Betwin(0.55f, 0.65f))
		{
			m_pWeapon->Set_Active();
		}
		else if (m_pModelCom->Get_Ratio_Betwin(0.8f, 0.9f))
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
		if (m_pModelCom->Check_CurDuration(0.35f) || m_pModelCom->Check_CurDuration(0.6f) || m_pModelCom->Check_CurDuration(0.85f))
		{
			CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
			pThirdPersonCamera->Shake_Camera(0.23f, 0.01f, 0.03f, 72.f);

			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Attack3.ogg"), SOUND_MONSTER);
			_vector vPos = XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f);
			_vector vLook = XMVectorSet(m_pParentMatrix->_31, m_pParentMatrix->_32, m_pParentMatrix->_33, 0.f);
			_float4 fPos;
			XMStoreFloat4(&fPos, vPos + 2.f * vLook);
			EFFECTMGR->Generate_Lightning(1, fPos);
		}
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK4)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		if (m_pModelCom->Get_Ratio_Betwin(0.4f, 0.7f))
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Attack4.ogg"), SOUND_MONSTER);
		}
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK5)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		if (m_pModelCom->Get_Ratio_Betwin(0.5f, 0.8f) && m_bAttacking)
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Attack4.ogg"), SOUND_MONSTER);
		}
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK6)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_float4 fPos = _float4(m_pParentMatrix->_41, m_pParentMatrix->_42 + 2.f, m_pParentMatrix->_43, 1.f);
			_vector vLook = XMVectorSet(m_pParentMatrix->_31, m_pParentMatrix->_32, m_pParentMatrix->_33, 0.f);
			EFFECTMGR->Generate_Particle(68, fPos, nullptr, XMVectorZero(), 0.f, vLook);
			EFFECTMGR->Generate_Particle(69, fPos, nullptr, XMVectorZero(), 0.f, vLook);
		}
		m_pWeapon->Set_Active(false);
	}
	else if (*m_pState == CMalkhel::STATE_ATTACK7)
	{
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		if (m_pModelCom->Get_Ratio_Betwin(0.5f, 0.8f))
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
		m_bAttacking = true;
	}
	else if (*m_pState == CMalkhel::STATE_DEAD)
	{
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
	}
	

	if (*m_pState == CMalkhel::STATE_DASHLEFT || *m_pState == CMalkhel::STATE_DASHRIGHT ||
		*m_pState == CMalkhel::STATE_DASHFRONT || *m_pState == CMalkhel::STATE_DASHBACK)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = m_pModelCom->Get_AnimFinished();

	if (m_bAnimFinished)
	{
		m_bAttacking = false;
		m_pWeapon->Set_Active(false);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Malkhel::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//}
}

HRESULT CBody_Malkhel::Render()
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

		if (i <= 1)
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

HRESULT CBody_Malkhel::Render_Reflection()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_float4x4 ViewMatrix;
	const _float4x4* matCam = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	// 원래 뷰 행렬 로드
	XMMATRIX mOriginalView = XMLoadFloat4x4(matCam);

	// 카메라 위치 추출
	XMVECTOR vCamPos = XMVector3Transform(XMVectorZero(), mOriginalView);

	// 바닥 평면의 높이 (예: Y = 0)
	float floorHeight = 300.f;

	// 반사된 카메라 위치 계산 (Y 좌표만 반전)
	XMVECTOR vReflectedCamPos = vCamPos;
	vReflectedCamPos = XMVectorSetY(vReflectedCamPos, 2 * floorHeight - XMVectorGetY(vCamPos));

	// 카메라 방향 벡터 추출
	XMVECTOR vCamLook = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 0, 1, 0), mOriginalView) - vCamPos);
	XMVECTOR vCamUp = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 1, 0, 0), mOriginalView) - vCamPos);

	// 반사된 카메라 방향 벡터 계산
	XMVECTOR vReflectedCamLook = XMVectorSetY(vCamLook, -XMVectorGetY(vCamLook));
	XMVECTOR vReflectedCamUp = XMVectorSetY(vCamUp, -XMVectorGetY(vCamUp));

	// 반사된 뷰 행렬 생성
	XMMATRIX mReflectedView = XMMatrixLookToLH(vReflectedCamPos, vReflectedCamLook, vReflectedCamUp);

	// 변환된 행렬 저장
	XMStoreFloat4x4(&ViewMatrix, mReflectedView);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (i <= 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(10);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Malkhel::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Malkhel"),
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

HRESULT CBody_Malkhel::Bind_ShaderResources()
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

CBody_Malkhel* CBody_Malkhel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Malkhel* pInstance = new CBody_Malkhel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Malkhel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Malkhel::Clone(void* pArg)
{
	CBody_Malkhel* pInstance = new CBody_Malkhel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Malkhel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Malkhel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
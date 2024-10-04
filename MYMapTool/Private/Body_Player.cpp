
#include "..\Public\Body_Player.h"
#include "GameInstance.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

//	m_pTransformCom->Scaling(0.07f, 0.07f, 0.07f);
	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
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
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 0.f;
		}
		break;
	default:
		break;
	}
}

void CBody_Player::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	*m_pCanCombo = false;
	if (*m_pState == CPlayer::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_FIGHTIDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 14;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_WALK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 44;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_RUN)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 38;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 13;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_JUMPSTART)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 18;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_DOUBLEJUMPSTART)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_JUMP)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_LAND)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 17;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_PARRY)
	{
		if (m_iPastAnimIndex < 26 || m_iPastAnimIndex > 28)
		{
			m_iPastAnimIndex = 26;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_JUMPATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 210;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_JUMPATTACK_LAND)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 209;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_ROLLATTACK)
	{
		if (m_iPastAnimIndex < 33 || m_iPastAnimIndex > 37)
		{
			m_iPastAnimIndex = 33;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_BACKATTACK)
	{
		if (m_iPastAnimIndex < 70 || m_iPastAnimIndex > 74)
		{
			m_iPastAnimIndex = 70;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_LCHARGEATTACK)
	{
		if (m_iPastAnimIndex < 150 || m_iPastAnimIndex > 153)
		{
			m_iPastAnimIndex = 150;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.5f;
	}
	else if (*m_pState == CPlayer::STATE_RCHARGEATTACK)
	{
		if (m_iPastAnimIndex < 131 || m_iPastAnimIndex > 136)
		{
			m_iPastAnimIndex = 131;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.5f;
	}
	else if (*m_pState == CPlayer::STATE_LATTACK1)
	{
		if (m_iPastAnimIndex < 144 || m_iPastAnimIndex > 149)
		{
			m_iPastAnimIndex = 144;
		}
		if (m_iPastAnimIndex == 149) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_LATTACK2)
	{
		if (m_iPastAnimIndex < 138 || m_iPastAnimIndex > 142)
		{
			m_iPastAnimIndex = 138;
		}
		if (m_iPastAnimIndex == 142) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_LATTACK3)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 143;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_RATTACK1)
	{
		if (m_iPastAnimIndex < 121 || m_iPastAnimIndex > 125)
		{
			m_iPastAnimIndex = 121;
		}
		if (m_iPastAnimIndex == 125) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_RATTACK2)
	{
		if (m_iPastAnimIndex < 75 || m_iPastAnimIndex > 79)
		{
			m_iPastAnimIndex = 75;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_RUNLATTACK1)
	{
		if (m_iPastAnimIndex < 137 || m_iPastAnimIndex > 142)
		{
			m_iPastAnimIndex = 137;
		}
		if (m_iPastAnimIndex == 142) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_RUNLATTACK2)
	{
		if (m_iPastAnimIndex < 82 || m_iPastAnimIndex > 85)
		{
			m_iPastAnimIndex = 82;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_RUNRATTACK)
	{
		if (m_iPastAnimIndex < 75 || m_iPastAnimIndex > 79)
		{
			m_iPastAnimIndex = 75;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_COUNTER)
	{
		if (m_iPastAnimIndex < 48 || m_iPastAnimIndex > 53)
		{
			m_iPastAnimIndex = 48;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_ROLL)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 32;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_DASH)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 165;
		fAnimSpeed = 2.f;
	}
	else if (*m_pState == CPlayer::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CPlayer::STATE_REVIVE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 40;
		fAnimSpeed = 1.f;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false;
	// 여러 애니메이션을 재생할 때 마지막 애니메이션은 보간 필요
	if (m_iPastAnimIndex == 37 || m_iPastAnimIndex == 28 || m_iPastAnimIndex == 53 || m_iPastAnimIndex == 149 || 
		m_iPastAnimIndex == 74 || m_iPastAnimIndex == 153 || m_iPastAnimIndex == 79 || m_iPastAnimIndex == 125 || m_iPastAnimIndex == 136 ||
		m_iPastAnimIndex == 142 || AnimDesc.iAnimIndex == 209 || m_iPastAnimIndex == 85 ||
		(m_iPastAnimIndex == 0 && *m_pState != CPlayer::STATE_LATTACK1 && *m_pState != CPlayer::STATE_LATTACK2
			&& *m_pState != CPlayer::STATE_LATTACK3 && *m_pState != CPlayer::STATE_RATTACK1))
	{
		isLerp = true;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		// rollattack
		if (AnimDesc.iAnimIndex >= 33 && AnimDesc.iAnimIndex < 37)
		{
			m_iPastAnimIndex++;
		}
		// 패링
		else if (AnimDesc.iAnimIndex >= 26 && AnimDesc.iAnimIndex < 28)
		{
			m_iPastAnimIndex++;
		}
		// 반격
		else if (AnimDesc.iAnimIndex >= 48 && AnimDesc.iAnimIndex < 53)
		{
			m_iPastAnimIndex++;
		}
		// 공격1
		else if (AnimDesc.iAnimIndex >= 144 && AnimDesc.iAnimIndex < 149)
		{
			m_iPastAnimIndex++;
		}
		// 공격2, 달리기공격2
		else if (AnimDesc.iAnimIndex >= 82 && AnimDesc.iAnimIndex < 85)
		{
			m_iPastAnimIndex++;
		}
		// 달리기공격1
		else if (AnimDesc.iAnimIndex >= 137 && AnimDesc.iAnimIndex < 142)
		{
			m_iPastAnimIndex++;
		}
		// 우공격1
		else if (AnimDesc.iAnimIndex >= 121 && AnimDesc.iAnimIndex < 125)
		{
			m_iPastAnimIndex++;
		}
		// 달리기 우공격, 우공격2
		else if (AnimDesc.iAnimIndex >= 75 && AnimDesc.iAnimIndex < 79)
		{
			m_iPastAnimIndex++;
		}
		// 뒤잡
		else if (AnimDesc.iAnimIndex >= 70 && AnimDesc.iAnimIndex < 74)
		{
			m_iPastAnimIndex++;
		}
		// 왼 차지공격
		else if (AnimDesc.iAnimIndex >= 150 && AnimDesc.iAnimIndex < 153)
		{
			m_iPastAnimIndex++;
		}
		// 우 차지공격
		else if (AnimDesc.iAnimIndex >= 131 && AnimDesc.iAnimIndex < 136)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 0;
			m_bAnimFinished = true;
		}
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_C))
	{
		m_bIsClocking = true;
	}
	else
	{
		m_bIsClocking = false;
	}

	//if (m_bIsClocking)
	//{
	//	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
	//}
	//else
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);
	}

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//}


}

HRESULT CBody_Player::Render()
{
	if (m_bIsClocking)
	{
		return Render_Distortion();
	}

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

		if (i == 2)
		{
		/*	if (FAILED(m_pDisolveCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
				return E_FAIL;*/
		/*	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
				return E_FAIL;*/
			//m_pShaderCom->Begin(7);
		}
		else
		{
			m_pShaderCom->Begin(0);
		}
		

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_Player::Render_Reflection()
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
	float floorHeight = 271.f;

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

	
		{
			m_pShaderCom->Begin(5);
		}


		m_pModelCom->Render(i);
	}

	return S_OK;
}
HRESULT CBody_Player::Render_LightDepth()
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

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_SPHERE;
	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wander"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DisolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDisolveCom);
}
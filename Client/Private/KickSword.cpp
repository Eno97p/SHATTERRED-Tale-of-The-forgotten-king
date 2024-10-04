#include "stdafx.h"
#include "..\Public\KickSword.h"

#include "GameInstance.h"
#include "Player.h"
#include "Andras.h"
#include "EffectManager.h"
#include "UI_Manager.h"
#include "CutSceneCamera.h"

CKickSword::CKickSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CKickSword::CKickSword(const CKickSword& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CKickSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKickSword::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = (PARTOBJ_DESC*)pArg;
	pDesc->fSpeedPerSec = 50.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	list<CGameObject*>& MonsterList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));
	m_pAndras = dynamic_cast<CAndras*>(MonsterList.front());
	Safe_AddRef(m_pAndras);

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->mWorldMatrix._41, pDesc->mWorldMatrix._42, pDesc->mWorldMatrix._43, 1.f));

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook.m128_f32[1] = 0.f;
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vDir = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(vPlayerLook, vDir))));

	m_bIsActive = true;
	m_pGameInstance->Disable_Echo();
	m_pGameInstance->Play_Effect_Sound(TEXT("RushStart.ogg"), SOUND_EFFECT);

	return S_OK;
}

void CKickSword::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 2.f;
		break;
	default:
		break;
	}
	if (m_fDisolveValue < 0.f)
	{
		m_pGameInstance->Erase(this);
	}
}

void CKickSword::Tick(_float fTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vUp = XMVector3Normalize(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vLook = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vUp, -vRight));
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_fDisolveValue == 1.f)
	{
		m_pTransformCom->Go_Up(fTimeDelta);

		// 몬스터 무기와 플레이어 충돌처리
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START || (CUI_Manager::GetInstance()->Get_isQTEAlive() && CUI_Manager::GetInstance()->Check_End_QTE()))
		{
			m_eDisolveType = TYPE_DECREASE;
			fSlowValue = 1.f;
			dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_SlowMo(false);

			if (!CUI_Manager::GetInstance()->Delete_QTE())
			{
				m_pPlayer->PlayerHit(10, true);
			}
			else
			{
				m_pAndras->KickStop();
			}
		}
		else
		{
			_vector vAndrasPos = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Monster"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f);


			_float fLengthFromPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			_float fLengthFromAndras = XMVectorGetX(XMVector3Length(vAndrasPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			_bool StartSlowMo = false;

			if (fLengthFromAndras < 5.1f)
			{
				StartSlowMo = true;
			}

			if (fLengthFromPlayer < 8.f || StartSlowMo) // 
			{
				if (fSlowValue != 0.01f && !StartSlowMo)
				{
					CUI_Manager::GetInstance()->Create_QTE();
				}
				fSlowValue = 0.01f;
				//dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_SlowMo(true);
			}
			else
			{
				fSlowValue = 1.f;
			}
		}
	}
}

void CKickSword::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

HRESULT CKickSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CKickSword::Render_LightDepth()
{

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	//_float4x4		ViewMatrix, ProjMatrix;

	///* 광원 기준의 뷰 변환행렬. */
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 30.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
	//	return E_FAIL;

	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	m_pShaderCom->Begin(1);

	//	m_pModelCom->Render(i);
	//}

	return S_OK;
}

HRESULT CKickSword::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.2f, 1.f, 0.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	wstring meshName = TEXT("Prototype_Component_Model_Weapon_Andras3");

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, meshName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKickSword::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	m_bMotionBlur = true;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &m_bMotionBlur, sizeof(_bool))))
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

CKickSword* CKickSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKickSword* pInstance = new CKickSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CKickSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKickSword::Clone(void* pArg)
{
	CKickSword* pInstance = new CKickSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CKickSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKickSword::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
	Safe_Release(m_pAndras);
}

#include "stdafx.h"
#include "..\Public\RushSword.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"
#include "Particle.h"


CRushSword::CRushSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CRushSword::CRushSword(const CRushSword& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CRushSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRushSword::Initialize(void* pArg)
{
	RUSH_DESC* pDesc = (RUSH_DESC*)pArg;
	pDesc->fSpeedPerSec = 50.f;
	m_fHeight = pDesc->fHeight + 0.5f;
	m_iMeshNum = pDesc->meshNum;
	m_bSound = pDesc->bSound;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix._11, pDesc->mWorldMatrix._12, pDesc->mWorldMatrix._13, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix._21, pDesc->mWorldMatrix._22, pDesc->mWorldMatrix._23, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix._31, pDesc->mWorldMatrix._32, pDesc->mWorldMatrix._33, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->mWorldMatrix._41, pDesc->mWorldMatrix._42, pDesc->mWorldMatrix._43, 1.f));

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook.m128_f32[1] = 0.f;
	EFFECTMGR->Generate_Particle(45, fPos, nullptr, XMVectorZero(), 0.f, vLook);
	pParticle = EFFECTMGR->Generate_Particle(96, fPos, this);
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vDir = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDegree = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(vPlayerLook, vDir))));

	if (pDesc->eRushtype == TYPE_SHOOTINGSTAR)
	{
		int i = RandomInt(1, 2);
		if (i == 1)
		{
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-10.f));
		}
		else
		{
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(10.f));
		}
	}

	m_bIsActive = false;

	return S_OK;
}

void CRushSword::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 0.5f;
		break;
	default:
		break;
	}
	if (m_fDisolveValue < 0.f)
	{
		static_cast<CParticle*>(pParticle)->Set_Delete();
		m_pGameInstance->Erase(this);
	}
}

void CRushSword::Tick(_float fTimeDelta)
{
	m_fShootDelay -= fTimeDelta;
	if (m_fShootDelay < 0.f && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] > m_fHeight)
	{
		Set_Active(true);
		m_pTransformCom->Go_Up(fTimeDelta);
	}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= m_fHeight)
	{

		if (m_eDisolveType != TYPE_DECREASE)
		{
			_float4 pParticlePos;
			XMStoreFloat4(&pParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Particle(97, pParticlePos);
			EFFECTMGR->Generate_Particle(98, pParticlePos);
			EFFECTMGR->Generate_Particle(99, pParticlePos, nullptr, XMVectorSet(1.f,0.f,0.f,0.f), 90.f);

		}
		m_eDisolveType = TYPE_DECREASE;
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_fDisolveValue == 1.f)
	{
		// 몬스터 무기와 플레이어 충돌처리
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{
			m_pPlayer->PlayerHit(15);
		}
	}
}

void CRushSword::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

HRESULT CRushSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_iMeshNum == 15 && i != 0)
		{
			continue;
		}

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

HRESULT CRushSword::Render_LightDepth()
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

void CRushSword::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		if (m_bSound)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("RushStart.ogg"), SOUND_EFFECT);
		}
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

HRESULT CRushSword::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.2f, 1.f, 0.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	wstring meshName = TEXT("");
	switch (m_iMeshNum)
	{
	case 0:
		meshName = TEXT("Prototype_Component_Model_Weapon_Andras");
		break;
	case 1:
		meshName = TEXT("Prototype_Component_Model_Weapon_Andras2");
		break;
	case 2:
		meshName = TEXT("Prototype_Component_Model_Weapon_Andras3");
		break;
	case 3:
		meshName = TEXT("Prototype_Component_Model_Weapon_Andras4");
		break;
	case 4:
		meshName = TEXT("Prototype_Component_Model_Weapon_Andras5");
		break;
	case 5:
		meshName = TEXT("Prototype_Component_Model_Catharsis");
		break;
	case 6:
		meshName = TEXT("Prototype_Component_Model_Cendres");
		break;
	case 7:
		meshName = TEXT("Prototype_Component_Model_CorruptedSword");
		break;
	case 8:
		meshName = TEXT("Prototype_Component_Model_DurgaSword");
		break;
	case 9:
		meshName = TEXT("Prototype_Component_Model_IceBlade");
		break;
	case 10:
		meshName = TEXT("Prototype_Component_Model_PretorianSword");
		break;
	case 11:
		meshName = TEXT("Prototype_Component_Model_RadamantheSword");
		break;
	case 12:
		meshName = TEXT("Prototype_Component_Model_SitraSword");
		break;
	case 13:
		meshName = TEXT("Prototype_Component_Model_ValnirSword");
		break;
	case 14:
		meshName = TEXT("Prototype_Component_Model_VeilleurSword");
		break;
	case 15:
		meshName = TEXT("Prototype_Component_Model_WhisperSword");
		break;
	case 16:
		meshName = TEXT("Prototype_Component_Model_Weapon_Mantari");
		break;
	case 17:
		meshName = TEXT("Prototype_Component_Model_Lughan");
		break;
	case 18:
		// 이칼 돌아가있음
		meshName = TEXT("Prototype_Component_Model_Weapon_Sword_LGGun");
		break;
	}
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

HRESULT CRushSword::Bind_ShaderResources()
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

CRushSword* CRushSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRushSword* pInstance = new CRushSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CRushSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRushSword::Clone(void* pArg)
{
	CRushSword* pInstance = new CRushSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRushSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRushSword::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}

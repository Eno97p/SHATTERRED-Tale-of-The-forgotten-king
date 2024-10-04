#include "stdafx.h"
#include "..\Public\CircleSphere.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"
#include "Monster.h"
#include "SideViewCamera.h"

CCircleSphere::CCircleSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CCircleSphere::CCircleSphere(const CCircleSphere& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CCircleSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCircleSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC gameObjDesc = *(GAMEOBJECT_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameObjDesc.mWorldMatrix._41, gameObjDesc.mWorldMatrix._42, gameObjDesc.mWorldMatrix._43, 1.f)); //초기 위치 설정))

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_fPlayerY = XMVectorGetY(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_pJuggulus = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Boss")).front());

	Particle = EFFECTMGR->Generate_Particle(95, _float4(gameObjDesc.mWorldMatrix._41, gameObjDesc.mWorldMatrix._42, gameObjDesc.mWorldMatrix._43, 1.f), this);
	return S_OK;
}

void CCircleSphere::Priority_Tick(_float fTimeDelta)
{
	if (m_pJuggulus->Get_MonsterDead())
	{
		m_pGameInstance->Erase(this);
		return;
	}

	m_fShootDelay -= fTimeDelta;
}

void CCircleSphere::Tick(_float fTimeDelta)
{
	if (m_pJuggulus->Get_MonsterDead())
	{
		return;
	}

	m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_fShootDelay > 0.f)
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(3.f));	
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
	{
		if (!m_bLookatPlayer)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_Sphere.ogg"), SOUND_MONSTER);
			m_pTransformCom->Set_Speed(40.f);
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_bLookatPlayer = true;
		}
		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < m_fPlayerY)
		{
			_matrix Mat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
			_vector vPos = Mat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			EFFECTMGR->Generate_Particle(14, vStartPos);
			EFFECTMGR->Generate_Particle(15, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
			// 여기서 폭발 이펙트 재생
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_SphereExplode.ogg"), SOUND_EFFECT);
			m_pGameInstance->Erase(this);
			m_pGameInstance->Erase(Particle);

			dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.3f, 0.15f);

		}
	}
}

void CCircleSphere::Late_Tick(_float fTimeDelta)
{
	if (m_pJuggulus->Get_MonsterDead())
	{
		return;
	}

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsParried)
	{
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{

			if (m_pPlayer->Get_Parry())
			{
				EFFECTMGR->Generate_Particle(4, fPos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				EFFECTMGR->Generate_Particle(5, fPos);
				m_bIsParried = true;
				m_pPlayer->Parry_Succeed();
				// 보스 타겟팅
				CTransform* bossTransform = dynamic_cast<CTransform*>(m_pJuggulus->Get_Component(TEXT("Com_Transform")));
				_vector vBossPos = bossTransform->Get_State(CTransform::STATE_POSITION);
				vBossPos.m128_f32[1] += 15.f;
				m_pTransformCom->LookAt(vBossPos);
			}
			else
			{
				m_pPlayer->PlayerHit(10);
				_matrix Mat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
				_vector vPos = Mat.r[3];
				_float4 vStartPos;
				XMStoreFloat4(&vStartPos, vPos);
				EFFECTMGR->Generate_Particle(14, vStartPos);
				EFFECTMGR->Generate_Particle(15, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
				// 여기서 폭발 이펙트 재생
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_SphereExplode.ogg"), SOUND_EFFECT);
				m_pGameInstance->Erase(this);
				m_pGameInstance->Erase(Particle);
				dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.3f, 0.15f);

			}

		}
	}
	else
	{
		if (m_pColliderCom->Intersect(m_pJuggulus->Get_Collider()) == CCollider::COLL_START)
		{
			m_pJuggulus->Add_Hp(-10);
			_matrix Mat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
			_vector vPos = Mat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			EFFECTMGR->Generate_Particle(14, vStartPos);
			EFFECTMGR->Generate_Particle(15, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
			// 여기서 폭발 이펙트 재생
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_SphereExplode.ogg"), SOUND_EFFECT);
			m_pGameInstance->Erase(this);
			m_pGameInstance->Erase(Particle);
			dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.5f, 0.25f);

		}
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CCircleSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(9);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CCircleSphere::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(9);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCircleSphere::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCircleSphere::Bind_ShaderResources()
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
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCircleSphere* CCircleSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCircleSphere* pInstance = new CCircleSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCircleSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCircleSphere::Clone(void* pArg)
{
	CCircleSphere* pInstance = new CCircleSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCircleSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCircleSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
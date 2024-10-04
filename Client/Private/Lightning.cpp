#include "stdafx.h"
#include "..\Public\Lightning.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"
#include "Monster.h"
#include "Particle.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CLightning::CLightning(const CLightning& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CLightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightning::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC gameObjDesc = *(GAMEOBJECT_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameObjDesc.mWorldMatrix._41, gameObjDesc.mWorldMatrix._42, gameObjDesc.mWorldMatrix._43, 1.f)); //초기 위치 설정))

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.3f, 0.3f, 0.3f);

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));


	_float4 vParticlePos;
	XMStoreFloat4(&vParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Particle = EFFECTMGR->Generate_Particle(131, vParticlePos, this);


	return S_OK;
}

void CLightning::Priority_Tick(_float fTimeDelta)
{
	m_fLightningDelay -= fTimeDelta;
}

void CLightning::Tick(_float fTimeDelta)
{
	if (m_fLightningDelay > 0.f)
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPlayerPos), XMVectorGetY(vPos), XMVectorGetZ(vPlayerPos), 1.f));
	}
	else
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y -= 10.f;
		EFFECTMGR->Generate_Lightning(0, vPos);

		_matrix vMatrix = m_pTransformCom->Get_WorldMatrix();
		vMatrix.r[3].m128_f32[1] -= 10.f;
		m_pColliderCom->Tick(vMatrix);

		if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
		{
			m_pPlayer->PlayerHit(10);
		}

		m_pGameInstance->Erase(this);
		static_cast<CParticle*>(Particle)->Set_Target(nullptr);
		static_cast<CParticle*>(Particle)->Set_Delete();
	}

}

void CLightning::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CLightning::Render()
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

HRESULT CLightning::Render_Bloom()
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

HRESULT CLightning::Add_Components()
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

HRESULT CLightning::Bind_ShaderResources()
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

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightning::Clone(void* pArg)
{
	CLightning* pInstance = new CLightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightning::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);


}
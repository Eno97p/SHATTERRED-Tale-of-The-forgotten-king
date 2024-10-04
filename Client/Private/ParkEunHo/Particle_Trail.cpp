#include "stdafx.h"
#include "Particle_Trail.h"
#include "GameInstance.h"

CParticle_Trail::CParticle_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject(pDevice, pContext)
{
}

CParticle_Trail::CParticle_Trail(const CParticle_Trail & rhs)
	: CBlendObject(rhs)
{
}

HRESULT CParticle_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Trail::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	
	m_pTrailDesc = new TRAIL_DESC;
	*m_pTrailDesc = *((TRAIL_DESC*)pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Trail::Priority_Tick(_float fTimeDelta)
{
	
}

void CParticle_Trail::Tick(_float fTimeDelta)
{
	if (m_pVIBufferCom->Check_Instance_Dead())
		m_pGameInstance->Erase(this);

	switch (m_pTrailDesc->eFuncType)
	{
	case TRAIL_EXTINCT:
		m_pVIBufferCom->ExtinctTrail(fTimeDelta);
		break;
	case TRAIL_ETERNAL:
		m_pVIBufferCom->EternalTrail(fTimeDelta);
		break;
	case TRAIL_CATMULROM:
		m_pVIBufferCom->CatMullRomTrail(fTimeDelta);
		break;
	default:
		break;
	}

}
void CParticle_Trail::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_pTrailDesc->Blur)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLUR, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);

	if (m_pTrailDesc->Bloom)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CParticle_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Trail::Render_Bloom()
{
	if (FAILED(Bind_BlurResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Trail::Render_Blur()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CParticle_Trail::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&(m_pTrailDesc->traildesc))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_pTrailDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_DesolveTexture))))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CParticle_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_DesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_pTrailDesc->DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &m_pTrailDesc->Desolve, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &m_pTrailDesc->vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &m_pTrailDesc->vEndColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Trail::Bind_BlurResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_DesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_pTrailDesc->DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &m_pTrailDesc->Desolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomColor", &m_pTrailDesc->vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_pTrailDesc->fBloompower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CParticle_Trail * CParticle_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Trail*		pInstance = new CParticle_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CParticle_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Trail::Clone(void * pArg)
{
	CParticle_Trail*		pInstance = new CParticle_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticle_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_DesolveTexture);
	Safe_Release(m_pShaderCom);
	Safe_Delete(m_pTrailDesc);
}

#include "FireEffect.h"
#include "GameInstance.h"

CFireEffect::CFireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CFireEffect::CFireEffect(const CFireEffect& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CFireEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFireEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_OwnDesc = make_shared<FIREEFFECTDESC>(*((FIREEFFECTDESC*)pArg));

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	m_pTransformCom->Set_Scale(m_OwnDesc->vStartScale.x, m_OwnDesc->vStartScale.y, 0.f);  
	_matrix Mat = m_pTransformCom->Get_WorldMatrix();
	_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffsetPos), Mat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_OwnDesc->mWorldMatrix));

	return S_OK;
}

void CFireEffect::Priority_Tick(_float fTimeDelta)
{
}

void CFireEffect::Tick(_float fTimeDelta)
{
	m_fAcctime += fTimeDelta;
	if (m_fAcctime > 1000.f)
		m_fAcctime = 0.f;

}

void CFireEffect::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_OwnDesc->vStartScale.y))
	{
		Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->BillBoard_Y();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	}
}

HRESULT CFireEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFireEffect::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFireEffect::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Fire"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDiffuse"),
		TEXT("Com_DiffuseTEX"), reinterpret_cast<CComponent**>(&m_pTextureCom[FIRE_DIFFUSE]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireNoise"),
		TEXT("Com_NoiseTEX"), reinterpret_cast<CComponent**>(&m_pTextureCom[FIRE_NOISE]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireAlpha"),
		TEXT("Com_AlphaTEX"), reinterpret_cast<CComponent**>(&m_pTextureCom[FIRE_ALPHA]))))
		return E_FAIL;


	return S_OK;
}

HRESULT CFireEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[FIRE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_FireTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[FIRE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[FIRE_ALPHA]->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fAcctime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeeds", &m_OwnDesc->ScrollSpeeds, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Scales", &m_OwnDesc->Scales, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("distortion1", &m_OwnDesc->distortion1, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("distortion2", &m_OwnDesc->distortion2, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("distortion3", &m_OwnDesc->distortion3, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("distortionScale", &m_OwnDesc->distortionScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("distortionBias", &m_OwnDesc->distortionBias, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}


CFireEffect* CFireEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireEffect* pInstance = new CFireEffect(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFireEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFireEffect::Clone(void* pArg)
{
	CFireEffect* pInstance = new CFireEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFireEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireEffect::Free()
{
	__super::Free();
	for (auto& iter : m_pTextureCom)
	{
		Safe_Release(iter);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}

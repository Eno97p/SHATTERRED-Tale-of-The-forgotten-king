#include "BlackHorizon.h"
#include "GameInstance.h"

CBlackHorizon::CBlackHorizon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CBlackHorizon::CBlackHorizon(const CBlackHorizon& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CBlackHorizon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackHorizon::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_unique<HORIZON_DESC>(*((HORIZON_DESC*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
	m_pTransformCom->Set_Scale(m_OwnDesc->vMinSize.x, m_OwnDesc->vMinSize.y, m_OwnDesc->vMinSize.z);
	return S_OK;
}

void CBlackHorizon::Priority_Tick(_float fTimeDelta)
{

}

void CBlackHorizon::Tick(_float fTimeDelta)
{
	if (Erase)
		return;
	m_AccTime += 0.016f;
	//if (m_AccTime <= m_OwnDesc->fStartdelay && !m_SetDead)
	//{
	//	_float t = m_AccTime / m_OwnDesc->fStartdelay;
	//	t = max(0.f, min(t, 1.f));
	//	_vector Minsize = XMLoadFloat3(&m_OwnDesc->vMinSize);
	//	_vector Maxsize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
	//	_vector ResultSize = XMVectorLerp(Minsize, Maxsize, t);
	//	m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	//}
	//if (m_SetDead)
	//{
	//	if (m_AccTime > m_DeadDelay)
	//		Erase = true;

	//	_float t = m_AccTime / m_DeadDelay;
	//	t = max(0.f, min(t, 1.f));
	//	_vector Maxsize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
	//	_vector Minsize = XMVectorZero();
	//	_vector ResultSize = XMVectorLerp(Maxsize, Minsize, t);
	//	m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	//}

	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
}

void CBlackHorizon::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (Erase)
		return;
	m_pTransformCom->BillBoard();
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CBlackHorizon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBlackHorizon::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}


void CBlackHorizon::Set_BlackHole_Dead()
{
	m_DeadDelay = m_AccTime + m_OwnDesc->fStartdelay;
	m_SetDead = true;
}

HRESULT CBlackHorizon::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GradiantTex"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlackHorizon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;
	return S_OK;
}

HRESULT CBlackHorizon::Bind_BloomResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;
	return S_OK;
}

CBlackHorizon* CBlackHorizon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlackHorizon* pInstance = new CBlackHorizon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlackHorizon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackHorizon::Clone(void* pArg)
{
	CBlackHorizon* pInstance = new CBlackHorizon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlackHorizon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlackHorizon::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

}

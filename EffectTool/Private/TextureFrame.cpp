#include "TextureFrame.h"
#include "GameInstance.h"

CTextureFrame::CTextureFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CTextureFrame::CTextureFrame(const CTextureFrame& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CTextureFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextureFrame::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	TEXFRAMEDESC* pDesc = reinterpret_cast<TEXFRAMEDESC*>(pArg);
	m_OwnDesc = make_shared<TEXFRAMEDESC>(*pDesc);

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, 0.f);
	return S_OK;
}

void CTextureFrame::Priority_Tick(_float fTimeDelta)
{
}

void CTextureFrame::Tick(_float fTimeDelta)
{
	m_fCurFrame += fTimeDelta * m_OwnDesc->Frame[TX_FRAMEFLOAT::FRAMESPEED];
	if (m_fCurFrame > m_OwnDesc->Frame[TX_FRAMEFLOAT::MAXFRAME])
	{
		m_fCurFrame = m_OwnDesc->Frame[TX_FRAMEFLOAT::MAXFRAME];
		m_pGameInstance->Erase(this);
	}

	m_fFrameRatio = m_fCurFrame / m_OwnDesc->Frame[TX_FRAMEFLOAT::MAXFRAME];


}

void CTextureFrame::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_OwnDesc->State[TX_BILL] == true)
	{
		m_pTransformCom->BillBoard();
		m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, 0.f);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	if (m_OwnDesc->State[TXFRAMESTATE::TX_BLUR])
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	}
}

HRESULT CTextureFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTextureFrame::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTextureFrame::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FrameTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevel(), m_OwnDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Desolve */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_pDesolveTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextureFrame::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->iNumDesolve)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Division", &m_OwnDesc->iDivision, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &m_OwnDesc->State[TXFRAMESTATE::TX_DESOLVE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsColor", &m_OwnDesc->State[TXFRAMESTATE::TX_ISCOLOR], sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameRatio", &m_fFrameRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("vStartClolor", &m_OwnDesc->Color[0], sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("vEndColor", &m_OwnDesc->Color[1], sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextureFrame::Bind_BlurResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BlurPower", &m_OwnDesc->Frame[TX_FRAMEFLOAT::BLURPOWER], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("vBloomColor", &m_OwnDesc->Color[2], sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &m_OwnDesc->State[TXFRAMESTATE::TX_DESOLVE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->iNumDesolve)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameRatio", &m_fFrameRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTextureFrame* CTextureFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTextureFrame* pInstance = new CTextureFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTextureFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTextureFrame::Clone(void* pArg)
{
	CTextureFrame* pInstance = new CTextureFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTextureFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTextureFrame::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDesolveTexture);
}

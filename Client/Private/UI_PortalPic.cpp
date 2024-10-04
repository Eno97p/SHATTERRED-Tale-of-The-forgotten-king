#include "UI_PortalPic.h"

#include "GameInstance.h"

CUI_PortalPic::CUI_PortalPic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_PortalPic::CUI_PortalPic(const CUI_PortalPic& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_PortalPic::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_PortalPic::Initialize(void* pArg)
{
	UI_PORTALPIC_DESC* pDesc = static_cast<UI_PORTALPIC_DESC*>(pArg);

	m_iPicNum = pDesc->iPicNum;
	m_fScale = pDesc->fScale;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->fAngle);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	return S_OK;
}

void CUI_PortalPic::Priority_Tick(_float fTimeDelta)
{
}

void CUI_PortalPic::Tick(_float fTimeDelta)
{
	m_fOpacityTimer += fTimeDelta;

	if (!m_isFadeIn)
	{
		m_fDisolveValue += fTimeDelta;

		if (m_fDisolveValue >= 1.f)
		{
			m_isFadeEnd = true;
		}
	}
}

void CUI_PortalPic::Late_Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_Scale(m_fScale.x, m_fScale.y, 4.5f); // 3.7f, 6.f, 4.5f

	CGameInstance::GetInstance()->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	CGameInstance::GetInstance()->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CUI_PortalPic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(14); // 0
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_PortalPic::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(15); // 0
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_PortalPic::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_OpacityTexture"), reinterpret_cast<CComponent**>(&m_pOpacityTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic_Dissolve"),
		TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PortalPic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iPicNum)))
		return E_FAIL;
	if (FAILED(m_pOpacityTextureCom->Bind_ShaderResource(m_pShaderCom, "g_OpacityTex", 2)))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowTime", &m_fOpacityTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isFadeIn, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_PortalPic* CUI_PortalPic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PortalPic* pInstance = new CUI_PortalPic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_PortalPic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PortalPic::Clone(void* pArg)
{
	CUI_PortalPic* pInstance = new CUI_PortalPic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_PortalPic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PortalPic::Free()
{
	__super::Free();

	Safe_Release(m_pOpacityTextureCom);
	Safe_Release(m_pDisolveTextureCom);
}

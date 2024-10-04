#include "UI_UpGPageBtn_Select.h"

#include "GameInstance.h"

CUI_UpGPageBtn_Select::CUI_UpGPageBtn_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpGPageBtn_Select::CUI_UpGPageBtn_Select(const CUI_UpGPageBtn_Select& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpGPageBtn_Select::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPageBtn_Select::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX - 310.f;
	m_fY = (g_iWinSizeY >> 1) + 100.f;
	m_fSizeX = 512.f; // 1024
	m_fSizeY = 64.f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_UpGPageBtn_Select::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPageBtn_Select::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpGPageBtn_Select::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_UpGPageBtn_Select::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("UPGRADE"), _float2(m_fX - 90.f, m_fY - 13.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPageBtn_Select::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_BtnSelect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPageBtn_Select::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_UpGPageBtn_Select* CUI_UpGPageBtn_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPageBtn_Select* pInstance = new CUI_UpGPageBtn_Select(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPageBtn_Select");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPageBtn_Select::Clone(void* pArg)
{
	CUI_UpGPageBtn_Select* pInstance = new CUI_UpGPageBtn_Select(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPageBtn_Select");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPageBtn_Select::Free()
{
	__super::Free();
}

#include "UI_Script_DialogBox.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Script_DialogBox::CUI_Script_DialogBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Script_DialogBox::CUI_Script_DialogBox(const CUI_Script_DialogBox& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Script_DialogBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Script_DialogBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_Script_DialogBox::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Script_DialogBox::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_Script_DialogBox::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTH);
}

HRESULT CUI_Script_DialogBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight15"), m_wstrDialogText, _float2(270.f, (g_iWinSizeY >> 1) + 200.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Script_DialogBox::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_DialogBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Script_DialogBox::Bind_ShaderResources()
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

void CUI_Script_DialogBox::Setting_Text(wstring wstrDialogText)
{
	m_wstrDialogText = wstrDialogText;
}

void CUI_Script_DialogBox::Setting_TextXY(_float fX, _float fY)
{
	// 필요 없을 거 같음
	m_fTextX = fX;
	m_fTextY = fY; 
}

CUI_Script_DialogBox* CUI_Script_DialogBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Script_DialogBox* pInstance = new CUI_Script_DialogBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Script_DialogBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Script_DialogBox::Clone(void* pArg)
{
	CUI_Script_DialogBox* pInstance = new CUI_Script_DialogBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Script_DialogBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Script_DialogBox::Free()
{
	__super::Free();
}

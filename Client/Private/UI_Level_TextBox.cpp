#include "UI_Level_TextBox.h"

#include "GameInstance.h"

CUI_Level_TextBox::CUI_Level_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Level_TextBox::CUI_Level_TextBox(const CUI_Level_TextBox& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Level_TextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Level_TextBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) - 150.f;
	m_fSizeX = 800.f;
	m_fSizeY = 800.f;

	Setting_Position();

	Resset_Animation(true);

	Setting_Text();

	return S_OK;
}

void CUI_Level_TextBox::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Level_TextBox::Tick(_float fTimeDelta)
{
	m_fMoveTimer += fTimeDelta;

	if (1.f <= m_fMoveTimer)
	{
		m_fX -= 1.5f;

		Setting_Position();

		Render_Animation(fTimeDelta, 0.8f);

		m_fColor -= 0.01f;
		if (m_fColor < 0.f)
			m_fColor = 0.f;

		if (m_isRenderAnimFinished)
		{
			m_isDead = true;
		}
	}
}

void CUI_Level_TextBox::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_Level_TextBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo23"), Setting_Text(), _float2(m_fX - m_fFontX, m_fY - m_fFontY), XMVectorSet(m_fColor, m_fColor, m_fColor, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Level_TextBox::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Level_TextBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Level_TextBox::Bind_ShaderResources()
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

wstring CUI_Level_TextBox::Setting_Text()
{
	LEVEL eCurrentLevel = static_cast<LEVEL>(CGameInstance::GetInstance()->Get_CurrentLevel());

	m_fFontY = 65.f;

	switch (eCurrentLevel)
	{

	case Client::LEVEL_GAMEPLAY:
		m_fFontX = 140.f;
		return TEXT("START OF ABSYSS");
	case Client::LEVEL_ACKBAR:
		m_fFontX = 150.f;
		return TEXT("VISION OF ACKBAR");
	case Client::LEVEL_JUGGLAS:
		m_fFontX = 160.f;
		return TEXT("SHAMRA'S GROTTO");
	case Client::LEVEL_ANDRASARENA:
		m_fFontX = 130.f;
		return TEXT("LIMBO PLAINS");
	case Client::LEVEL_GRASSLAND:
		m_fFontX = 220.f;
		return TEXT("WILDERNESS OF THE KING");
	default:
		return TEXT("");
	}
}

CUI_Level_TextBox* CUI_Level_TextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Level_TextBox* pInstance = new CUI_Level_TextBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Level_TextBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Level_TextBox::Clone(void* pArg)
{
	CUI_Level_TextBox* pInstance = new CUI_Level_TextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Level_TextBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Level_TextBox::Free()
{
	__super::Free();
}

#include "UI_BossTextBox.h"

#include "GameInstance.h"

CUI_BossTextBox::CUI_BossTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BossTextBox::CUI_BossTextBox(const CUI_BossTextBox& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_BossTextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossTextBox::Initialize(void* pArg)
{
	UI_BOSSTEXTBOX_DESC* pDesc = static_cast<UI_BOSSTEXTBOX_DESC*>(pArg);

	m_isCreateText = pDesc->isCreateText;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) + 30.f;
	m_fSizeX = 1200.f;
	m_fSizeY = 1600.f;

	Setting_Position();

	Resset_Animation(true);

	Setting_Text();

	return S_OK;
}

void CUI_BossTextBox::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BossTextBox::Tick(_float fTimeDelta)
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

void CUI_BossTextBox::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_BossTextBox::Render()
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

HRESULT CUI_BossTextBox::Add_Components()
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

HRESULT CUI_BossTextBox::Bind_ShaderResources()
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

wstring CUI_BossTextBox::Setting_Text()
{
	LEVEL eCurrentLevel = static_cast<LEVEL>(CGameInstance::GetInstance()->Get_CurrentLevel());

	m_fFontY = 110.f;
	
	switch (eCurrentLevel)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		if (m_isCreateText)
		{
			m_fFontX = 280.f;
			return TEXT("MANTARI, THE WANDERING SPECTER");
		}
		else
		{
			m_fFontX = 140.f;
			return TEXT("SPECTER SLAIN");
		}
	}
	case Client::LEVEL_ACKBAR:
	{
		if (m_isCreateText)
		{
			m_fFontX = 260.f;
			return TEXT("KETHER, APOCRYPHAL WATCHER");
		}
		else
		{
			m_fFontX = 140.f;
			return TEXT("WATCHER SLAIN");
		}
	}
	case Client::LEVEL_JUGGLAS:
	{
		if (m_isCreateText)
		{
			m_fFontX = 230.f;
			return TEXT("RAS SAMRAH, GUILTY ONE");
		}
		else
		{
			m_fFontX = 150.f;
			return TEXT("GUILTY ONE SLAIN");
		}
	}
	case Client::LEVEL_ANDRASARENA:
	{
		if (m_isCreateText)
		{
			m_fFontX = 240.f;
			return TEXT("ANDRAS, THE FORSAKEN KING");
		}
		else
		{
			m_fFontX = 140.f;
			return TEXT("KING SLAIN");
		}
	}
	case Client::LEVEL_GRASSLAND:
	{
		if (m_isCreateText)
		{
			m_fFontX = 240.f;
			return TEXT("MALKHEL, JUDGE AND KING");
		}
		else
		{
			m_fFontX = 140.f;
			return TEXT("JUDGE SLAIN");
		}
	}
	default:
		return TEXT("");
	}
}

CUI_BossTextBox* CUI_BossTextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossTextBox* pInstance = new CUI_BossTextBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BossTextBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossTextBox::Clone(void* pArg)
{
	CUI_BossTextBox* pInstance = new CUI_BossTextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BossTextBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossTextBox::Free()
{
	__super::Free();
}

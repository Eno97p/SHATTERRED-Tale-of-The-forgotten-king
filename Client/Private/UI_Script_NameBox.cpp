#include "UI_Script_NameBox.h"

#include "GameInstance.h"

CUI_Script_NameBox::CUI_Script_NameBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Script_NameBox::CUI_Script_NameBox(const CUI_Script_NameBox& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Script_NameBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Script_NameBox::Initialize(void* pArg)
{
	UI_SCRIPT_NAMEBOX_DESC* pDesc = static_cast<UI_SCRIPT_NAMEBOX_DESC*>(pArg);

	m_eNpcType = pDesc->eNpcType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 260.f;
	m_fY = (g_iWinSizeY >> 1) + 150.f;
	m_fSizeX = 256.f;
	m_fSizeY = 256.f;

	Setting_Position();

	return S_OK;
}

void CUI_Script_NameBox::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Script_NameBox::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_Script_NameBox::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SEVENTH);
}

HRESULT CUI_Script_NameBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), Setting_Text(), _float2(m_fX - 43.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Script_NameBox::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_NameBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Script_NameBox::Bind_ShaderResources()
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

_tchar* CUI_Script_NameBox::Setting_Text()
{
	switch (m_eNpcType)
	{
	case Client::CUI_Script_NameBox::NPC_RLYA:
		return TEXT("RLYA");
	case Client::CUI_Script_NameBox::NPC_VALNIR:
		return TEXT("VALNIR");
	case Client::CUI_Script_NameBox::NPC_CHORON:
		return TEXT("CHORON");
	case Client::CUI_Script_NameBox::NPC_YAAK:
		return TEXT("YAAK");
	default:
		return TEXT("");
	}
}

CUI_Script_NameBox* CUI_Script_NameBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Script_NameBox* pInstance = new CUI_Script_NameBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Script_NameBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Script_NameBox::Clone(void* pArg)
{
	CUI_Script_NameBox* pInstance = new CUI_Script_NameBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Script_NameBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Script_NameBox::Free()
{
	__super::Free();
}

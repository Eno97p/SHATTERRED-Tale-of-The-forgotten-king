#include "UI_ScriptBG_Npc.h"

#include "GameInstance.h"

CUI_ScriptBG_Npc::CUI_ScriptBG_Npc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ScriptBG_Npc::CUI_ScriptBG_Npc(const CUI_ScriptBG_Npc& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ScriptBG_Npc::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ScriptBG_Npc::Initialize(void* pArg)
{
	UI_SCRIPT_DESC* pDesc = static_cast<UI_SCRIPT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->wstrTextureName)))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_ScriptBG_Npc::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ScriptBG_Npc::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_ScriptBG_Npc::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIFTH);
}

HRESULT CUI_ScriptBG_Npc::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(8);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ScriptBG_Npc::Add_Components(wstring wstrTextureName)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrTextureName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ScriptBG_Npc::Bind_ShaderResources()
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

CUI_ScriptBG_Npc* CUI_ScriptBG_Npc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ScriptBG_Npc* pInstance = new CUI_ScriptBG_Npc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_ScriptBG_Npc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_ScriptBG_Npc::Clone(void* pArg)
{
	CUI_ScriptBG_Npc* pInstance = new CUI_ScriptBG_Npc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_ScriptBG_Npc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ScriptBG_Npc::Free()
{
	__super::Free();
}

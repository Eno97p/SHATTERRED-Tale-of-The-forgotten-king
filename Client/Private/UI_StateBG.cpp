#include "UI_StateBG.h"

#include "GameInstance.h"

CUI_StateBG::CUI_StateBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_StateBG::CUI_StateBG(const CUI_StateBG& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_StateBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_StateBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 260.f; 
	m_fY = 60.f; 
	m_fSizeX = 768.f; // 1024
	m_fSizeY = 96.f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_StateBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateBG::Tick(_float fTimeDelta)
{
}

void CUI_StateBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_StateBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_StateBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_StateBG* CUI_StateBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateBG* pInstance = new CUI_StateBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateBG::Clone(void* pArg)
{
	CUI_StateBG* pInstance = new CUI_StateBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateBG::Free()
{
	__super::Free();
}

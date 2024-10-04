#include "UI_Shop_SoulBG.h"

#include "GameInstance.h"

#include "UI_StateSoul.h"

CUI_Shop_SoulBG::CUI_Shop_SoulBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Shop_SoulBG::CUI_Shop_SoulBG(const CUI_Shop_SoulBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Shop_SoulBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Shop_SoulBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 490.f;
	m_fY = (g_iWinSizeY >> 1) + 70.f;
	m_fSizeX = 350.f; // 1024
	m_fSizeY = 40.f; // 512

	Setting_Position();

	if (FAILED(Create_Soul()))
		return E_FAIL;

	return S_OK;
}

void CUI_Shop_SoulBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Shop_SoulBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
	{
		Render_Animation(fTimeDelta);
	}

	m_pSoulIcon->Tick(fTimeDelta);
}

void CUI_Shop_SoulBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTH);

	m_pSoulIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_Shop_SoulBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Shop_SoulBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SoulBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop_SoulBG::Bind_ShaderResources()
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

HRESULT CUI_Shop_SoulBG::Create_Soul()
{
	CUI_StateSoul::UI_SOUL_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX + 30.f;
	pDesc.fY = m_fY - 2.f;
	pDesc.fSizeX = 32.f;
	pDesc.fSizeY = 32.f;
	pDesc.eUISort = SEVENTH;

	m_pSoulIcon = dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pDesc));

	return S_OK;
}

CUI_Shop_SoulBG* CUI_Shop_SoulBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shop_SoulBG* pInstance = new CUI_Shop_SoulBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Shop_SoulBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Shop_SoulBG::Clone(void* pArg)
{
	CUI_Shop_SoulBG* pInstance = new CUI_Shop_SoulBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Shop_SoulBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shop_SoulBG::Free()
{
	__super::Free();

	Safe_Release(m_pSoulIcon);
}

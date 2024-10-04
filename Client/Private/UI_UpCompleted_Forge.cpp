#include "UI_UpCompleted_Forge.h"

#include "GameInstance.h"

#include "UI_ItemIcon.h"

CUI_UpCompleted_Forge::CUI_UpCompleted_Forge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpCompleted_Forge::CUI_UpCompleted_Forge(const CUI_UpCompleted_Forge& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpCompleted_Forge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpCompleted_Forge::Initialize(void* pArg)
{
	UI_UPCOM_FORGE_DESC* pDesc = static_cast<UI_UPCOM_FORGE_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	if (FAILED(Create_ItemIcon(pDesc->wstrTextureName)))
		return E_FAIL;

	return S_OK;
}

void CUI_UpCompleted_Forge::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpCompleted_Forge::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	// ItemIcon을 Texture Change 해줄 것

	m_pItemIcon->Tick(fTimeDelta);
}

void CUI_UpCompleted_Forge::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTEENTH);

	m_pItemIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_UpCompleted_Forge::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_UpCompleted_Forge::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Forge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpCompleted_Forge::Bind_ShaderResources()
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

HRESULT CUI_UpCompleted_Forge::Create_ItemIcon(wstring wstrTextureName)
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX - 30.f;
	pDesc.fY = m_fY - 90.f;
	pDesc.fSizeX = 300.f;
	pDesc.fSizeY = 300.f;
	pDesc.eUISort = FIFTEENTH;
	pDesc.wszTexture = wstrTextureName; // None   Prototype_Component_Texture_ItemIcon_None
	
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

CUI_UpCompleted_Forge* CUI_UpCompleted_Forge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpCompleted_Forge* pInstance = new CUI_UpCompleted_Forge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpCompleted_Forge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpCompleted_Forge::Clone(void* pArg)
{
	CUI_UpCompleted_Forge* pInstance = new CUI_UpCompleted_Forge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpCompleted_Forge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpCompleted_Forge::Free()
{
	__super::Free();

	Safe_Release(m_pItemIcon);
}

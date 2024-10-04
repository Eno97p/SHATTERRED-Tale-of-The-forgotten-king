#include "UI_MapIcon.h"

#include "GameInstance.h"

CUI_MapIcon::CUI_MapIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_MapIcon::CUI_MapIcon(const CUI_MapIcon& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_MapIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MapIcon::Initialize(void* pArg)
{
	UI_MAPICON_DESC* pDesc = static_cast<UI_MAPICON_DESC*>(pArg);

	m_iIndex = pDesc->iIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_MapIcon::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MapIcon::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_MapIcon::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_MapIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_MapIcon::Update_Pos(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;
	Setting_Position();
}

HRESULT CUI_MapIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_MapIcon* CUI_MapIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MapIcon* pInstance = new CUI_MapIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MapIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MapIcon::Clone(void* pArg)
{
	CUI_MapIcon* pInstance = new CUI_MapIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MapIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MapIcon::Free()
{
	__super::Free();
}

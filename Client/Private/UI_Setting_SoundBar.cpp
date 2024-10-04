#include "UI_Setting_SoundBar.h"

#include "GameInstance.h"

CUI_Setting_SoundBar::CUI_Setting_SoundBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Setting_SoundBar::CUI_Setting_SoundBar(const CUI_Setting_SoundBar& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Setting_SoundBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Setting_SoundBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_fX = (g_iWinSizeX >> 1);
	//m_fY = (g_iWinSizeY >> 1 ) + 100.f;
	//m_fSizeX = 800.f; // 800
	//m_fSizeY = 31.f; // 31

	Setting_Position();

	return S_OK;
}

void CUI_Setting_SoundBar::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_SoundBar::Tick(_float fTimeDelta)
{
}

void CUI_Setting_SoundBar::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_Setting_SoundBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Setting_SoundBar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_SoundBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_SoundBar::Bind_ShaderResources()
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

CUI_Setting_SoundBar* CUI_Setting_SoundBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Setting_SoundBar* pInstance = new CUI_Setting_SoundBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Setting_SoundBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Setting_SoundBar::Clone(void* pArg)
{
	CUI_Setting_SoundBar* pInstance = new CUI_Setting_SoundBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Setting_SoundBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Setting_SoundBar::Free()
{
	__super::Free();
}

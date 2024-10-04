#include "UI_BuffTimer_Bar.h"

#include "GameInstance.h"

CUI_BuffTimer_Bar::CUI_BuffTimer_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_BuffTimer_Bar::CUI_BuffTimer_Bar(const CUI_BuffTimer_Bar& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_BuffTimer_Bar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_BuffTimer_Bar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 80.f;
	m_fY = (g_iWinSizeY >> 1) + 130.f;
	m_fSizeX = 70.f; // 512
	m_fSizeY = 70.f;

	Setting_Position();

	return S_OK;
}

void CUI_BuffTimer_Bar::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BuffTimer_Bar::Tick(_float fTimeDelta)
{
}

void CUI_BuffTimer_Bar::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_BuffTimer_Bar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_BuffTimer_Bar::Update_Position(_float fX)
{
	m_fX = fX - 1.f;
	Setting_Position();
}

HRESULT CUI_BuffTimer_Bar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BuffTimer_Bar::Bind_ShaderResources()
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

CUI_BuffTimer_Bar* CUI_BuffTimer_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BuffTimer_Bar* pInstance = new CUI_BuffTimer_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BuffTimer_Bar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BuffTimer_Bar::Clone(void* pArg)
{
	CUI_BuffTimer_Bar* pInstance = new CUI_BuffTimer_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BuffTimer_Bar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BuffTimer_Bar::Free()
{
	__super::Free();
}

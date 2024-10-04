#include "UI_HUDEffect.h"

#include "GameInstance.h"

CUI_HUDEffect::CUI_HUDEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_HUDEffect::CUI_HUDEffect(const CUI_HUDEffect& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_HUDEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HUDEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_fX = 140.f;
	//m_fY = (g_iWinSizeX >> 1); +200.f;
	//m_fSizeX = 256.f; // 256
	//m_fSizeY = 256.f;

	Setting_Position();

	return S_OK;
}

void CUI_HUDEffect::Priority_Tick(_float fTimeDelta)
{
}

void CUI_HUDEffect::Tick(_float fTimeDelta)
{
	// 팍 나타났다가 서서히 사라지게? or 팍 사라지게?
	m_fDeadTimer += fTimeDelta;
	if (0.5f <= m_fDeadTimer)
		m_isDead = true;
}

void CUI_HUDEffect::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_HUDEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_HUDEffect::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HUDEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HUDEffect::Bind_ShaderResources()
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

CUI_HUDEffect* CUI_HUDEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HUDEffect* pInstance = new CUI_HUDEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_HUDEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_HUDEffect::Clone(void* pArg)
{
	CUI_HUDEffect* pInstance = new CUI_HUDEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_HUDEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HUDEffect::Free()
{
	__super::Free();
}

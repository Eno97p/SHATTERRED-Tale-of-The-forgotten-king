#include "UI_BuffTimer_Timer.h"

#include "GameInstance.h"

CUI_BuffTimer_Timer::CUI_BuffTimer_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BuffTimer_Timer::CUI_BuffTimer_Timer(const CUI_BuffTimer_Timer& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_BuffTimer_Timer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BuffTimer_Timer::Initialize(void* pArg)
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

void CUI_BuffTimer_Timer::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BuffTimer_Timer::Tick(_float fTimeDelta)
{
	m_fBuffTimer += fTimeDelta;

	m_fCurrentRatio = m_fBuffTimer / 60.f;
}

void CUI_BuffTimer_Timer::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_BuffTimer_Timer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(12);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_BuffTimer_Timer::Update_Position(_float fX)
{
	m_fX = fX;
	Setting_Position();
}

HRESULT CUI_BuffTimer_Timer::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BuffTimer"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BuffTimer_Timer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_CurrentRatio"), &m_fCurrentRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_HudRatio"), &m_fHudRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_BuffTimer_Timer* CUI_BuffTimer_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BuffTimer_Timer* pInstance = new CUI_BuffTimer_Timer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BuffTimer_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BuffTimer_Timer::Clone(void* pArg)
{
	CUI_BuffTimer_Timer* pInstance = new CUI_BuffTimer_Timer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BuffTimer_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BuffTimer_Timer::Free()
{
	__super::Free();
}

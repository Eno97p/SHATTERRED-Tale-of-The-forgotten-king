#include "UI_WeaponRTop.h"

#include "GameInstance.h"

CUI_WeaponRTop::CUI_WeaponRTop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_WeaponRTop::CUI_WeaponRTop(const CUI_WeaponRTop& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_WeaponRTop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WeaponRTop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX - 380.f;
	m_fY = 100.f;
	m_fSizeX = 341.3f; // 512
	m_fSizeY = 42.f; // 64

	Setting_Position();

	return S_OK;
}

void CUI_WeaponRTop::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WeaponRTop::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_WeaponRTop::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_WeaponRTop::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WeaponRTop::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponRTop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WeaponRTop::Bind_ShaderResources()
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

CUI_WeaponRTop* CUI_WeaponRTop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WeaponRTop* pInstance = new CUI_WeaponRTop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WeaponRTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponRTop::Clone(void* pArg)
{
	CUI_WeaponRTop* pInstance = new CUI_WeaponRTop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WeaponRTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponRTop::Free()
{
	__super::Free();
}

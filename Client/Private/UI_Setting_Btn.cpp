#include "UI_Setting_Btn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

CUI_Setting_Btn::CUI_Setting_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_Setting_Btn::CUI_Setting_Btn(const CUI_Setting_Btn& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_Setting_Btn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Setting_Btn::Initialize(void* pArg)
{
	UI_SETTINGBTN_DESC* pDesc = static_cast<UI_SETTINGBTN_DESC*>(pArg);

	Setting_BtnData(pDesc->iBtnIdx);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_Setting_Btn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_Btn::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.15f), LONG(m_fY - m_fSizeY * 0.15f),
						LONG(m_fX + m_fSizeX * 0.15f) ,LONG(m_fY + m_fSizeY * 0.15f) };

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_isOn = !m_isOn;
	}
}

void CUI_Setting_Btn::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_Setting_Btn::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Rend_Font();

	return S_OK;
}

HRESULT CUI_Setting_Btn::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_OnOffBtn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Btn::Bind_ShaderResources()
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

void CUI_Setting_Btn::Rend_Font()
{
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), wstrBtnText, _float2(m_fX - 220.f, m_fY - 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (m_isOn)
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("ON"), _float2(m_fX - 22.f, m_fY - 15.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return;
	}
	else
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("OFF"), _float2(m_fX - 25.f, m_fY - 15.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return;
	}
}

void CUI_Setting_Btn::Setting_BtnData(_uint iBtnIdx)
{
	switch (iBtnIdx)
	{
	case 0:
		wstrBtnText = TEXT("TEST 1");
		m_eBtnType = BTN_TEST1;
		break;
	case 1:
		wstrBtnText = TEXT("TEST 2");
		m_eBtnType = BTN_TEST2;
		break;
	case 2:
		wstrBtnText = TEXT("TEST 3");
		m_eBtnType = BTN_TEST3;
		break;
	case 3:
		wstrBtnText = TEXT("TEST 4");
		m_eBtnType = BTN_TEST4;
		break;
	default:
		break;
	}
}

CUI_Setting_Btn* CUI_Setting_Btn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Setting_Btn* pInstance = new CUI_Setting_Btn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Setting_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Setting_Btn::Clone(void* pArg)
{
	CUI_Setting_Btn* pInstance = new CUI_Setting_Btn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Setting_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Setting_Btn::Free()
{
	__super::Free();
}

#include "UI_UpGPageBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

#include "UI_UpGPageBtn_Select.h"
#include "UIGroup_UpGPage.h"

CUI_UpGPageBtn::CUI_UpGPageBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_UpGPageBtn::CUI_UpGPageBtn(const CUI_UpGPageBtn& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_UpGPageBtn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPageBtn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX - 400.f;
	m_fY = (g_iWinSizeY >> 1) + 100.f;
	m_fSizeX = 1024.f; // 2048
	m_fSizeY = 64.f; // 128

	Setting_Position();

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUI_UpGPageBtn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPageBtn::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.05f),
					LONG(m_fY - m_fSizeY * 0.3f),
					LONG(m_fX + m_fSizeX * 0.3f),
					LONG(m_fY + m_fSizeY * 0.3f) };

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_pGameInstance->Mouse_Down(DIM_LB) && m_isSelect) // 클릭한 경우 강화 화면으로 넘어가기
	{
		dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Create_CompletedPage();
	}

	if(nullptr != m_pSelectUI)
		m_pSelectUI->Tick(fTimeDelta);
}

void CUI_UpGPageBtn::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, EIGHT);

	if (m_isSelect && nullptr != m_pSelectUI)
	{
		m_pSelectUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUI_UpGPageBtn::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("UPGRADE"), _float2(m_fX, m_fY - 13.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPageBtn::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Btn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPageBtn::Bind_ShaderResources()
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

HRESULT CUI_UpGPageBtn::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	m_pSelectUI = dynamic_cast<CUI_UpGPageBtn_Select*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageBtn_Select"), &pDesc));

	return S_OK;
}

CUI_UpGPageBtn* CUI_UpGPageBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPageBtn* pInstance = new CUI_UpGPageBtn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPageBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPageBtn::Clone(void* pArg)
{
	CUI_UpGPageBtn* pInstance = new CUI_UpGPageBtn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPageBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPageBtn::Free()
{
	__super::Free();

	Safe_Release(m_pSelectUI);
}

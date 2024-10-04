#include "UI_MapArea.h"

#include "GameInstance.h"

CUI_MapArea::CUI_MapArea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_MapArea::CUI_MapArea(const CUI_MapArea& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_MapArea::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MapArea::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX - 280.f;
	m_fY = (g_iWinSizeY >> 1 ) - 70.f;
	m_fSizeX = 400.; // 1024
	m_fSizeY = 400.f;

	Setting_Position();

	return S_OK;
}

void CUI_MapArea::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MapArea::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_MapArea::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_MapArea::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
	
	m_wstrArea = TEXT("MYOSIS");
	// 추후 플레이어가 위치한 LEVEL의 정보를 출력할 것

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrArea, _float2(m_fX - 50.f, m_fY - 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapArea::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Area"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapArea::Bind_ShaderResources()
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

CUI_MapArea* CUI_MapArea::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MapArea* pInstance = new CUI_MapArea(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MapArea");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MapArea::Clone(void* pArg)
{
	CUI_MapArea* pInstance = new CUI_MapArea(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MapArea");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MapArea::Free()
{
	__super::Free();
}

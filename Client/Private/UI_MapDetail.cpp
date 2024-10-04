#include "UI_MapDetail.h"

#include "GameInstance.h"
#include "UI_MapIcon.h"

CUI_MapDetail::CUI_MapDetail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_MapDetail::CUI_MapDetail(const CUI_MapDetail& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_MapDetail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MapDetail::Initialize(void* pArg)
{
	UI_MAPDETAIL_DESC* pDesc = static_cast<UI_MAPDETAIL_DESC*>(pArg);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_AreaData(pDesc->iIdx);
	Setting_Position();

	Create_MapIcon(pDesc->iIdx);

	return S_OK;
}

void CUI_MapDetail::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MapDetail::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	if (nullptr != m_pMapIcon)
		m_pMapIcon->Tick(fTimeDelta);
}

void CUI_MapDetail::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);

	if (nullptr != m_pMapIcon)
		m_pMapIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_MapDetail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), m_wstrDetail, _float2(m_fX - 60.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapDetail::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Detail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapDetail::Bind_ShaderResources()
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

void CUI_MapDetail::Setting_AreaData(_uint iIdx)
{
	switch (iIdx)
	{
	case 0: // Mantari
		m_fX = 300.f;
		m_fY = 260.f;
		m_wstrDetail = TEXT("Absyss");
		break;
	case 1: // Ackbar
		m_fX = 500.f;
		m_fY = (g_iWinSizeY >> 1) + 30.f;
		m_wstrDetail = TEXT("Ackbar");
		break;
	case 2: // Juggulus Grotto
		m_fX = (g_iWinSizeX >> 1) - 50.f;
		m_fY = 100.f;
		m_wstrDetail = TEXT("Shamra's Grotto");
		break;
	case 3: // Plains
		m_fX = (g_iWinSizeX >> 1) + 50.f;
		m_fY = g_iWinSizeY - 170.f;
		m_wstrDetail = TEXT("Limbo Plains");
		break;
	case 4: // Andrass Wilderness
		m_fX = (g_iWinSizeX >> 1) + 200.f;
		m_fY = 220.f;
		m_wstrDetail = TEXT("Wilderness of the King");
		break;
	default:
		break;
	}

	m_fSizeX = 250.f; // 512
	m_fSizeY = 250.f;
}

void CUI_MapDetail::Create_MapIcon(_uint iIdx)
{
	CUI_MapIcon::UI_MAPICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX - 95.f;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f; // 256
	pDesc.fSizeY = 64.f;
	pDesc.iIndex = iIdx;

	m_pMapIcon = dynamic_cast<CUI_MapIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapIcon"), &pDesc));
}

CUI_MapDetail* CUI_MapDetail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MapDetail* pInstance = new CUI_MapDetail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MapDetail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MapDetail::Clone(void* pArg)
{
	CUI_MapDetail* pInstance = new CUI_MapDetail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MapDetail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MapDetail::Free()
{
	__super::Free();

	Safe_Release(m_pMapIcon);
}

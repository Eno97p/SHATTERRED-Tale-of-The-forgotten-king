#include "UI_MapUser.h"

#include "GameInstance.h"
#include "UI_MapIcon.h"

CUI_MapUser::CUI_MapUser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_MapUser::CUI_MapUser(const CUI_MapUser& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_MapUser::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MapUser::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 220.f; // 512
	m_fSizeY = 220.f; 

	Setting_Position();

	Create_PlayerIcon();

	return S_OK;
}

void CUI_MapUser::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MapUser::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Update_Pos();

	if (nullptr != m_pPlayerIcon)
		m_pPlayerIcon->Tick(fTimeDelta);
}

void CUI_MapUser::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);

	if (nullptr != m_pPlayerIcon)
		m_pPlayerIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_MapUser::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MapUser::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_User"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapUser::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
	//	return E_FAIL;

	return S_OK;
}

void CUI_MapUser::Create_PlayerIcon()
{
	CUI_MapIcon::UI_MAPICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 60.f;
	pDesc.fSizeY = 60.f;
	pDesc.iIndex = 5;

	m_pPlayerIcon = dynamic_cast<CUI_MapIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapIcon"), &pDesc));
}

void CUI_MapUser::Update_Pos()
{
	_uint iLevel = m_pGameInstance->Get_CurrentLevel();

	if (LEVEL_GAMEPLAY == iLevel)
	{
		m_fX = 300.f;
		m_fY = 330.f;
	}
	else if (LEVEL_ACKBAR == iLevel)
	{
		m_fX = 500.f;
		m_fY = (g_iWinSizeY >> 1 ) + 100.f;
	}
	else if (LEVEL_JUGGLAS == iLevel)
	{
		m_fX = (g_iWinSizeX >> 1) - 50.f;
		m_fY = 170.f;
	}

	m_pPlayerIcon->Update_Pos(m_fX, m_fY);

	Setting_Position();
}

CUI_MapUser* CUI_MapUser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MapUser* pInstance = new CUI_MapUser(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MapUser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MapUser::Clone(void* pArg)
{
	CUI_MapUser* pInstance = new CUI_MapUser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MapUser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MapUser::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerIcon);
}

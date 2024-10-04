#include "UI_UpgradeForge.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

CUI_UpgradeForge::CUI_UpgradeForge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_UpgradeForge::CUI_UpgradeForge(const CUI_UpgradeForge& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_UpgradeForge::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_UpgradeForge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = DEFAULT_SIZE; // 1024
	m_fSizeY = DEFAULT_SIZE;

	Setting_Position();

	return S_OK;
}

void CUI_UpgradeForge::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpgradeForge::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	__super::Tick(fTimeDelta);

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	Change_Scale();

	if (m_pGameInstance->Mouse_Down(DIM_LB) && m_isSelect) // 클릭한 경우 강화 화면으로 넘어가기
	{
		CUI_Manager::GetInstance()->Set_MenuPage(true, "UpGPage");
	}
}

void CUI_UpgradeForge::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIFTH);
}

HRESULT CUI_UpgradeForge::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_UpgradeForge::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Forge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpgradeForge::Bind_ShaderResources()
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

void CUI_UpgradeForge::Change_Scale()
{
	if (m_isSelect)
	{
		m_fSizeX = DEFAULT_SIZE + 50.f;
		m_fSizeY = DEFAULT_SIZE + 50.f;
	}
	else
	{
		m_fSizeX = DEFAULT_SIZE;
		m_fSizeY = DEFAULT_SIZE;
	}

	Setting_Position();
}

CUI_UpgradeForge* CUI_UpgradeForge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpgradeForge* pInstance = new CUI_UpgradeForge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpgradeForge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpgradeForge::Clone(void* pArg)
{
	CUI_UpgradeForge* pInstance = new CUI_UpgradeForge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpgradeForge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpgradeForge::Free()
{
	__super::Free();
}

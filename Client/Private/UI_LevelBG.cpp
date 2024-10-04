#include "UI_LevelBG.h"

#include "GameInstance.h"

CUI_LevelBG::CUI_LevelBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LevelBG::CUI_LevelBG(const CUI_LevelBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LevelBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LevelBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) - 150.f;
	m_fSizeX = 800.f;
	m_fSizeY = 800.f;

	Setting_Position();

	Resset_Animation(true);

	return S_OK;
}

void CUI_LevelBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LevelBG::Tick(_float fTimeDelta)
{
	m_fMoveTimer += fTimeDelta;

	if (1.f <= m_fMoveTimer)
	{
		m_fX += 1.5f;

		Setting_Position();

		Render_Animation(fTimeDelta, 0.8f);

		if (m_isRenderAnimFinished)
		{
			m_isDead = true;
		}
	}
}

void CUI_LevelBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_LevelBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LevelBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Level_BG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LevelBG::Bind_ShaderResources()
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

CUI_LevelBG* CUI_LevelBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LevelBG* pInstance = new CUI_LevelBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LevelBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LevelBG::Clone(void* pArg)
{
	CUI_LevelBG* pInstance = new CUI_LevelBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LevelBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LevelBG::Free()
{
	__super::Free();
}

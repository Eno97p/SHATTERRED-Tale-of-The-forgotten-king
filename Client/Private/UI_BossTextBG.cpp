#include "UI_BossTextBG.h"

#include "GameInstance.h"

CUI_BossTextBG::CUI_BossTextBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BossTextBG::CUI_BossTextBG(const CUI_BossTextBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_BossTextBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossTextBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) + 30.f;
	m_fSizeX = 800.f;
	m_fSizeY = 800.f;

	Setting_Position();

	Resset_Animation(true);

	return S_OK;
}

void CUI_BossTextBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BossTextBG::Tick(_float fTimeDelta)
{
	// 얘는 움직이는 게 아니고 디졸브로 사라지게 하는 게 어떨지
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

void CUI_BossTextBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_BossTextBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_BossTextBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossTextBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossTextBG::Bind_ShaderResources()
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

CUI_BossTextBG* CUI_BossTextBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossTextBG* pInstance = new CUI_BossTextBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BossTextBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossTextBG::Clone(void* pArg)
{
	CUI_BossTextBG* pInstance = new CUI_BossTextBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BossTextBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossTextBG::Free()
{
	__super::Free();
}

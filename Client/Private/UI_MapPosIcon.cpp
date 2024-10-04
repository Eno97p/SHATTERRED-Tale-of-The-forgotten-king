#include "UI_MapPosIcon.h"

#include "GameInstance.h"

CUI_MapPosIcon::CUI_MapPosIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_MapPosIcon::CUI_MapPosIcon(const CUI_MapPosIcon& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_MapPosIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MapPosIcon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	/*m_fX = 278.f;
	m_fY = 140.f;*/
	m_fSizeX = 80.f; // 128
	m_fSizeY = 80.f;

	Setting_Position();

	return S_OK;
}

void CUI_MapPosIcon::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MapPosIcon::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Setting_AnimData();
	UpAndDown_Anim(fTimeDelta);
}

void CUI_MapPosIcon::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_MapPosIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MapPosIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_PosIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MapPosIcon::Bind_ShaderResources()
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

void CUI_MapPosIcon::UpAndDown_Anim(_float fTimeDelta)
{
	if (m_isUpAnim) // 올라가는 중
	{
		m_fY -= fTimeDelta * 30.f;

		if (m_fY <= (m_fDefaultY - ANIM_Y))
		{
			m_fY = m_fDefaultY - ANIM_Y;
			m_isUpAnim = false;
		}
	}
	else // 내려가는 중
	{
		m_fY += fTimeDelta * 30.f;
		
		if (m_fY >= (m_fDefaultY + ANIM_Y))
		{
			m_fY = m_fDefaultY + ANIM_Y;
			m_isUpAnim = true;
		}
	}

	Setting_Position();
}

void CUI_MapPosIcon::Setting_AnimData()
{
	_uint iLevel = m_pGameInstance->Get_CurrentLevel();

	if (LEVEL_GAMEPLAY == iLevel)
	{
		m_fX = 270.f;
		m_fDefaultY = 200.f;

	}
	else if (LEVEL_ACKBAR == iLevel)
	{
		m_fX = 470.f;
		m_fDefaultY = (g_iWinSizeY >> 1) - 30.f;
	}
	else if (LEVEL_JUGGLAS == iLevel)
	{
		m_fX = (g_iWinSizeX >> 1) - 50.f;
		m_fDefaultY = 40.f;
	}

	if (m_fY < m_fDefaultY - ANIM_Y)
		m_fY = m_fDefaultY - ANIM_Y;
}

CUI_MapPosIcon* CUI_MapPosIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MapPosIcon* pInstance = new CUI_MapPosIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MapPosIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MapPosIcon::Clone(void* pArg)
{
	CUI_MapPosIcon* pInstance = new CUI_MapPosIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MapPosIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MapPosIcon::Free()
{
	__super::Free();
}

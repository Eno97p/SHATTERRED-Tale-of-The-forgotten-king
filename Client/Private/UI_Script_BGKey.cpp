#include "UI_Script_BGKey.h"

#include "GameInstance.h"

CUI_Script_BGKey::CUI_Script_BGKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Script_BGKey::CUI_Script_BGKey(const CUI_Script_BGKey& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Script_BGKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Script_BGKey::Initialize(void* pArg)
{
	UI_SCRIPT_KEY_DESC* pDesc = static_cast<UI_SCRIPT_KEY_DESC*>(pArg);

	m_iKeyIdx = pDesc->iKeyIdx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	/*m_fX = 278.f;
	m_fY = 140.f;*/
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX; // 128
	m_fSizeY = g_iWinSizeY;

	if (0 == m_iKeyIdx)
		m_isUpAnim = true;
	else
		m_isUpAnim = false;

	Setting_Position();

	return S_OK;
}

void CUI_Script_BGKey::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Script_BGKey::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	//Setting_AnimData();
	UpAndDown_Anim(fTimeDelta);
}

void CUI_Script_BGKey::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTH);
}

HRESULT CUI_Script_BGKey::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10); // 8?
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Script_BGKey::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_ChoronKey"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Script_BGKey::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iKeyIdx)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
	//	return E_FAIL;

	return S_OK;
}

void CUI_Script_BGKey::UpAndDown_Anim(_float fTimeDelta)
{
	if (m_isUpAnim) // 올라가는 중
	{
		m_fY -= fTimeDelta * 30.f;

		if (m_fY <= ((g_iWinSizeY >> 1) - ANIM_Y))
		{
			m_fY = (g_iWinSizeY >> 1) - ANIM_Y;
			m_isUpAnim = false;
		}
	}
	else // 내려가는 중
	{
		m_fY += fTimeDelta * 30.f;

		if (m_fY >= ((g_iWinSizeY >> 1) + ANIM_Y))
		{
			m_fY = (g_iWinSizeY >> 1) + ANIM_Y;
			m_isUpAnim = true;
		}
	}

	Setting_Position();
}

CUI_Script_BGKey* CUI_Script_BGKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Script_BGKey* pInstance = new CUI_Script_BGKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Script_BGKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Script_BGKey::Clone(void* pArg)
{
	CUI_Script_BGKey* pInstance = new CUI_Script_BGKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Script_BGKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Script_BGKey::Free()
{
	__super::Free();
}

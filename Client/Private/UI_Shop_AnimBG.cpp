#include "UI_Shop_AnimBG.h"

#include "GameInstance.h"

CUI_Shop_AnimBG::CUI_Shop_AnimBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Shop_AnimBG::CUI_Shop_AnimBG(const CUI_Shop_AnimBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Shop_AnimBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Shop_AnimBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_Shop_AnimBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Shop_AnimBG::Tick(_float fTimeDelta)
{
	// 이걸 흘러가게 하지 말고 커졌다가 작아지는 식으로 바꾸기 >>> AckBar에 추가하고 나면!
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_fFlowTimer += fTimeDelta * 0.03f;
	if (10.f <= m_fFlowTimer)
	{
		m_fFlowTimer = 0.f;
	}
}

void CUI_Shop_AnimBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTH);
}

HRESULT CUI_Shop_AnimBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Shop_AnimBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ValnirBG_Anim"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop_AnimBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowTime", &m_fFlowTimer, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
	//	return E_FAIL;

	return S_OK;
}

CUI_Shop_AnimBG* CUI_Shop_AnimBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shop_AnimBG* pInstance = new CUI_Shop_AnimBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Shop_AnimBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Shop_AnimBG::Clone(void* pArg)
{
	CUI_Shop_AnimBG* pInstance = new CUI_Shop_AnimBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Shop_AnimBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shop_AnimBG::Free()
{
	__super::Free();
}

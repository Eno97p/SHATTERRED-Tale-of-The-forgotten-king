#include "UI_LogoBanner.h"

#include "GameInstance.h"

CUI_LogoBanner::CUI_LogoBanner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LogoBanner::CUI_LogoBanner(const CUI_LogoBanner& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LogoBanner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LogoBanner::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) - 70.f;
	//m_fSizeX = 1365.f; // 2048.f
	//m_fSizeY = 341.3f; // 512.f
	m_fSizeX = g_iWinSizeX; // 2048.f
	m_fSizeY = 341.3f; // 512.f

	Setting_Position();

	return S_OK;
}

void CUI_LogoBanner::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LogoBanner::Tick(_float fTimeDelta)
{
	//if (!m_isRenderAnimFinished)
	//Render_Animation(fTimeDelta); // Dissolve로 수정할 예정

	m_fGlitchTimer += fTimeDelta;

	// Dissolve 애니메이션 처리 필요함(덜 됨)
	//if (!m_isRenderAnimFinished)
	//{
	//	m_fRenderTimer += fTimeDelta * 0.3f;

	//	if (m_fRenderTimer >= 1.f)
	//	{
	//		m_fRenderTimer = 1.f;

	//		if (!m_isRenderOffAnim)
	//		{
	//			//// 화면 전환? 필요
	//			//if (FAILED(Create_FadeIn()))
	//			//	return;
	//		}
	//		m_isRenderAnimFinished = true;
	//	}
	//}


	//if (!m_isRenderAnimFinished)
	//	Render_Animation(fTimeDelta); // 얘는 수치값등등 다르게 처리?
}

void CUI_LogoBanner::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRD);
}

HRESULT CUI_LogoBanner::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4); //4
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LogoBanner::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_Banner"), // Prototype_Component_Texture_Logo_Banner
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	// 텍스쳐의 문제였네... Banner로는 안 됨! 왤까나
	// 기본 텍스쳐가 Banner인 게 문제임~~!!!!!!

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"), // Prototype_Component_Texture_Desolve16
		TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LogoBanner::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 29)))
		return E_FAIL;


	// 글리치 시도
	//texture2D	g_NoiseTexture;
	//float		g_GlitchTimer;

	/*if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlitchTimer", &m_fGlitchTimer, sizeof(_float))))
		return E_FAIL;*/



	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_LogoBanner* CUI_LogoBanner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LogoBanner* pInstance = new CUI_LogoBanner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LogoBanner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LogoBanner::Clone(void* pArg)
{
	CUI_LogoBanner* pInstance = new CUI_LogoBanner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LogoBanner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LogoBanner::Free()
{
	__super::Free();

	Safe_Release(m_pDisolveTextureCom);
}

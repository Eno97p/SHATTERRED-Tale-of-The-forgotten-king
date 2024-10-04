#include "UI_Memento.h"

#include "GameInstance.h"

CUI_Memento::CUI_Memento(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Memento::CUI_Memento(const CUI_Memento& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Memento::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Memento::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1);
	m_fY = (g_iWinSizeY >> 1);
	m_fSizeX = 512.f;
	m_fSizeY = 256.f;

	Setting_Position();

	return S_OK;
}

void CUI_Memento::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Memento::Tick(_float fTimeDelta)
{
	m_fRenderTimer += fTimeDelta * 0.7f;

	if (1.f <= m_fRenderTimer) // 시간 채워지면
	{
		if (!m_isRenderOffAnim) // 점점 켜지는 중이면
		{
			m_isRenderOffAnim = true;
			m_fRenderTimer = 0.f;
			m_fColor = 1.f;
		}
		else // 꺼지는 중이면
		{
			m_isRend = false;
		}
	}

	// Font Color
	if (!m_isRenderOffAnim) // 점점 켜지는 중이면
	{
		m_fColor += 0.02f;
		if (m_fColor >= 1.f)
			m_fColor = 1.f;
	}
	else
	{
		m_fColor -= 0.02f;
		if (m_fColor <= 0.f)
			m_fColor = 0.f;
	}
}

void CUI_Memento::Late_Tick(_float fTimeDelta)
{
	if(m_isRend)
		CGameInstance::GetInstance()->Add_UI(this, SEVENTEENTH); // 더 추가될 수 있음
}

HRESULT CUI_Memento::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3); // 8
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	// 글씨 색도 같이 옅어지도록 처리
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo23"), TEXT("Memento"), _float2(m_fX - 70.f, m_fY - 15.f), XMVectorSet(m_fColor, m_fColor, m_fColor, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Memento::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Memento"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Memento::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool)))) // m_isRenderOffAnim
		return E_FAIL;

	return S_OK;
}

CUI_Memento* CUI_Memento::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Memento* pInstance = new CUI_Memento(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Memento");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Memento::Clone(void* pArg)
{
	CUI_Memento* pInstance = new CUI_Memento(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Memento");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Memento::Free()
{
	__super::Free();
}

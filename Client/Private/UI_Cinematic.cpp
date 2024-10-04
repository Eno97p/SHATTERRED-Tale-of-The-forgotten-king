#include "UI_Cinematic.h"

#include "GameInstance.h"

CUI_Cinematic::CUI_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Cinematic::CUI_Cinematic(const CUI_Cinematic& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Cinematic::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Cinematic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = fMAXSIZE;

	Setting_Position();

	return S_OK;
}

void CUI_Cinematic::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Cinematic::Tick(_float fTimeDelta)
{
	// 컷씬 카메라 생성 시 호출, 삭제 시 호출하는 식으로 함수 짜기

	if (!m_isAnimIn) // 줄어들기
	{
		m_isRend = true;

		m_fSizeY -= 5.f;
		if (m_fSizeY < fMINSIZE)
		{
			m_fSizeY = fMINSIZE;
		}
		Setting_Position();
	}
	else // 커지기
	{
		m_fSizeY += 5.f;
		if (m_fSizeY > fMAXSIZE)
		{
			m_fSizeY = fMAXSIZE;
			m_isRend = false;
		}
		Setting_Position();
	}
}

void CUI_Cinematic::Late_Tick(_float fTimeDelta)
{
	if(m_isRend)
		CGameInstance::GetInstance()->Add_UI(this, SEVENTEENTH);
}

HRESULT CUI_Cinematic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Cinematic::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Cinematic"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cinematic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_Cinematic* CUI_Cinematic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Cinematic* pInstance = new CUI_Cinematic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Cinematic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Cinematic::Clone(void* pArg)
{
	CUI_Cinematic* pInstance = new CUI_Cinematic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Cinematic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Cinematic::Free()
{
	__super::Free();
}

#include "UI_QTE_Particle.h"

#include "GameInstance.h"

CUI_QTE_Particle::CUI_QTE_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_QTE_Particle::CUI_QTE_Particle(const CUI_QTE_Particle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_QTE_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_QTE_Particle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	Resset_Animation(true); // Test

	return S_OK;
}

void CUI_QTE_Particle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QTE_Particle::Tick(_float fTimeDelta)
{
	m_fDeadTimer += fTimeDelta;

	m_fSizeX *= 1.1f; // 1.1
	m_fSizeY *= 1.1f;
	Setting_Position();

	//if (0.1f <= m_fDeadTimer) // 이 부분도 수정이 들어가야 함
	//{
	//	if (!m_isRenderAnimFinished)
	//		Render_Animation(fTimeDelta, 10.f);// 10
	//	else
	//		m_isDead = true;
	//}

	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta, 600.f);// 10
	else
		m_isDead = true;
}

void CUI_QTE_Particle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_QTE_Particle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_QTE_Particle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Particle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QTE_Particle::Bind_ShaderResources()
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

CUI_QTE_Particle* CUI_QTE_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QTE_Particle* pInstance = new CUI_QTE_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QTE_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QTE_Particle::Clone(void* pArg)
{
	CUI_QTE_Particle* pInstance = new CUI_QTE_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QTE_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QTE_Particle::Free()
{
	__super::Free();
}

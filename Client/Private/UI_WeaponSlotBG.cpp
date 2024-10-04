#include "UI_WeaponSlotBG.h"

#include "GameInstance.h"

#include "UI_HUDEffect.h"

CUI_WeaponSlotBG::CUI_WeaponSlotBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_WeaponSlotBG::CUI_WeaponSlotBG(const CUI_WeaponSlotBG& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_WeaponSlotBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WeaponSlotBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 140.f;
	m_fY = g_iWinSizeY - 110.f;
	m_fSizeX = 204.f; // 512
	m_fSizeY = 204.f; // 512

	Setting_Position();

	return S_OK;
}

void CUI_WeaponSlotBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WeaponSlotBG::Tick(_float fTimeDelta)
{
	if (nullptr != m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);

		if (m_pEffect->Get_isDead())
		{
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}
}

void CUI_WeaponSlotBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);

	if (nullptr != m_pEffect)
		m_pEffect->Late_Tick(fTimeDelta);
}

HRESULT CUI_WeaponSlotBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WeaponSlotBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_WeaponSlotBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WeaponSlotBG::Bind_ShaderResources()
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

void CUI_WeaponSlotBG::Create_UI(_bool isSkill)
{
	if (m_pEffect != nullptr)
		return;

	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fSizeX = 100.f;
	pDesc.fSizeY = 100.f;

	if (isSkill)
	{
		pDesc.fX = 140.f;
		pDesc.fY = (g_iWinSizeY >> 1) + 220.f;
	}
	else
	{
		pDesc.fX = 190.f;
		pDesc.fY = (g_iWinSizeY >> 1) + 270.f;
	}

	m_pEffect = dynamic_cast<CUI_HUDEffect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_HUDEffect"), &pDesc));
}

CUI_WeaponSlotBG* CUI_WeaponSlotBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WeaponSlotBG* pInstance = new CUI_WeaponSlotBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WeaponSlotBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponSlotBG::Clone(void* pArg)
{
	CUI_WeaponSlotBG* pInstance = new CUI_WeaponSlotBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WeaponSlotBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponSlotBG::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}

#include "UI_WeaponSlot.h"

#include "GameInstance.h"

CUI_WeaponSlot::CUI_WeaponSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_WeaponSlot::CUI_WeaponSlot(const CUI_WeaponSlot& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_WeaponSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WeaponSlot::Initialize(void* pArg)
{
	UI_WEAPONSLOT_DESC* pDesc = static_cast<UI_WEAPONSLOT_DESC*>(pArg);

	m_eSlotNum = pDesc->eSlotNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 85.3f; // 512
	m_fSizeY = 85.3f; // 512

	Setting_Position();

	return S_OK;
}

void CUI_WeaponSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WeaponSlot::Tick(_float fTimeDelta)
{
}

void CUI_WeaponSlot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_WeaponSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WeaponSlot::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// 일단 깡통 출력 위해 임의로
	switch (m_eSlotNum)
	{
	case Client::CUI_WeaponSlot::SLOT_ONE:
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_LimboBlade"), // 추후 Texture 바꾸는 거 필요
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case Client::CUI_WeaponSlot::SLOT_TWO:
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_EtherBolt"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	case Client::CUI_WeaponSlot::SLOT_THREE:
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Whisperer"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_WeaponSlot::Bind_ShaderResources()
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

void CUI_WeaponSlot::Setting_XY()
{
	switch (m_eSlotNum)
	{
	case Client::CUI_WeaponSlot::SLOT_ONE:
		m_fX = 90.f;
		m_fY = g_iWinSizeY - 100.f;
		break;
	case Client::CUI_WeaponSlot::SLOT_TWO:
		m_fX = 140.f;
		m_fY = g_iWinSizeY - 140.f;
		break;
	case Client::CUI_WeaponSlot::SLOT_THREE:
		m_fX = 190.f;
		m_fY = g_iWinSizeY - 100.f;
		break;
	default:
		break;
	}
}

HRESULT CUI_WeaponSlot::Change_Texture()
{
	return S_OK;
}

CUI_WeaponSlot* CUI_WeaponSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WeaponSlot* pInstance = new CUI_WeaponSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WeaponSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponSlot::Clone(void* pArg)
{
	CUI_WeaponSlot* pInstance = new CUI_WeaponSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WeaponSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponSlot::Free()
{
	__super::Free();
}

#include "UI_Slot_EquipSign.h"

#include "GameInstance.h"

CUI_Slot_EquipSign::CUI_Slot_EquipSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Slot_EquipSign::CUI_Slot_EquipSign(const CUI_Slot_EquipSign& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Slot_EquipSign::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Slot_EquipSign::Initialize(void* pArg)
{
	UI_EQUIPSIGN_DESC* pDesc = static_cast<UI_EQUIPSIGN_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_Slot_EquipSign::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Slot_EquipSign::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_Slot_EquipSign::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);
}

HRESULT CUI_Slot_EquipSign::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Slot_EquipSign::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_EquipSign"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot_EquipSign::Bind_ShaderResources()
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

CUI_Slot_EquipSign* CUI_Slot_EquipSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Slot_EquipSign* pInstance = new CUI_Slot_EquipSign(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Slot_EquipSign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Slot_EquipSign::Clone(void* pArg)
{
	CUI_Slot_EquipSign* pInstance = new CUI_Slot_EquipSign(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Slot_EquipSign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Slot_EquipSign::Free()
{
	__super::Free();
}

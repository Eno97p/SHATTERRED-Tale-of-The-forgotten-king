#include "UI_Slot_Frame.h"

#include "GameInstance.h"

CUI_Slot_Frame::CUI_Slot_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Slot_Frame::CUI_Slot_Frame(const CUI_Slot_Frame& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Slot_Frame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Slot_Frame::Initialize(void* pArg)
{
	UI_SLOTFRAME_DESC* pDesc = static_cast<UI_SLOTFRAME_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_Slot_Frame::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Slot_Frame::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_Slot_Frame::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);
}

HRESULT CUI_Slot_Frame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Slot_Frame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot_SelectFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot_Frame::Bind_ShaderResources()
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

CUI_Slot_Frame* CUI_Slot_Frame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Slot_Frame* pInstance = new CUI_Slot_Frame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Slot_Frame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Slot_Frame::Clone(void* pArg)
{
	CUI_Slot_Frame* pInstance = new CUI_Slot_Frame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Slot_Frame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Slot_Frame::Free()
{
	__super::Free();
}

#include "UI_UpGPage_SelectSlot.h"

#include "GameInstance.h"
#include "Inventory.h"

CUI_UpGPage_SelectSlot::CUI_UpGPage_SelectSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpGPage_SelectSlot::CUI_UpGPage_SelectSlot(const CUI_UpGPage_SelectSlot& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpGPage_SelectSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPage_SelectSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_UpGPage_SelectSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPage_SelectSlot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpGPage_SelectSlot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_UpGPage_SelectSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Rend_Font();

	return S_OK;
}

void CUI_UpGPage_SelectSlot::Update_Data(_uint iCurSlotIdx)
{
	if (CInventory::GetInstance()->Get_WeaponSize() - 1 < iCurSlotIdx) // 빈 슬롯 예외 처리
		return;

	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < iCurSlotIdx; ++i)
		++weapon;

	m_iLevel = (*weapon)->Get_Level();
	m_iAddDamage = (*weapon)->Get_AddDamage();
}

HRESULT CUI_UpGPage_SelectSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_SlotSelect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_SelectSlot::Bind_ShaderResources()
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

void CUI_UpGPage_SelectSlot::Rend_Font()
{
	// Item Name
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrItemName, _float2(m_fX - 80.f, m_fY - 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	// NameBox
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_wstrItemName, _float2((g_iWinSizeX >> 1) + 210.f, 217.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	// Level
	wstring wstrLevel = TEXT("|  level : ") + to_wstring(m_iLevel);
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), wstrLevel, _float2((g_iWinSizeX >> 1) + 400.f, 217.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	// Damage
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("DAMAGE"), _float2((g_iWinSizeX >> 1) + 310.f, (g_iWinSizeY >> 1) - 80.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("Actual level"), _float2((g_iWinSizeX >> 1) + 180.f, (g_iWinSizeY >> 1) - 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("Next level"), _float2((g_iWinSizeX >> 1) + 180.f, (g_iWinSizeY >> 1)), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	//Add Damage를 Level과 곱해서 출력해주기
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("+ ") + to_wstring(m_iAddDamage * m_iLevel), _float2((g_iWinSizeX >> 1) + 340.f, (g_iWinSizeY >> 1) - 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("+ ") + to_wstring(m_iAddDamage * (m_iLevel + 1)), _float2((g_iWinSizeX >> 1) + 340.f, (g_iWinSizeY >> 1)), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

}

CUI_UpGPage_SelectSlot* CUI_UpGPage_SelectSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPage_SelectSlot* pInstance = new CUI_UpGPage_SelectSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPage_SelectSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPage_SelectSlot::Clone(void* pArg)
{
	CUI_UpGPage_SelectSlot* pInstance = new CUI_UpGPage_SelectSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPage_SelectSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPage_SelectSlot::Free()
{
	__super::Free();
}

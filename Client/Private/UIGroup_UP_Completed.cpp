#include "UIGroup_UP_Completed.h"

#include "GameInstance.h"
#include "Inventory.h"

#include "UI_UpCompletedBG.h"
#include "UI_UpCompleted_Circle.h"
#include "UI_UpCompleted_Forge.h"
#include "UI_UpCompleted_Crucible.h"

CUIGroup_UP_Completed::CUIGroup_UP_Completed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{ pDevice, pContext }
{
}

CUIGroup_UP_Completed::CUIGroup_UP_Completed(const CUIGroup_UP_Completed& rhs)
    : CUIGroup{ rhs }
{
}

HRESULT CUIGroup_UP_Completed::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_UP_Completed::Initialize(void* pArg)
{
	UIGROUP_COMPLETED_DESC* pDesc = static_cast<UIGROUP_COMPLETED_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI(Setting_TextureName(pDesc->iCurSlotIdx))))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_UP_Completed::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_UP_Completed::Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
	{
		pUI->Tick(fTimeDelta);
	}

	m_fCloseTimer += fTimeDelta;
	if (3.f <= m_fCloseTimer)
	{
		m_isEnd = true;
	}
}

void CUIGroup_UP_Completed::Late_Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
		pUI->Late_Tick(fTimeDelta);
}

HRESULT CUIGroup_UP_Completed::Render()
{
	return S_OK;
}

HRESULT CUIGroup_UP_Completed::Create_UI(wstring wstrTextureName)
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	// BG 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpCompletedBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpCompletedBG"), &pDesc)));

	// Circle
	CUI_UpCompleted_Circle::UI_CIRCLE_DESC pCircleDesc{};
	pCircleDesc.eLevel = LEVEL_STATIC;
	pCircleDesc.fX = (g_iWinSizeX >> 1) - 15.f;
	pCircleDesc.fY = (g_iWinSizeY >> 1) - 85.f;
	pCircleDesc.fSizeX = 900.f; // 512
	pCircleDesc.fSizeY = 900.f;
	pCircleDesc.eCircleType = CUI_UpCompleted_Circle::CIRCLE_ONE;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpCompleted_Circle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpCompleted_Circle"), &pCircleDesc)));

	pCircleDesc.fX = (g_iWinSizeX >> 1) - 15.f;
	pCircleDesc.fY = (g_iWinSizeY >> 1) - 85.f;
	pCircleDesc.fSizeX = 880.f;
	pCircleDesc.fSizeY = 880.f;
	pCircleDesc.eCircleType = CUI_UpCompleted_Circle::CIRCLE_TWO;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpCompleted_Circle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpCompleted_Circle"), &pCircleDesc)));

	// Forge
	CUI_UpCompleted_Forge::UI_UPCOM_FORGE_DESC pForgeDesc{};
	pForgeDesc.eLevel = LEVEL_STATIC;
	pForgeDesc.wstrTextureName = wstrTextureName;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpCompleted_Forge*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpCompleted_Forge"), &pForgeDesc)));

	// Crucible
	m_vecUI.emplace_back(dynamic_cast<CUI_UpCompleted_Crucible*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpCompleted_Crucible"), &pDesc)));

	return S_OK;
}

wstring CUIGroup_UP_Completed::Setting_TextureName(_uint iCurSlotIdx)
{
	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < iCurSlotIdx; ++i)
		++weapon;

	return (*weapon)->Get_TextureName();
}

CUIGroup_UP_Completed* CUIGroup_UP_Completed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_UP_Completed* pInstance = new CUIGroup_UP_Completed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_UP_Completed");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_UP_Completed::Clone(void* pArg)
{
	CUIGroup_UP_Completed* pInstance = new CUIGroup_UP_Completed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_UP_Completed");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_UP_Completed::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}

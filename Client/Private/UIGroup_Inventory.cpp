#include "UIGroup_Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "UI_MenuPageBG.h"
#include "UI_MenuPageTop.h"
#include "UI_StateSoul.h"
#include "UI_QuickExplain.h"
#include "UI_Slot.h"
#include "UI_QuickInvBG.h"
#include "UIGroup_InvSub.h"

CUIGroup_Inventory::CUIGroup_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Inventory::CUIGroup_Inventory(const CUIGroup_Inventory& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Inventory::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Inventory::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Inventory::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Inventory::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend &&
		!(dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Get_Rend()))
	{
		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}

			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;

		for (auto& pSlot : m_vecSlot)
		{
			if (!m_isRenderOnAnim && !(pSlot->Get_RenderOnAnim()))
			{
				pSlot->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pSlot->Get_RenderOnAnim())
			{
				pSlot->Resset_Animation(false);
			}

			pSlot->Tick(fTimeDelta);
		}

		m_pSoul->Tick(fTimeDelta);
	}
}

void CUIGroup_Inventory::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pSlot : m_vecSlot)
			pSlot->Late_Tick(fTimeDelta);

		m_pSoul->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Inventory::Render()
{
	return S_OK;
}

void CUIGroup_Inventory::Update_Inventory_Add(_uint iSlotIdx)
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iSlotIdx; ++i)
		++slot;

	(*slot)->Create_ItemIcon_Inv();
}

void CUIGroup_Inventory::Update_Slot_EquipSign(_uint iCurSlotIdx, _bool isEquip)
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iCurSlotIdx; ++i)
		++slot;

	(*slot)->Set_isEquip(isEquip);
}

void CUIGroup_Inventory::Update_Inventory_Delete(_uint iSlotIdx)
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iSlotIdx; ++i)
		++slot;

	(*slot)->Delete_ItemIcon();
}

void CUIGroup_Inventory::Rend_Calcul(_int iSoul)
{
	m_pSoul->Rend_Calcul(iSoul);
}

void CUIGroup_Inventory::Update_Inventory(_uint iSlotIdx)
{
	// slot을 순회하면서 Inventory의 m_vecItem.size 보다 slotIdx가 작거나 같은데 ItemIcon이 nullptr이라면 그 다음 한칸씩 쭉 당겨오도록 구현
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();

	for (size_t i = 0; i < iSlotIdx; ++i)
		++slot;

	while (slot != m_vecSlot.end()) // 마지막이 아닐 동안 while문을 돌기
	{
		++slot; // 다음 슬롯에 접근해서 정보 가져오기

		if ((*slot)->Get_isItemIconNull())
			break;

		_bool isEquip = (*slot)->Get_isEquip();

		wstring wstrTexture, wstrItemName, wstrItemExplain, wstrItemExplain_Quick;
		wstrTexture = (*slot)->Get_Texture();
		wstrItemName = (*slot)->Get_ItemName();
		wstrItemExplain = (*slot)->Get_ItemExplain();
		wstrItemExplain_Quick = (*slot)->Get_ItemExplain_Quick();

		// 다음 슬롯 비워주기
		(*slot)->Delete_ItemIcon();

		--slot;
		(*slot)->Pull_ItemIcon(isEquip, wstrTexture, wstrItemName, wstrItemExplain, wstrItemExplain_Quick);

		++slot;
	}
}

HRESULT CUIGroup_Inventory::Create_RedDot(_uint iSlotIdx)
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iSlotIdx; ++i)
		++slot;

	return (*slot)->Create_RedDot();
}

HRESULT CUIGroup_Inventory::Delete_RedDot()
{
	for (auto& pSlot : m_vecSlot)
		pSlot->Delete_RedDot();

	return S_OK;
}

HRESULT CUIGroup_Inventory::Create_UI()
{
	CUI::UI_DESC pDesc{};

	// BG
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageBG"), &pDesc)));

	// Top
	CUI_MenuPageTop::UI_TOP_DESC pTopDesc{};
	pTopDesc.eLevel = LEVEL_STATIC;
	pTopDesc.eTopType = CUI_MenuPageTop::TOP_INV;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageTop"), &pTopDesc)));

	// Soul
	CUI_StateSoul::UI_SOUL_DESC pSoulDesc{}; 
	pSoulDesc.eLevel = LEVEL_STATIC;
	pSoulDesc.fX = 250.f;
	pSoulDesc.fY = 120.f;
	pSoulDesc.fSizeX = 32.f;
	pSoulDesc.fSizeY = 32.f;
	pSoulDesc.eUISort = NINETH;
	m_pSoul = dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc));

	// InvBG
	CUI_QuickInvBG::UI_INVBG_DESC pInvBGDesc{};
	pInvBGDesc.eLevel = LEVEL_STATIC;
	pInvBGDesc.isInv = true;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickInvBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickInvBG"), &pInvBGDesc)));

	// Explain
	CUI_QuickExplain::UI_EXPLAIN_DESC pExplainDesc{};
	pExplainDesc.eLevel = LEVEL_STATIC;
	pExplainDesc.eUISort = NINETH;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickExplain*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickExplain"), &pExplainDesc)));

	Create_Slot();
		
	return S_OK;
}

HRESULT CUIGroup_Inventory::Create_Slot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	_uint iSlotIdx = { 0 };
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 5; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = 230.f + (j * 76.f); // 160
			pDesc.fY = 200.f + (i * 76.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.iSlotIdx = iSlotIdx;
			pDesc.eSlotType = CUI_Slot::SLOT_INV;
			pDesc.eUISort = NINETH;
			m_vecSlot.emplace_back(dynamic_cast<CUI_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		
			++iSlotIdx;
		}
	}

	return S_OK;
}

CUIGroup_Inventory* CUIGroup_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Inventory* pInstance = new CUIGroup_Inventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Inventory");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Inventory::Clone(void* pArg)
{
	CUIGroup_Inventory* pInstance = new CUIGroup_Inventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Inventory");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Inventory::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
	m_vecUI.clear();

	for (auto& pSlot : m_vecSlot)
		Safe_Release(pSlot);
	m_vecSlot.clear();

	Safe_Release(m_pSoul);
}

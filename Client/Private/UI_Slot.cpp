#include "UI_Slot.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "CMouse.h"

#include "UI_Slot_Frame.h"
#include "UI_ItemIcon.h"
#include "UI_Slot_EquipSign.h"

#include "UIGroup_Inventory.h"
#include "UIGroup_InvSub.h"
#include "UIGroup_Weapon.h"
#include "UIGroup_Quick.h"
#include "UIGroup_WeaponSlot.h"

#include "UI_RedDot.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_Slot::CUI_Slot(const CUI_Slot& rhs)
	: CUI_Interaction{ rhs }
{
}

void CUI_Slot::Resset_Data()
{
	Safe_Release(m_pItemIcon);
	Safe_Release(m_pSymbolIcon);
	m_pItemIcon = nullptr;
	m_pSymbolIcon = nullptr;
	m_isEquip = false; 
}

void CUI_Slot::Clear_ItemIcon()
{
	m_pItemIcon = nullptr;
	m_pSymbolIcon = nullptr;
	m_isEquip = false;
}

void CUI_Slot::Check_Equip(_bool isWeapon, CItemData* pItemData)
{
	for (size_t i = 0; i < 3; ++i)
	{
		 if (isWeapon) // weapon 일 때
		 {
			 if (pItemData == CInventory::GetInstance()->Get_EquipWeapon(i))
			 {
				 m_isEquip = true;
			 }

		 }
		 else // skill 일 때
		 {
			 if (pItemData == CInventory::GetInstance()->Get_EquipSkill(i))
			 {
				 m_isEquip = true;
			 }
		 }
	}
}

HRESULT CUI_Slot::Create_RedDot()
{
	if (nullptr == m_pItemIcon)
		return S_OK;

	return m_pItemIcon->Create_RedDot();
}

HRESULT CUI_Slot::Delete_RedDot()
{
	if (nullptr == m_pItemIcon)
		return S_OK;

	return m_pItemIcon->Delete_RedDot();
}

HRESULT CUI_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
	UI_SLOT_DESC* pDesc = static_cast<UI_SLOT_DESC*>(pArg);

	m_iSlotIdx = pDesc->iSlotIdx;
	m_eUISort = pDesc->eUISort;
	m_eSlotType = pDesc->eSlotType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Create_Frame();
	Create_EquipSign();

	Setting_Position();

	return S_OK;
}

void CUI_Slot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Slot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	__super::Tick(fTimeDelta);

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_pGameInstance->Mouse_Down(DIM_LB) && m_isSelect) // Slot이 빈 슬롯이 아니고 사용 or QuickAccess에 등록 가능한 아이템인 경우에만 한하도록 예외 처리 필요
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("SFX_UI_New_Button.wav"), SOUND_UI);

		if (nullptr != m_pItemIcon || SLOT_QUICK == m_eSlotType)
		{
			Click_BtnEvent();
		}
	}

	if (nullptr != m_pSelectFrame)
		m_pSelectFrame->Tick(fTimeDelta);

	if (nullptr != m_pItemIcon)
		m_pItemIcon->Tick(fTimeDelta);

	if (nullptr != m_pSymbolIcon)
		m_pSymbolIcon->Tick(fTimeDelta);

	if (nullptr != m_pEquipSign)
		m_pEquipSign->Tick(fTimeDelta);

	//if (nullptr != m_pRedDot)
	//	m_pRedDot->Tick(fTimeDelta);
}

void CUI_Slot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);

	if (nullptr != m_pSelectFrame && m_isSelect)
		m_pSelectFrame->Late_Tick(fTimeDelta);

	// UIGroup의 RenderOnAnim이 false 일 때가 점점 꺼지는 중인 거니까 그때가 되면 ItemIcon은 아예 Render 그룹에 넣지 않는 것으로?
	if (nullptr != m_pItemIcon && Check_GroupRenderOnAnim())
		m_pItemIcon->Late_Tick(fTimeDelta);

	if (nullptr != m_pSymbolIcon && m_isSelect)
		m_pSymbolIcon->Late_Tick(fTimeDelta);

	if (nullptr != m_pEquipSign && m_isEquip && Check_GroupRenderOnAnim())
		m_pEquipSign->Late_Tick(fTimeDelta);
}

HRESULT CUI_Slot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (m_isSelect && nullptr != m_pItemIcon)
		Render_Font();

	if(nullptr != m_pItemIcon && (SLOT_INV == m_eSlotType || SLOT_QUICKINV == m_eSlotType))
		Rend_Count();

	return S_OK;
}

HRESULT CUI_Slot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot::Bind_ShaderResources()
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

HRESULT CUI_Slot::Create_Frame()
{
	CUI_Slot_Frame::UI_SLOTFRAME_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 85.3f;
	pDesc.fSizeY = 85.3f;
	
	if (SLOT_QUICK == m_eSlotType)
		pDesc.eUISort = ELEVENTH; // ELEVENTH
	else if(SLOT_QUICKINV == m_eSlotType)
		pDesc.eUISort = ELEVENTH;
	else if (SLOT_INV == m_eSlotType || SLOT_WEAPON == m_eSlotType)
		pDesc.eUISort = TENTH;
	else if (SLOT_INVSUB == m_eSlotType)
		pDesc.eUISort = FOURTEENTH;

	m_pSelectFrame = dynamic_cast<CUI_Slot_Frame*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot_Frame"), &pDesc));
	if (nullptr == m_pSelectFrame)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot::Create_EquipSign()
{
	CUI_Slot_EquipSign::UI_EQUIPSIGN_DESC pDesc{};
	
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;

	if (SLOT_QUICK == m_eSlotType)
		pDesc.eUISort = THIRTEENTH;
	else if(SLOT_QUICKINV == m_eSlotType)
		pDesc.eUISort = THIRTEENTH;
	else if (SLOT_INV == m_eSlotType || SLOT_WEAPON == m_eSlotType)
		pDesc.eUISort = TENTH;
	else if (SLOT_INVSUB == m_eSlotType)
		pDesc.eUISort = FOURTEENTH;

	m_pEquipSign = dynamic_cast<CUI_Slot_EquipSign*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot_EquipSign"), &pDesc));
	if (nullptr == m_pEquipSign)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon_Inv()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = ELEVENTH;
	pDesc.wszTexture = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wstrTexture = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_TextureName();
	m_wszItemName = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_ItemNameText();
	m_wszItemExplain = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_ItemExplainText();

	m_wszItemExplain_Quick = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_ItemExplainText_Quick();

	if (SLOT_INV == m_eSlotType)
	{
		// 여기서 Symbol Icon도 생성해주기
		pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
		pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
		pDesc.fSizeX = 160.f;
		pDesc.fSizeY = 160.f;
		m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));
	}

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon_SubQuick(_uint iSlotIdx)
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = SIXTEENTH;
	pDesc.wszTexture = CInventory::GetInstance()->Get_ItemData(iSlotIdx)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = CInventory::GetInstance()->Get_ItemData(iSlotIdx)->Get_ItemNameText();
	m_wszItemExplain = CInventory::GetInstance()->Get_ItemData(iSlotIdx)->Get_ItemExplainText();

	m_iInventoryIdx = iSlotIdx; // 여기서 넣어줬는데 왜 안 넣어준 것처럼 무조건 0이라고 뜰까

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon_Quick(CItemData* pItemData, _int iInvenIdx)
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = TWELFTH; // SIXTEENTH
	pDesc.wszTexture = pItemData->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = pItemData->Get_ItemNameText();
	m_wszItemExplain = pItemData->Get_ItemExplainText();

	m_iInventoryIdx = iInvenIdx;

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon_Weapon()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = NINETH;

	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize() - 1; ++i)
		++weapon;
	pDesc.wszTexture = (*weapon)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = (*weapon)->Get_ItemNameText();
	m_wszItemExplain = (*weapon)->Get_ItemExplainText();

	// 여기서 Symbol Icon도 생성해주기
	pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
	pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
	pDesc.fSizeX = 160.f;
	pDesc.fSizeY = 160.f;
	m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon_Skill()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = NINETH; // ?

	vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize() - 1; ++i)
		++skill;
	pDesc.wszTexture = (*skill)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = (*skill)->Get_ItemNameText();
	m_wszItemExplain = (*skill)->Get_ItemExplainText();

	// 여기서 Symbol Icon도 생성해주기
	pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
	pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
	pDesc.fSizeX = 160.f;
	pDesc.fSizeY = 160.f;
	m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

HRESULT CUI_Slot::Change_ItemIcon_Weapon()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = NINETH; // ?

	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	
	for (size_t i = 0; i < m_iSlotIdx; ++i)
		++weapon;
	pDesc.wszTexture = (*weapon)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = (*weapon)->Get_ItemNameText();
	m_wszItemExplain = (*weapon)->Get_ItemExplainText();

	Check_Equip(true, *weapon);

	// SymbolIcon
	pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
	pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
	pDesc.fSizeX = 160.f;
	pDesc.fSizeY = 160.f;
	m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

HRESULT CUI_Slot::Change_ItemIcon_Skill()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = NINETH; // ?

	vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
	for (size_t i = 0; i < m_iSlotIdx; ++i)
		++skill;
	pDesc.wszTexture = (*skill)->Get_TextureName();
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	m_wszItemName = (*skill)->Get_ItemNameText();
	m_wszItemExplain = (*skill)->Get_ItemExplainText();

	Check_Equip(false, *skill);

	// SymbolIcon
	pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
	pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
	pDesc.fSizeX = 160.f;
	pDesc.fSizeY = 160.f;
	m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	// RedDot 처리도 해주기
	if ((*skill)->Get_isRedDotUse())
	{
		m_pItemIcon->Create_RedDot();
		(*skill)->Set_isRedDotUse(false);
	}

	return S_OK;
}

HRESULT CUI_Slot::Pull_ItemIcon(_bool isEquip, wstring wstrTexture, wstring wstrItemName, wstring wstrItemExplain, wstring wstrItemExplain_Quick)
{
	// !!!!!!!!!!!!!!!!!!! 여기서 뭔가 예외 처리가 잘못 되었는지 한번씩 안 당겨질 때가 있는?

	// 다음 Slot의 정보를 현재 Slot에 담는 함수
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = ELEVENTH;
	pDesc.wszTexture = wstrTexture;
	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));
	
	m_wstrTexture = wstrTexture;
	m_wszItemName = wstrItemName;
	m_wszItemExplain = wstrItemExplain;

	m_wszItemExplain_Quick = wstrItemExplain_Quick;

	if (SLOT_INV == m_eSlotType)
	{
		// 여기서 Symbol Icon도 생성해주기
		pDesc.fX = (g_iWinSizeX >> 1) + 380.f;
		pDesc.fY = (g_iWinSizeY >> 1) - 100.f;
		pDesc.fSizeX = 160.f;
		pDesc.fSizeY = 160.f;
		m_pSymbolIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));
	}

	m_isEquip = isEquip;

	return S_OK;
}

HRESULT CUI_Slot::Delete_ItemIcon()
{
	Safe_Release(m_pItemIcon);
	m_pItemIcon = nullptr;

	Safe_Release(m_pSymbolIcon);
	m_pSymbolIcon = nullptr;

	m_isEquip = false;

	return S_OK;
}

void CUI_Slot::Click_BtnEvent()
{
	if (SLOT_INV == m_eSlotType) // 인벤토리에 있는 슬롯을 클릭한 경우
	{
		// 장착 가능한 아이템의 경우에만 Quick에 등록
		if (CItemData::ITEMTYPE_BUFF == CInventory::GetInstance()->Get_ItemData(m_iSlotIdx)->Get_ItemType()
			|| CItemData::ITEMTYPE_USABLE == CInventory::GetInstance()->Get_ItemData(m_iSlotIdx)->Get_ItemType()
			|| CItemData::ITEMTYPE_ETC == CInventory::GetInstance()->Get_ItemData(m_iSlotIdx)->Get_ItemType())
		{
			dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Set_SlotIdx(m_iSlotIdx);
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_AnimFinished(false);
			CUI_Manager::GetInstance()->Render_UIGroup(true, "InvSub");
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_RenderOnAnim(true);
		}
	}
	else if (SLOT_WEAPON == m_eSlotType) // Weapon에 있는 슬롯을 클릭한 경우
	{
		// Alpha BG 활성화
		dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Set_CurSlotIdx(m_iSlotIdx);
		dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Set_EquipMode(true);
	}
	else if (SLOT_QUICK == m_eSlotType) // Quick Acess의 슬롯을 클릭한 경우
	{
		if (nullptr == m_pItemIcon) // ItemIcon이 없으면 InvSlot 활성화
		{
			dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Set_isInvSlot_Act(true);
		}
		else // 있으면 장착 해제
		{
			Delete_ItemIcon(); // Quick Acess Slot에서 ItemIcon 해제
			
			// Quick Acess InvSlot에서 Equip Sign 해제
			dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_EquipSign(m_iInventoryIdx, false);

			// Inventory에서 EqupiSign 해제 / Inv의 SubQuick에서도 제거
			dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Slot_EquipSign(m_iInventoryIdx, false);
			dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Delete_InvSub_QuickSlot_ToInvIdx(m_iInventoryIdx);

			// Inventory에서 장착 여부 비활성화 해주기
			CInventory::GetInstance()->Delete_QuickAccess(m_iInventoryIdx, m_iSlotIdx, m_wszItemName);

			// 인덱스 갱신..?
			dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotIdx(CUIGroup_WeaponSlot::SLOT_QUICK);

			// HUD에서도 출력 제거 
			dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_QUICK);
		}
	}
	else if (SLOT_QUICKINV == m_eSlotType) // Quick Acess의 InvSlot을 클릭한 경우
	{
		CItemData* pItem = CInventory::GetInstance()->Get_ItemData(m_iSlotIdx);

		// 장착 가능한 아이템만 Quick에 등록
		if (CItemData::ITEMTYPE_BUFF == pItem->Get_ItemType()
			|| CItemData::ITEMTYPE_USABLE == pItem->Get_ItemType()
			|| CItemData::ITEMTYPE_ETC == pItem->Get_ItemType())
		{
			if (!pItem->Get_isEquip())
			{
				CInventory::GetInstance()->Add_QuickAccess(pItem, m_iSlotIdx);

				// Equip Sign 활성화
				pItem->Set_isEquip(true);
				dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Slot_EquipSign(m_iSlotIdx, true);

				// Quick Acess의 InvSlot도 Equip Sign 활성화
				dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_EquipSign(m_iSlotIdx, true);

				// Quick에 Item을 추가한 순간 Quick~ size에 맞게 m_iQuickIdx 초기화 (Skill에도 필요할 거 같음)
				dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotIdx(CUIGroup_WeaponSlot::SLOT_QUICK);
			}
		}
	}
}

void CUI_Slot::Render_Font()
{
	if (SLOT_INV == m_eSlotType || SLOT_WEAPON == m_eSlotType)
	{
		wstring wstrTitle = m_wszItemName;

		if (SLOT_WEAPON == m_eSlotType)
		{
			// Weapon의 경우에만 해야함 Skill의 경우에는 X
			if (CUIGroup_Weapon::TAB_L == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
			{
				// Level
				vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
				for (size_t i = 0; i < m_iSlotIdx; ++i)
					++weapon;
				if (0 < (*weapon)->Get_Level())
				{
					wstrTitle = m_wszItemName + TEXT("   + ") + to_wstring((*weapon)->Get_Level());
				}
			}
		}

		// Title
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), wstrTitle, _float2((g_iWinSizeX >> 1) + 50.f, 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return;

		// Explain
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight13"), m_wszItemExplain, _float2((g_iWinSizeX >> 1) + 50.f, (g_iWinSizeY >> 1) - 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return; 
	}
	else if (SLOT_QUICKINV == m_eSlotType)
	{
		// Title
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_wszItemName, _float2(170.f, g_iWinSizeY - 185.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return;

		// Explain
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight13"), m_wszItemExplain_Quick, _float2(180.f, g_iWinSizeY - 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return;
	}
}

void CUI_Slot::Rend_Count()
{
	// m_iSlotIdx를 활용 
	
	// >>> 여기서 터짐
	CItemData* pItem = CInventory::GetInstance()->Get_ItemData(m_iSlotIdx);

	if (nullptr == pItem)
		return;
	//CItemData* pItem = CInventory::GetInstance()->Get_ItemData_ByName(m_wszItemName);

	_uint iCount = pItem->Get_Count();

	if (!Check_GroupRenderOnAnim())
		return;

	if ((CItemData::ITEMNAME_CATALYST != pItem->Get_ItemName()) && iCount >= 2)
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), to_wstring(iCount), _float2(m_fX + 17.f, m_fY + 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f)))) // m_fX + 10.f, m_fY + 10.f
			return;
	}
	else if (CItemData::ITEMNAME_CATALYST == pItem->Get_ItemName())
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), to_wstring(iCount), _float2(m_fX + 17.f, m_fY + 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f)))) // m_fX + 10.f, m_fY + 10.f
			return;
	}
}

_bool CUI_Slot::Check_GroupRenderOnAnim()
{
	// 현재 Slot이 속한 UIGroup의 RenderOnAnim의 여부를 반환하는 함수

	string strUIGroup = "";

	switch (m_eSlotType)
	{
	case Client::CUI_Slot::SLOT_QUICK:
	case Client::CUI_Slot::SLOT_QUICKINV:
		strUIGroup = "Quick";
		break;
	case Client::CUI_Slot::SLOT_INV:
		strUIGroup = "Inventory";
		break;
	case Client::CUI_Slot::SLOT_WEAPON:
	case Client::CUI_Slot::SLOT_INVSUB:
		strUIGroup = "Weapon";
		break;
	default:
		break;
	}

	return 	CUI_Manager::GetInstance()->Get_UIGroup(strUIGroup)->Get_RenderOnAnim();
}

CUI_Slot* CUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Slot* pInstance = new CUI_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Slot::Clone(void* pArg)
{
	CUI_Slot* pInstance = new CUI_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pRedDot);
	Safe_Release(m_pEquipSign);
	Safe_Release(m_pSymbolIcon);
	Safe_Release(m_pItemIcon);
	Safe_Release(m_pSelectFrame);
}

#include "UI_Manager.h"

#include "GameInstance.h"
#include "ItemData.h"

#include "UIGroup_Logo.h"
#include "UIGroup_Loading.h"
#include "UIGroup_State.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Menu.h"
#include "UIGroup_Quick.h"
#include "UIGroup_Character.h"
#include "UIGroup_Inventory.h"
#include "UIGroup_Weapon.h"
#include "UIGroup_InvSub.h"
#include "UIGroup_Upgrade.h"
#include "UIGroup_UpGPage.h"
#include "UIGroup_UP_Completed.h"
#include "UIGroup_Map.h"
#include "UIGroup_Setting.h"
#include "UIGroup_Ch_Upgrade.h"
#include "UIGroup_BuffTimer.h"
#include "UIGroup_Level.h"
#include "UIGroup_BossText.h"

#include "UI_ScreenBlood.h"
#include "UI_Broken.h"
#include "UI_Dash.h"
#include "QTE.h"
#include "UI_FadeInOut.h"
#include "UI_Cinematic.h"
#include "UI_PhaseChange.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CUI_Manager::Set_MenuPage(_bool isOpen, string PageKey)
{
	// render true false  조절
	map<string, CUIGroup*>::iterator iter = m_mapUIGroup.find(PageKey);
	(*iter).second->Set_Rend(isOpen);
	(*iter).second->Set_RenderOnAnim(true);
}

void CUI_Manager::Set_ScreenBloodRend(_bool isRend)
{
	m_pScreenBlood->Set_Rend(isRend);
	m_pScreenBlood->Resset_Animation(true);
}

void CUI_Manager::Set_Broken(_bool isRend)
{
	m_pBroken->Set_Rend(isRend);
	m_pBroken->Resset_Animation(true);
}

_bool CUI_Manager::Get_Dash()
{
	vector<CUI_Dash*>::iterator dash = m_vecDash.begin();

	return (*dash)->Get_Rend();
}

void CUI_Manager::Set_Dash(_bool isRend)
{
	vector<CUI_Dash*>::iterator dash = m_vecDash.begin();

	(*dash)->Set_Rend(isRend);
	(*dash)->Resset_Animation(true);

	++dash;
	(*dash)->Set_Rend(isRend);
	(*dash)->Resset_Animation(false);

	/*m_pDash->Set_Rend(isRend);
	m_pDash->Resset_Animation(true);*/
}

_bool CUI_Manager::Get_isMouseOn()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	map<string, CUIGroup*>::iterator logo = m_mapUIGroup.find("Logo");
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	map<string, CUIGroup*>::iterator upgrade = m_mapUIGroup.find("Upgrade"); 
	map<string, CUIGroup*>::iterator ch_upgrade = m_mapUIGroup.find("Ch_Upgrade");

	 if ((*menu).second->Get_Rend() || (*quick).second->Get_Rend() || (*upgrade).second->Get_Rend() || (*logo).second->Get_Rend() || (*ch_upgrade).second->Get_Rend()
		 || m_isShopOn)
		return true;
	else
		return false;
}

_bool CUI_Manager::Get_MenuPageState()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	return dynamic_cast<CUIGroup_Menu*>((*menu).second)->Get_MenuPageState();
}

void CUI_Manager::Set_MenuPageOpen()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	dynamic_cast<CUIGroup_Menu*>((*menu).second)->Set_MenuPageState(true);
}

CUIGroup* CUI_Manager::Get_UIGroup(string strKey)
{
	map<string, CUIGroup*>::iterator group = m_mapUIGroup.find(strKey);
	return (*group).second;
}

void CUI_Manager::Tick(_float fTimeDelta)
{
	for (auto& pGroup : m_mapUIGroup)
		pGroup.second->Tick(fTimeDelta);

	m_pScreenBlood->Tick(fTimeDelta);
	m_pBroken->Tick(fTimeDelta);

	for (auto& pDash : m_vecDash)
		pDash->Tick(fTimeDelta);

	if (nullptr != m_pQTE)
		m_pQTE->Tick(fTimeDelta);

	if (nullptr != m_pFadeOut)
		m_pFadeOut->Tick(fTimeDelta);

	if (nullptr != m_pFadeIn)
	{
		if (m_pFadeIn->Get_isFadeAnimEnd())
			Delete_FadeInOut(true);
		else
			m_pFadeIn->Tick(fTimeDelta);
	}

	if (nullptr != m_pPhaseChangeOut)
	{
		if (m_pPhaseChangeOut->Get_isFadeAnimEnd())
			Delete_PhaseChange(false);
		else
			m_pPhaseChangeOut->Tick(fTimeDelta);
	}

	if (nullptr != m_pPhaseChangeIn)
	{
		if (m_pPhaseChangeIn->Get_isFadeAnimEnd())
			Delete_PhaseChange(true);
		else
			m_pPhaseChangeIn->Tick(fTimeDelta);
	}


	if (nullptr != m_pCinematic)
		m_pCinematic->Tick(fTimeDelta);

	for (auto& pPortal : m_vecPortal)
	{
		pPortal->Tick(fTimeDelta);
	}

	vector<CUIGroup_Portal*>::iterator portal = m_vecPortal.begin();
	for (size_t i = 0; i < m_vecPortal.size(); ++i)
	{
		if ((*portal)->Get_isFadeEnd())
		{
			Safe_Release(*portal);
			portal = m_vecPortal.erase(portal);
		}
		else
			++portal;
	}
}

void CUI_Manager::Late_Tick(_float fTimeDelta)
{
	// 상점 이용 중일 때는 예외 처리 되어야 함. Upgrade 할 때도!
	if (m_isKeyActivate)
	{
		Key_Input();
	}

	for (auto& pGroup : m_mapUIGroup)
		pGroup.second->Late_Tick(fTimeDelta);

	m_pScreenBlood->Late_Tick(fTimeDelta);
	m_pBroken->Late_Tick(fTimeDelta);

	for (auto& pDash : m_vecDash)
		pDash->Late_Tick(fTimeDelta);

	if (nullptr != m_pQTE)
		m_pQTE->Late_Tick(fTimeDelta);

	if (nullptr != m_pFadeOut)
		m_pFadeOut->Late_Tick(fTimeDelta);

	if (nullptr != m_pFadeIn)
		m_pFadeIn->Late_Tick(fTimeDelta);

	if (nullptr != m_pCinematic)
		m_pCinematic->Late_Tick(fTimeDelta);

	for (auto& pPortal : m_vecPortal)
		pPortal->Late_Tick(fTimeDelta);

	if (nullptr != m_pPhaseChangeIn)
		m_pPhaseChangeIn->Late_Tick(fTimeDelta);

	if (nullptr != m_pPhaseChangeOut)
		m_pPhaseChangeOut->Late_Tick(fTimeDelta);
}

void CUI_Manager::Render_UIGroup(_bool isRender, string strKey)
{
	map<string, CUIGroup*>::iterator uigroup = m_mapUIGroup.find(strKey);
	(*uigroup).second->Set_Rend(isRender);
}

void CUI_Manager::Update_Inventory_Add(_uint iSlotIdx)
{
	map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
	dynamic_cast<CUIGroup_Inventory*>((*inventory).second)->Update_Inventory_Add(iSlotIdx);

}

void CUI_Manager::Update_InvSub_Quick_Add(_uint iSlotIdx)
{
	map<string, CUIGroup*>::iterator invsubQuick = m_mapUIGroup.find("InvSub");
	dynamic_cast<CUIGroup_InvSub*>((*invsubQuick).second)->Update_InvSub_QuickSlot(iSlotIdx);
}

void CUI_Manager::Update_Quick_Add(CItemData* pItemData, _int iInvenIdx)
{
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	dynamic_cast<CUIGroup_Quick*>((*quick).second)->Update_QuickSlot_Add(pItemData, iInvenIdx);
}

void CUI_Manager::Update_Quick_InvSlot_Add(_uint iSlotIdx)
{
	map<string, CUIGroup*>::iterator quickInvSlot = m_mapUIGroup.find("Quick");
	dynamic_cast<CUIGroup_Quick*>((*quickInvSlot).second)->Update_InvSlot_Add(iSlotIdx);
}

void CUI_Manager::Update_Weapon_Add()
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_Weapon_Add();
}

void CUI_Manager::Update_EquipWeapon_Add(_uint iEquipSlotIdx)
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_EquipSlot_Add(iEquipSlotIdx);
}

void CUI_Manager::Update_EquipWeapon_Delete(_uint iEquipSlotIdx)
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_EquipSlot_Delete(iEquipSlotIdx);
}

void CUI_Manager::Update_Skill_Add()
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_Skill_Add();
}

void CUI_Manager::Update_EquipSkill_Add(_uint iEquipSlotIdx)
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	//dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_EquipSl
}

void CUI_Manager::Update_EquipSkill_Delete(_uint iEquipSlotIdx) // 이거 같아도 됨?
{
	map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
	dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Update_EquipSlot_Delete(iEquipSlotIdx);
}

void CUI_Manager::Resset_Player()
{
	// Player를 참조 중인 녀석들의 값을 초기화
	map<string, CUIGroup*>::iterator hud = m_mapUIGroup.find("HUD_State");
	dynamic_cast<CUIGroup_State*>((*hud).second)->Resset_Player();
}

_bool CUI_Manager::Get_isUIOpen()
{
	// !!!!!!!!!!!!!!!!!!!! 여기서 Shop에 대한 처리도 해야 하지 않을 지 !!!!!!!!!!!!!!!!!!!!
	

	// UI가 켜져있는지 활성화에 대한 여부를 반환하는 함수
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	map<string, CUIGroup*>::iterator ch_upgrade = m_mapUIGroup.find("Ch_Upgrade");
	map<string, CUIGroup*>::iterator upgrade = m_mapUIGroup.find("Upgrade");

	_bool isMenuOpen = (*menu).second->Get_Rend();
	_bool isQuickOpen = (*quick).second->Get_Rend();
	_bool isChUpgradeOpen = (*ch_upgrade).second->Get_Rend();
	_bool isUpgradeOpen = (*upgrade).second->Get_Rend();

	if (isMenuOpen || isQuickOpen || isChUpgradeOpen || isUpgradeOpen)
		return true;
	else
		return false;

	//Test
}

HRESULT CUI_Manager::Initialize()
{
	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Create_UI()
{
	CUIGroup::UIGROUP_DESC pDesc;

	pDesc.eLevel = LEVEL_STATIC;

	// Logo
	m_mapUIGroup.emplace("Logo", dynamic_cast<CUIGroup_Logo*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Logo"), &pDesc)));

	// Loading
	m_mapUIGroup.emplace("Loading", dynamic_cast<CUIGroup_Loading*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Loading"), &pDesc)));

	// State
	m_mapUIGroup.emplace("HUD_State", dynamic_cast<CUIGroup_State*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_State"), &pDesc)));

	// WeaponSlot
	m_mapUIGroup.emplace("HUD_WeaponSlot", dynamic_cast<CUIGroup_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_WeaponSlot"), &pDesc)));

	// Menu
	m_mapUIGroup.emplace("Menu", dynamic_cast<CUIGroup_Menu*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Menu"), &pDesc)));

	// Quick 
	m_mapUIGroup.emplace("Quick", dynamic_cast<CUIGroup_Quick*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Quick"), &pDesc)));

	// Character 
	m_mapUIGroup.emplace("Menu_Ch", dynamic_cast<CUIGroup_Character*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Character"), &pDesc)));

	// Inventory
	m_mapUIGroup.emplace("Inventory", dynamic_cast<CUIGroup_Inventory*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Inventory"), &pDesc)));

	// Weapon 
	m_mapUIGroup.emplace("Weapon", dynamic_cast<CUIGroup_Weapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Weapon"), &pDesc)));

	// Map
	m_mapUIGroup.emplace("Map", dynamic_cast<CUIGroup_Map*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Map"), &pDesc)));

	// Inv Sub 
	m_mapUIGroup.emplace("InvSub", dynamic_cast<CUIGroup_InvSub*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_InvSub"), &pDesc)));

	// Upgrade
	m_mapUIGroup.emplace("Upgrade", dynamic_cast<CUIGroup_Upgrade*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Upgrade"), &pDesc)));

	// UpGPage
	m_mapUIGroup.emplace("UpGPage", dynamic_cast<CUIGroup_UpGPage*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage"), &pDesc)));

	// Setting  
	m_mapUIGroup.emplace("Setting", dynamic_cast<CUIGroup_Setting*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Setting"), &pDesc)));
	
	// Ch_Upgrade
	m_mapUIGroup.emplace("Ch_Upgrade", dynamic_cast<CUIGroup_Ch_Upgrade*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade"), &pDesc)));

	// BuffTimer
	m_mapUIGroup.emplace("BuffTimer", dynamic_cast<CUIGroup_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_BuffTimer"), &pDesc)));

	// ScreenBlood
	CUI::UI_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;
	m_pScreenBlood = dynamic_cast<CUI_ScreenBlood*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ScreenBlood"), &pUIDesc));

	// Broken   
	m_pBroken = dynamic_cast<CUI_Broken*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Broken"), &pUIDesc));

	// Dash
	CUI_Dash::UI_DASH_DESC pDashDesc{};
	pDashDesc.eLevel = LEVEL_STATIC;
	for (size_t i = 0; i < 2; ++i)
	{
		pDashDesc.iDashIdx = i;
		m_vecDash.emplace_back(dynamic_cast<CUI_Dash*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Dash"), &pDashDesc)));
	}

	// Cinematic
	m_pCinematic = dynamic_cast<CUI_Cinematic*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Cinematic"), &pUIDesc));

	return S_OK;
}

HRESULT CUI_Manager::Create_FadeInOut_Dissolve(_bool isFadeIn)
{
	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};
	pDesc.isLevelChange = false;
	pDesc.eFadeType = CUI_FadeInOut::TYPE_DISSOLVE;

	if (isFadeIn)
	{
		if (nullptr != m_pFadeIn)
			return S_OK;

		pDesc.isFadeIn = true;

		m_pFadeIn = dynamic_cast<CUI_FadeInOut*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc));
		if (nullptr == m_pFadeIn)
			return E_FAIL;
	}
	else if(!isFadeIn/* && nullptr == m_pFadeIn*/)
	{
		if (nullptr != m_pFadeOut)
			return S_OK;

		pDesc.isFadeIn = false;

		m_pFadeOut = dynamic_cast<CUI_FadeInOut*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc));
		if (nullptr == m_pFadeOut)
			return E_FAIL;
	}

	return S_OK;
}

void CUI_Manager::Delete_FadeInOut(_bool isFadeIn)
{
	if (isFadeIn)
	{
		Safe_Release(m_pFadeIn);
		m_pFadeIn = nullptr;
	}
	else
	{
		Safe_Release(m_pFadeOut);
		m_pFadeOut = nullptr;
	}
}

_bool CUI_Manager::Get_isFadeAnimEnd(_bool isFadeIn)
{
	if (isFadeIn)
	{
		if (nullptr == m_pFadeIn)
			return false;
		else
			return m_pFadeIn->Get_isFadeAnimEnd();
	}
	else
	{
		if (nullptr == m_pFadeOut)
			return false;
		else
			return m_pFadeOut->Get_isFadeAnimEnd();
	}
}

HRESULT CUI_Manager::Create_RedDot_MenuBtn(_bool isInv)
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");

	if (FAILED(dynamic_cast<CUIGroup_Menu*>((*menu).second)->Create_RedDot_MenuBtn(isInv)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Delete_RedDot_MenuBtn(_bool isInv)
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	dynamic_cast<CUIGroup_Menu*>((*menu).second)->Delete_RedDot_MenuBtn(isInv);

	return S_OK;
}

HRESULT CUI_Manager::Create_RedDot_Slot(_bool isInv, _uint iSlotIdx, _bool isSkill)
{
	if (isInv)
	{
		// Inventory
		map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
		dynamic_cast<CUIGroup_Inventory*>((*inventory).second)->Create_RedDot(iSlotIdx);

		// Quiuck
		map<string, CUIGroup*>::iterator quickaccess = m_mapUIGroup.find("Quick");
		dynamic_cast<CUIGroup_Quick*>((*quickaccess).second)->Create_RedDot(iSlotIdx);
	}
	else
	{
		map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");

		if (!isSkill) // Weapon인 경우
		{
			dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Create_RedDot(iSlotIdx);
		}
		else // Skill인 경우
		{
			dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Create_RedDot(iSlotIdx, true);
		}
	}
	return S_OK;
}

HRESULT CUI_Manager::Delete_RedDot_Slot(_bool isInv)
{
	if (isInv)
	{
		// Inventory
		map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
		dynamic_cast<CUIGroup_Inventory*>((*inventory).second)->Delete_RedDot();

		// Quick
		map<string, CUIGroup*>::iterator quickaccess = m_mapUIGroup.find("Quick");
		dynamic_cast<CUIGroup_Quick*>((*quickaccess).second)->Delete_RedDot();
	}

	return S_OK;
}

void CUI_Manager::Set_RedDot_Rend(_bool isRend)
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	dynamic_cast<CUIGroup_Menu*>((*menu).second)->Set_RedDot_Rend(isRend);
}

void CUI_Manager::Create_QTE()
{
	CQTE::GAMEOBJECT_DESC pQteDesc{};

	if (nullptr != m_pQTE)
	{
		Safe_Release(m_pQTE);
		m_pQTE = nullptr;
	}

	m_pQTE = dynamic_cast<CQTE*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_QTE"), &pQteDesc));
}

_bool CUI_Manager::Delete_QTE()
{
	if (nullptr == m_pQTE)
		return false; // 이렇게 하면 될지;

	_bool isSuccess = m_pQTE->Check_ResultScore();

	Safe_Release(m_pQTE);
	m_pQTE = nullptr;

	return isSuccess;
}

_bool CUI_Manager::Check_End_QTE()
{
	return m_pQTE->Check_End();
}

_bool CUI_Manager::Get_isQTEAlive()
{
	// QTE 존재 여부 반환
	if (nullptr == m_pQTE)
		return false;
	else
		return true;
}

void CUI_Manager::Create_LevelUI()
{
	if (m_mapUIGroup.find("Level") != m_mapUIGroup.end()) // 값이 있으면
	{
		// erase를 해야 할듯
		Safe_Release((*m_mapUIGroup.find("Level")).second);
		m_mapUIGroup.erase("Level");
	}

	// UI Level을 생성
	CUIGroup::UIGROUP_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	m_mapUIGroup.insert({"Level", dynamic_cast<CUIGroup_Level*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Level"), &pDesc))});
}

void CUI_Manager::Setting_Cinematic(_bool isBigAnim)
{
	// true : 시작 / false : 끝 >> 반대??? 헷갈티비

	m_pCinematic->Set_isBigAim(isBigAnim);

	// 아래 이거 때문에 QTE시 깜박거리는 듯함!
	if (!isBigAnim) // 시네마틱 시작
		m_mapUIGroup.find("HUD_WeaponSlot")->second->Set_Rend(false);
	else
		m_mapUIGroup.find("HUD_WeaponSlot")->second->Set_Rend(true);
}

void CUI_Manager::Create_PortalUI(CUIGroup_Portal::UIGROUP_PORTAL_DESC* pDesc)
{
	m_vecPortal.emplace_back(dynamic_cast<CUIGroup_Portal*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Portal"), pDesc)));
}

void CUI_Manager::Delete_PortalUI(_bool isBackPortal)
{
	vector<CUIGroup_Portal*>::iterator portal = m_vecPortal.begin();

	for (size_t i = 0; i < m_vecPortal.size(); ++i)
	{
		// 여기서 Pic이 있는 녀석인지 체크하고? 
		if ((*portal)->Get_isPic())
		{
			((*portal)->Set_FadeOut());
		}
		else
		{
			Safe_Release(*portal);
			portal = m_vecPortal.erase(portal);
		}
	}
}

void CUI_Manager::Create_PhaseChange(_bool isFadeIn)
{
	CUI_PhaseChange::UI_PHASECHANGE_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	if (isFadeIn)
	{
		if (nullptr != m_pPhaseChangeIn)
			return;

		pDesc.isFadeIn = true;

		m_pPhaseChangeIn = dynamic_cast<CUI_PhaseChange*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_PhaseChange"), &pDesc));
	}
	else
	{
		if (nullptr != m_pPhaseChangeOut)
			return;

		pDesc.isFadeIn = false;

		m_pPhaseChangeOut = dynamic_cast<CUI_PhaseChange*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_PhaseChange"), &pDesc));
	}
}

void CUI_Manager::Delete_PhaseChange(_bool isFadeIn)
{
	if (isFadeIn)
	{
		Safe_Release(m_pPhaseChangeIn);
		m_pPhaseChangeIn = nullptr;
	}
	else
	{
		Safe_Release(m_pPhaseChangeOut);
		m_pPhaseChangeOut = nullptr;
	}
}

_bool CUI_Manager::Get_isPhaseChange_AnimEnd(_bool isFadeIn)
{
	if (isFadeIn)
	{
		if (nullptr == m_pPhaseChangeIn)
			return false;
		else
			return m_pPhaseChangeIn->Get_isFadeAnimEnd();
	}
	else
	{
		if (nullptr == m_pPhaseChangeOut)
			return false;
		else
			return m_pPhaseChangeOut->Get_isFadeAnimEnd();
	}
}

void CUI_Manager::Create_BossText(_bool isCreateText)
{
	if (m_mapUIGroup.find("BossText") != m_mapUIGroup.end()) // 값이 있으면
	{
		// erase를 해야 할듯
		Safe_Release((*m_mapUIGroup.find("BossText")).second);
		m_mapUIGroup.erase("BossText");
	}

	// UI BossText를 생성
	CUIGroup_BossText::UIGROUP_BOSSTEXT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.isCreateText = isCreateText;

	m_mapUIGroup.insert({"BossText", dynamic_cast<CUIGroup_BossText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_BossText"), &pDesc)) });
}

void CUI_Manager::Key_Input()
{
	// m_isShopOn에 대한 분기 처리 해야 하지 않을지?

	map<string, CUIGroup*>::iterator logo = m_mapUIGroup.find("Logo");
	map<string, CUIGroup*>::iterator loading = m_mapUIGroup.find("Loading");
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	map<string, CUIGroup*>::iterator invsub = m_mapUIGroup.find("InvSub");

	_bool isLogoOpen = (*logo).second->Get_Rend();
	_bool isLoadingOpen = (*loading).second->Get_Rend();
	_bool isMenuOpen = (*menu).second->Get_Rend();
	_bool isQuickOpen = (*quick).second->Get_Rend();
	_bool isInvSubOpen = (*invsub).second->Get_Rend();

	if (isLogoOpen || isLoadingOpen)
		return;

	if (isMenuOpen) // Menu가 켜져 있을 때
	{
		if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		{
			map<string, CUIGroup*>::iterator character = m_mapUIGroup.find("Menu_Ch");
			map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
			map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");
			map<string, CUIGroup*>::iterator mapPage = m_mapUIGroup.find("Map");
			map<string, CUIGroup*>::iterator setting = m_mapUIGroup.find("Setting");

			_bool isChOpen = (*character).second->Get_Rend();
			_bool isInvOpen = (*inventory).second->Get_Rend();
			_bool isWeaponOpen = (*weapon).second->Get_Rend();
			_bool isMapOpen = (*mapPage).second->Get_Rend();
			_bool isSettingOpen = (*setting).second->Get_Rend();

			if (!isInvSubOpen)
			{
				if (isChOpen)
				{
					(*character).second->Set_RenderOnAnim(false);
					Set_RedDot_Rend(true);
				}
				else if (isInvOpen)
				{
					(*inventory).second->Set_RenderOnAnim(false);
					Delete_RedDot_Slot(true); // RedDot 제거
				}
				else if (isWeaponOpen)
				{
					// weapon의 sub도 활성화 되어있지 않을 때 예외 처리 필요
					if (!dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Get_EquipMode())
					{
						(*weapon).second->Set_RenderOnAnim(false);
						dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Reset_Tab(); // Tab 상태 초기화

						// Skill에 RedDot이 하나라도 있는 게 아니라면 Menu Btn의 RedDot 제거
						if (!dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Check_RedDot())
						{
							CUI_Manager::GetInstance()->Delete_RedDot_MenuBtn(false);
						}

						dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Delete_RedDot();
					}
					else
						dynamic_cast<CUIGroup_Weapon*>((*weapon).second)->Set_EquipMode(false);
				}
				else if (isMapOpen)
				{
					(*mapPage).second->Set_RenderOnAnim(false);
					Set_RedDot_Rend(true);
				}
				else if (isSettingOpen)
				{
					(*setting).second->Set_RenderOnAnim(false);
					Set_RedDot_Rend(true);
				}
				else
				{
					(*menu).second->Set_RenderOnAnim(false);
					m_pGameInstance->Get_MainCamera()->Activate();
				}

				dynamic_cast<CUIGroup_Menu*>((*menu).second)->Set_MenuPageState(false);
			}
			else
			{
				(*invsub).second->Set_RenderOnAnim(false);

				Delete_RedDot_Slot(true); // RedDot 제거
			}
		}
	}
	else // Menu가 꺼져 있을 때
	{
		map<string, CUIGroup*>::iterator upgrade = m_mapUIGroup.find("Upgrade"); // Upgrade
		map<string, CUIGroup*>::iterator upgpage = m_mapUIGroup.find("UpGPage"); // Upgrade
		map<string, CUIGroup*>::iterator ch_upgrade = m_mapUIGroup.find("Ch_Upgrade");

		_bool isUpgradeOpen = (*upgrade).second->Get_Rend();
		_bool isUpgPageOpen = (*upgpage).second->Get_Rend();
		_bool isChUpgradeOpen = (*ch_upgrade).second->Get_Rend();


		if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		{
			// Quick이 열려있지 않을 때, Upgrade, UpPage, Ch Upgrade 가 열려있지 않을 때 Menu 열기
			if (!isQuickOpen && !isUpgradeOpen && !isUpgPageOpen && !isChUpgradeOpen)
			{
				(*menu).second->Set_Rend(true);
				(*menu).second->Set_RenderOnAnim(true);

				m_pGameInstance->Get_MainCamera()->Inactivate();
			}
			else if (isUpgradeOpen)
			{
				if (isUpgPageOpen)
				{
					(*upgpage).second->Set_RenderOnAnim(false);
				}
				else
				{
					(*upgrade).second->Set_RenderOnAnim(false);
				}

				m_pGameInstance->Get_MainCamera()->Inactivate();
			}
			else if (isChUpgradeOpen)
			{
				(*ch_upgrade).second->Set_RenderOnAnim(false);

				m_pGameInstance->Get_MainCamera()->Inactivate();
			}
		}
		else if (m_pGameInstance->Key_Down(DIK_I))
		{
			if (!isMenuOpen && !isUpgradeOpen && !isChUpgradeOpen)
			{
				(*quick).second->Set_AnimFinished(false);

				if (isQuickOpen) // 퀵슬롯이 켜져 있을 때 > 꺼지게
				{
					(*quick).second->Set_RenderOnAnim(false);

					m_pGameInstance->Get_MainCamera()->Activate();

					// MenuBtn도 사라져야죵
					CUI_Manager::GetInstance()->Delete_RedDot_MenuBtn(true);
					CUI_Manager::GetInstance()->Delete_RedDot_Slot(true); // UI Inventory의 RedDot 제거
				}
				else // 꺼져있을 때 > 켜지게
				{
					(*quick).second->Set_Rend(true);
					(*quick).second->Set_RenderOnAnim(true);

					m_pGameInstance->Get_MainCamera()->Inactivate();
				}
			}
		}
		else if (m_pGameInstance->Key_Down(DIK_U) && !isChUpgradeOpen)
		{
			map<string, CUIGroup*>::iterator upgrade = m_mapUIGroup.find("Upgrade"); // Upgrade
			(*upgrade).second->Set_Rend(true);
			(*upgrade).second->Set_RenderOnAnim(true);

			m_pGameInstance->Get_MainCamera()->Inactivate();
		}
		else if (m_pGameInstance->Key_Down(DIK_Y) && !isUpgradeOpen)
		{
			map<string, CUIGroup*>::iterator ch_upgrade = m_mapUIGroup.find("Ch_Upgrade");
			(*ch_upgrade).second->Set_Rend(true);
			(*ch_upgrade).second->Set_RenderOnAnim(true);
			dynamic_cast<CUIGroup_Ch_Upgrade*>((*ch_upgrade).second)->Resset_OriginData();

			m_pGameInstance->Get_MainCamera()->Inactivate();
		}
		else if (m_pGameInstance->Key_Down(DIK_L)) // <<<<< Test용 (QTE)
		{
			// QTE 생성 Prototype_GameObject_QTE
			Create_QTE();
		}
	}


	// Test 키보드로 테스트
	if (m_pGameInstance->Key_Down(DIK_J))
	{
		//Setting_Cinematic(true); // 없애기 (FadeIn 하기) > true
		Create_BossText(false);
	}
}

void CUI_Manager::Free()
{
	for (auto& pair : m_mapUIGroup)
	{
		Safe_Release(pair.second);
	}
	m_mapUIGroup.clear();

	for (auto& pDash : m_vecDash)
		Safe_Release(pDash);

	for (auto& pPortal : m_vecPortal)
		Safe_Release(pPortal);
	
	Safe_Release(m_pPhaseChangeIn);
	Safe_Release(m_pPhaseChangeOut);
	Safe_Release(m_pCinematic);
	Safe_Release(m_pFadeIn);
	Safe_Release(m_pFadeOut);
	Safe_Release(m_pQTE);
	Safe_Release(m_pBroken);
	Safe_Release(m_pScreenBlood);
	Safe_Release(m_pGameInstance);
}

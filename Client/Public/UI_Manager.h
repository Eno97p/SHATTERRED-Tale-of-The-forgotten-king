#pragma once

#include "Base.h"
#include "Client_Defines.h"

#include "UIGroup_Portal.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIGroup;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	_bool				Get_MenuPageState();
	void				Set_MenuPageOpen();
	CUIGroup*			Get_UIGroup(string strKey);
	void				Set_MenuPage(_bool isOpen, string PageKey);
	void				Set_ScreenBloodRend(_bool isRend);
	void				Set_Broken(_bool isRend);
	_bool				Get_Dash();
	void				Set_Dash(_bool isRend);
	_bool				Get_isMouseOn();
	void				Set_KeyActivate(_bool isKeyActivate) { m_isKeyActivate = isKeyActivate; }

	void				Set_isShopOn(_bool isOn) { m_isShopOn = isOn; }
	_bool				Get_isUIOpen();

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

	void				Render_UIGroup(_bool isRender, string strKey);

	void				Update_Inventory_Add(_uint iSlotIdx);
	void				Update_InvSub_Quick_Add(_uint iSlotIdx);
	void				Update_Quick_Add(class CItemData* pItemData, _int iInvenIdx);
	void				Update_Quick_InvSlot_Add(_uint iSlotIdx);
	void				Update_Weapon_Add();
	void				Update_EquipWeapon_Add(_uint iEquipSlotIdx);
	void				Update_EquipWeapon_Delete(_uint iEquipSlotIdx);
	void				Update_Skill_Add();
	void				Update_EquipSkill_Add(_uint iEquipSlotIdx);
	void				Update_EquipSkill_Delete(_uint iEquipSlotIdx);

	void				Resset_Player();

	HRESULT				Create_FadeInOut_Dissolve(_bool isFadeIn);
	void				Delete_FadeInOut(_bool isFadeIn);
	_bool				Get_isFadeAnimEnd(_bool isFadeIn);

	// RedDot 관련 함수
	HRESULT				Create_RedDot_MenuBtn(_bool isInv);
	HRESULT				Delete_RedDot_MenuBtn(_bool isInv);
	HRESULT				Create_RedDot_Slot(_bool isInv, _uint iSlotIdx, _bool isSkill = false);
	HRESULT				Delete_RedDot_Slot(_bool isInv);
	void				Set_RedDot_Rend(_bool isRend);

	// QTE
	void				Create_QTE();
	_bool				Delete_QTE();
	_bool				Check_End_QTE();
	_bool				Get_isQTEAlive();

	// Level
	void				Create_LevelUI();

	// Cinematic
	void				Setting_Cinematic(_bool isBigAnim);


	// Portal
	void				Create_PortalUI(CUIGroup_Portal::UIGROUP_PORTAL_DESC* pDesc);
	void				Delete_PortalUI(_bool isBackPortal = false);

	// PhaseChange
	void				Create_PhaseChange(_bool isFadeIn);
	void				Delete_PhaseChange(_bool isFadeIn);
	_bool				Get_isPhaseChange_AnimEnd(_bool isFadeIn);

	// BossText
	void				Create_BossText(_bool isCreateText);


	// For.LEVEL //PSW
	void SetVisitLevel(LEVEL eLevel) { m_VsitLevel.insert(eLevel); }
	_bool isVisitLevel(LEVEL eLevel) { return { m_VsitLevel.find(eLevel) != m_VsitLevel.end() }; }
	void SetPrevLevel(LEVEL eLevel) { m_PrevLevel = eLevel; }
	LEVEL GetPrevLevel() { return m_PrevLevel; }


private:
	_bool								m_isKeyActivate = { true }; // 키보드와의 상호작용 활성화 여부
	_bool								m_isShopOn = { false }; // 상점 활성화 여부

	CGameInstance*						m_pGameInstance = { nullptr };

	map<string, CUIGroup*>				m_mapUIGroup;
	class CUI_ScreenBlood*				m_pScreenBlood = { nullptr };
	class CUI_Broken*					m_pBroken = { nullptr };
	vector<class CUI_Dash*>				m_vecDash;
	class CQTE*							m_pQTE = { nullptr };
	class CUI_FadeInOut*				m_pFadeOut = { nullptr };
	class CUI_FadeInOut*				m_pFadeIn = { nullptr };
	class CUI_Cinematic*				m_pCinematic = { nullptr };
	vector<class CUIGroup_Portal*>		m_vecPortal;
	class CUI_PhaseChange*				m_pPhaseChangeIn = { nullptr };
	class CUI_PhaseChange*				m_pPhaseChangeOut = { nullptr };


private:	/*For. VisitLevel*/	//Add. PSW
	set<LEVEL>							m_VsitLevel = { LEVEL_END };
	LEVEL								m_PrevLevel = LEVEL_END;
private:
	HRESULT		Create_UI();

	void		Key_Input();

public:
	virtual void Free() override;
};

END
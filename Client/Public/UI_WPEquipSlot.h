#pragma once

#include "UI_Interaction.h"

BEGIN(Client)
class CUI_ItemIcon;
class CUI_WPEquipNone;

class CUI_WPEquipSlot final : public CUI_Interaction
{
	const _float DEFAULT_Y = (g_iWinSizeY >> 1) + 150.f;
	const _float SELECT_Y = DEFAULT_Y - 30.f;

public:
	enum SLOT_NUM { NUM_ONE = 0, NUM_TWO, NUM_THREE, NUM_END };
	typedef struct UI_EquipSlot_Desc : public UI_DESC
	{
		SLOT_NUM		eSlotNum;
	}UI_EQUIPSLOT_DESC;

private:
	CUI_WPEquipSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WPEquipSlot(const CUI_WPEquipSlot& rhs);
	virtual ~CUI_WPEquipSlot() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	HRESULT			Create_ItemIcon(_bool isWeapon);
	HRESULT			Delete_ItemIcon();

	HRESULT			Change_ItemIcon(_bool isWeapon, _uint iSlotIdx);

private:
	SLOT_NUM			m_eSlotNum = { NUM_END };

	CUI_WPEquipNone*	m_pNoneFrame = { nullptr };
	CUI_ItemIcon*		m_pItemIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Frame();
	void	Setting_XY();

	void	Click_Event();
	_bool	Check_GroupRenderOnAnim();

public:
	static CUI_WPEquipSlot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
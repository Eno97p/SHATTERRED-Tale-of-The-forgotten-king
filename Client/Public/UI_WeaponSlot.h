#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_WeaponSlot final : public CUI
{
public:
	enum SLOT_NUM { SLOT_ONE, SLOT_TWO, SLOT_THREE, SLOT_END };

	typedef struct UI_WeaponSlot_Desc : public UI_Desc
	{
		SLOT_NUM	eSlotNum;
	}UI_WEAPONSLOT_DESC;

private:
	CUI_WeaponSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WeaponSlot(const CUI_WeaponSlot& rhs);
	virtual ~CUI_WeaponSlot() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	SLOT_NUM		m_eSlotNum = { SLOT_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_XY();
	HRESULT	Change_Texture();

public:
	static CUI_WeaponSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
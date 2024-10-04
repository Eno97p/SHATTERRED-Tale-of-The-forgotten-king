#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_Ch_UpgradeBtn final : public CUI_Interaction
{
public:
	enum ABILITY_TYPE { ABILITY_VITALITY, ABILITY_STAMINA, ABILITY_STRENGHT, ABILITY_MYSTICISM, ABILITY_KNOWLEDGE, ABILITY_END };
	typedef struct UI_ChUp_Btn_Desc : public UI_DESC
	{
		_bool			isPlus;
		ABILITY_TYPE	eAbilityType;

	}UI_CHUP_BTN_DESC;
private:
	CUI_Ch_UpgradeBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ch_UpgradeBtn(const CUI_Ch_UpgradeBtn& rhs);
	virtual ~CUI_Ch_UpgradeBtn() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool								m_isPlus = { false }; // + or -

	ABILITY_TYPE						m_eAbilityType = { ABILITY_END };

	class CUI_Ch_UpgradeBtn_Select*		m_pSelect = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Select();

	void	Rend_Font();
	void	Apply_BtnEvent();

public:
	static CUI_Ch_UpgradeBtn*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
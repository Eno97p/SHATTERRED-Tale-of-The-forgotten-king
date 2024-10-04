#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_Menu final : public CUIGroup
{
private:
	CUIGroup_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Menu(const CUIGroup_Menu& rhs);
	virtual ~CUIGroup_Menu() = default;

public:
	_bool				Get_MenuPageState() { return m_isMenuPageOpen; }
	void				Set_MenuPageState(_bool isOpen) { m_isMenuPageOpen = isOpen; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT	Create_RedDot_MenuBtn(_bool isInv);
	HRESULT	Delete_RedDot_MenuBtn(_bool isInv);

	void	Set_RedDot_Rend(_bool isRend);

private:
	_bool					m_isMenuPageOpen;

	vector<CUI*>				m_vecUI;
	vector<class CUI_MenuBtn*>	m_vecBtn;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Menu*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
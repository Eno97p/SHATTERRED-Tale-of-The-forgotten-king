#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Level_TextBox final : public CUI
{
private:
	CUI_Level_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Level_TextBox(const CUI_Level_TextBox& rhs);
	virtual ~CUI_Level_TextBox() = default;

public:
	_bool			Get_isDead() { return m_isDead; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isDead = { false };
	_float			m_fFontX = { 0.f };
	_float			m_fFontY = { 0.f };
	_float			m_fMoveTimer = { 0.f };
	_float			m_fColor = { 1.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	wstring	Setting_Text();

public:
	static CUI_Level_TextBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
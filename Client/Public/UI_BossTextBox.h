#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossTextBox final : public CUI
{
public:
	typedef struct UI_BossTextBox_Desc : public UI_DESC
	{
		_bool		isCreateText;
	}UI_BOSSTEXTBOX_DESC;

private:
	CUI_BossTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossTextBox(const CUI_BossTextBox& rhs);
	virtual ~CUI_BossTextBox() = default;

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
	_bool			m_isCreateText = { false };
	_float			m_fFontX = { 0.f };
	_float			m_fFontY = { 0.f };
	_float			m_fMoveTimer = { 0.f };
	_float			m_fColor = { 1.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	wstring	Setting_Text();

public:
	static CUI_BossTextBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
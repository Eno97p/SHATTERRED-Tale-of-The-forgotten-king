#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MenuPageTop final : public CUI
{
public:
	enum TOP_TYPE { TOP_INV, TOP_WEAPON, TOP_END };
	typedef struct UI_Top_Desc : public UI_DESC
	{
		TOP_TYPE	eTopType;
	}UI_TOP_DESC;

private:
	CUI_MenuPageTop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MenuPageTop(const CUI_MenuPageTop& rhs);
	virtual ~CUI_MenuPageTop() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	TOP_TYPE		m_eTopType = { TOP_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Render_MenuTitle();

public:
	static CUI_MenuPageTop*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
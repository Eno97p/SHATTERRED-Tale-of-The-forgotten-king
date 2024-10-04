#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateBar final : public CUI
{
	const _float ORIGIN_SIZEX = 768.f;
	const _float ORIGIN_X = 390.f;

public:
	enum BAR_TYPE { BAR_HP, BAR_ENERGY, BAR_ETHER, BAR_END };
	typedef struct UI_StateBar_Desc : public UI_DESC
	{
		BAR_TYPE	eBarType;
	}UI_STATEBAR_DESC;

private:
	CUI_StateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateBar(const CUI_StateBar& rhs);
	virtual ~CUI_StateBar() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	BAR_TYPE		m_eBarType = { BAR_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_XY();

public:
	static CUI_StateBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
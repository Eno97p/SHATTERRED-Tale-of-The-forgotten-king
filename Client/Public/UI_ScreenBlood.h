#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_ScreenBlood final : public CUI
{
private:
	CUI_ScreenBlood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScreenBlood(const CUI_ScreenBlood& rhs);
	virtual ~CUI_ScreenBlood() = default;

public:
	void			Set_Rend(_bool isRend) { m_isRend = isRend; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isRend = { false };
	_float			m_fAlphaTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_ScreenBlood*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
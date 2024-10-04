#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MenuFontaine final : public CUI
{
private:
	CUI_MenuFontaine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MenuFontaine(const CUI_MenuFontaine& rhs);
	virtual ~CUI_MenuFontaine() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MenuFontaine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
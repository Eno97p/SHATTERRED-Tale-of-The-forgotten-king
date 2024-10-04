#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LogoTitle final : public CUI
{
private:
	CUI_LogoTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LogoTitle(const CUI_LogoTitle& rhs);
	virtual ~CUI_LogoTitle() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool		m_isInv = { false };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_LogoTitle*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
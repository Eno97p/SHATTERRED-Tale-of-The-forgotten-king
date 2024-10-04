#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LogoFlow final : public CUI
{
private:
	CUI_LogoFlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LogoFlow(const CUI_LogoFlow& rhs);
	virtual ~CUI_LogoFlow() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float			m_fFlowTimer = { 0.f };

	CTexture*		m_pMaskTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_LogoFlow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
#pragma once

#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_LogoBanner final : public CUI
{
private:
	CUI_LogoBanner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LogoBanner(const CUI_LogoBanner& rhs);
	virtual ~CUI_LogoBanner() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float					m_fGlitchTimer = { 0.f };

	CTexture*				m_pDisolveTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_LogoBanner*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
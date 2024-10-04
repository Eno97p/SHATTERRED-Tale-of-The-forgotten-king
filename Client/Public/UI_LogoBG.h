#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LogoBG final : public CUI
{
private:
	CUI_LogoBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LogoBG(const CUI_LogoBG& rhs);
	virtual ~CUI_LogoBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float			m_fGlitchTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_LogoBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
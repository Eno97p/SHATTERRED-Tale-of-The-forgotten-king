#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MapArea final : public CUI
{
private:
	CUI_MapArea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapArea(const CUI_MapArea& rhs);
	virtual ~CUI_MapArea() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	wstring			m_wstrArea;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MapArea*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
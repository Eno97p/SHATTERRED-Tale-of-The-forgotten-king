#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QuickBG final : public CUI
{
private:
	CUI_QuickBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickBG(const CUI_QuickBG& rhs);
	virtual ~CUI_QuickBG() = default;

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
	static CUI_QuickBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
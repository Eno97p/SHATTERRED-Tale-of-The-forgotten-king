#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LoadingBG final : public CUI
{
private:
	CUI_LoadingBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LoadingBG(const CUI_LoadingBG& rhs);
	virtual ~CUI_LoadingBG() = default;

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
	static CUI_LoadingBG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
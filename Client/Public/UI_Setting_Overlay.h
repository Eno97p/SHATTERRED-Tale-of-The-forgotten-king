#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Setting_Overlay final : public CUI
{
private:
	CUI_Setting_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Setting_Overlay(const CUI_Setting_Overlay& rhs);
	virtual ~CUI_Setting_Overlay() = default;

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
	static CUI_Setting_Overlay*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
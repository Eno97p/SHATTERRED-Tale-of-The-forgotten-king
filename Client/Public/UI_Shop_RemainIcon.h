#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Shop_RemainIcon final : public CUI
{
private:
	CUI_Shop_RemainIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop_RemainIcon(const CUI_Shop_RemainIcon& rhs);
	virtual ~CUI_Shop_RemainIcon() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:


private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Shop_RemainIcon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
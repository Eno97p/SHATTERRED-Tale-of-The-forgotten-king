#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Shop_SoulBG : public CUI
{
private:
	CUI_Shop_SoulBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop_SoulBG(const CUI_Shop_SoulBG& rhs);
	virtual ~CUI_Shop_SoulBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_StateSoul*	m_pSoulIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Soul();

public:
	static CUI_Shop_SoulBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
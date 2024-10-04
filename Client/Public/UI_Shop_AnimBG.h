#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Shop_AnimBG final : public CUI
{
private:
	CUI_Shop_AnimBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop_AnimBG(const CUI_Shop_AnimBG& rhs);
	virtual ~CUI_Shop_AnimBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;
	
private:
	_float			m_fFlowTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Shop_AnimBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
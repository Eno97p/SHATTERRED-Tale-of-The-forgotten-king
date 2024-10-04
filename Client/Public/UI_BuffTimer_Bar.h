#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BuffTimer_Bar final : public CUI
{

private:
	CUI_BuffTimer_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BuffTimer_Bar(const CUI_BuffTimer_Bar& rhs);
	virtual ~CUI_BuffTimer_Bar() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Position(_float fX);

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_BuffTimer_Bar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
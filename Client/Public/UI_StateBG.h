#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateBG final : public CUI
{
private:
	CUI_StateBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateBG(const CUI_StateBG& rhs);
	virtual ~CUI_StateBG() = default;

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
	static CUI_StateBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
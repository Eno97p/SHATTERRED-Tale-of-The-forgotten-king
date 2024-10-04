#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_Setting_BackBtn final : public CUI_Interaction
{
private:
	CUI_Setting_BackBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Setting_BackBtn(const CUI_Setting_BackBtn& rhs);
	virtual ~CUI_Setting_BackBtn() = default;

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
	static CUI_Setting_BackBtn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
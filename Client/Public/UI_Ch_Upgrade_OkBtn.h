#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_Ch_Upgrade_OkBtn final : public CUI_Interaction
{
private:
	CUI_Ch_Upgrade_OkBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ch_Upgrade_OkBtn(const CUI_Ch_Upgrade_OkBtn& rhs);
	virtual ~CUI_Ch_Upgrade_OkBtn() = default;

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
	static CUI_Ch_Upgrade_OkBtn*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
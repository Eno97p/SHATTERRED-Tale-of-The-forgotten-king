#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_UpGPageBtn final : public CUI_Interaction
{
private:
	CUI_UpGPageBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPageBtn(const CUI_UpGPageBtn& rhs);
	virtual ~CUI_UpGPageBtn() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI*		m_pSelectUI = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_UI();

public:
	static CUI_UpGPageBtn*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
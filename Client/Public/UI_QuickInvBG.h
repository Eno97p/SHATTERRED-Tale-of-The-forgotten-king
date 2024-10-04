#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QuickInvBG final : public CUI
{
public:
	typedef struct UI_InvBG_Desc : public UI_DESC
	{
		_bool		isInv = { false };
	}UI_INVBG_DESC;
private:
	CUI_QuickInvBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickInvBG(const CUI_QuickInvBG& rhs);
	virtual ~CUI_QuickInvBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isInv = { false };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_Data();

public:
	static CUI_QuickInvBG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
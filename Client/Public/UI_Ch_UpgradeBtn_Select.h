#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Ch_UpgradeBtn_Select final : public CUI
{
public:
	typedef struct UI_ChUp_BtnSelect_Desc : public UI_DESC
	{
		wstring		wstrSign;
	}UI_CHUP_BTNSELECT_DESC;

private:
	CUI_Ch_UpgradeBtn_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ch_UpgradeBtn_Select(const CUI_Ch_UpgradeBtn_Select& rhs);
	virtual ~CUI_Ch_UpgradeBtn_Select() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	wstring			m_wstrSign;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Ch_UpgradeBtn_Select*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END
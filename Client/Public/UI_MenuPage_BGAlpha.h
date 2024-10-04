#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MenuPage_BGAlpha final : public CUI
{
public:
	typedef struct UI_MP_BGAlpha_Desc : public UI_DESC
	{
		UISORT_PRIORITY		eUISort;
	}UI_MP_BGALPHA_DESC;

private:
	CUI_MenuPage_BGAlpha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MenuPage_BGAlpha(const CUI_MenuPage_BGAlpha& rhs);
	virtual ~CUI_MenuPage_BGAlpha() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	UISORT_PRIORITY		m_eUISort = { SORT_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MenuPage_BGAlpha*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
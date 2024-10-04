#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QuickExplain final : public CUI
{
public:
	typedef struct UI_Explain_Desc : public UI_DESC
	{
		UISORT_PRIORITY	eUISort;
	}UI_EXPLAIN_DESC;
private:
	CUI_QuickExplain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickExplain(const CUI_QuickExplain& rhs);
	virtual ~CUI_QuickExplain() = default;

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
	static CUI_QuickExplain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
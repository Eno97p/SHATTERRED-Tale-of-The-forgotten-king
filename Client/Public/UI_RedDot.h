#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_RedDot final : public CUI
{
public:
	typedef struct UI_RedDot_Desc : public UI_DESC
	{
		UISORT_PRIORITY		eUISort;
	}UI_REDDOT_DESC;
	
private:
	CUI_RedDot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_RedDot(const CUI_RedDot& rhs);
	virtual ~CUI_RedDot() = default;

public:
	void			Set_Rend(_bool isRend) { m_isRend = isRend; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool					m_isRend = { true };

	UISORT_PRIORITY			m_eUISort = { SORT_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_RedDot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
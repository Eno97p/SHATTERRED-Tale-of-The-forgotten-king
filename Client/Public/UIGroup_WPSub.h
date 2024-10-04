#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;

class CUIGroup_WPSub final : public CUIGroup
{
private:
	CUIGroup_WPSub(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_WPSub(const CUIGroup_WPSub& rhs);
	virtual ~CUIGroup_WPSub() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_WPSub*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_Logo final : public CUIGroup
{
private:
	CUIGroup_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Logo(const CUIGroup_Logo& rhs);
	virtual ~CUIGroup_Logo() = default;

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
	HRESULT					Create_Selector();

public:
	static CUIGroup_Logo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
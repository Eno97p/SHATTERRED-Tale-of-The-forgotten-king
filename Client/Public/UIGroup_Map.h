#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_Map final : public CUIGroup
{
private:
	CUIGroup_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Map(const CUIGroup_Map& rhs);
	virtual ~CUIGroup_Map() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CUI*>		m_vecUI;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Map*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
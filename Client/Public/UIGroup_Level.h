#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Level final : public CUIGroup
{
private:
	CUIGroup_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Level(const CUIGroup_Level& rhs);
	virtual ~CUIGroup_Level() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();
	void					Delete_UI();

public:
	static CUIGroup_Level*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
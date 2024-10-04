#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Upgrade final : public CUIGroup
{
private:
	CUIGroup_Upgrade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Upgrade(const CUIGroup_Upgrade& rhs);
	virtual ~CUIGroup_Upgrade() = default;

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

public:
	static CUIGroup_Upgrade*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
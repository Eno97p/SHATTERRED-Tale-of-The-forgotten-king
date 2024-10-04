#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;

class CUIGroup_Character final : public CUIGroup
{
private:
	CUIGroup_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Character(const CUIGroup_Character& rhs);
	virtual ~CUIGroup_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CUI*>		m_vecUI;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Character*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
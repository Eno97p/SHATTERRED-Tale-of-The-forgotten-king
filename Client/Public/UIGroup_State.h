#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_State final : public CUIGroup
{
private:
	CUIGroup_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_State(const CUIGroup_State& rhs);
	virtual ~CUIGroup_State() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Resset_Player();
	void			Rend_Calcul(_int iSoul);

private:
	vector<CUI*>			m_vecUI;
	vector<CUI*>			m_vecStates;
	class CUI_StateSoul*	m_pSoul = { nullptr };

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_State*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
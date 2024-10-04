#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_BossText final : public CUIGroup
{
public:
	typedef struct UIGroup_BossText_Desc : public UIGROUP_DESC
	{
		_bool	isCreateText;
	}UIGROUP_BOSSTEXT_DESC;

private:
	CUIGroup_BossText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_BossText(const CUIGroup_BossText& rhs);
	virtual ~CUIGroup_BossText() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool						m_isCreateText = { false };

	vector<class CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();
	void					Delete_UI();

public:
	static CUIGroup_BossText*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
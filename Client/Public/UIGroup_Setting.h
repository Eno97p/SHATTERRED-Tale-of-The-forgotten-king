#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Setting final : public CUIGroup
{
private:
	CUIGroup_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Setting(const CUIGroup_Setting& rhs);
	virtual ~CUIGroup_Setting() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CUI*>			m_vecUI;
	// btn은 map으로 관리하기?
	map<string, class CUI_Setting_Btn*>	m_mapBtn;


private:
	HRESULT					Create_UI();
	HRESULT					Create_Btn();

public:
	static CUIGroup_Setting*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
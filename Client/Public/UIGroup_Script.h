#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_Script_DialogBox;

class CUIGroup_Script final : public CUIGroup
{
public:
	enum NPC_TYPE { NPC_RLYA, NPC_VALNIR, NPC_CHORON, NPC_YAAK, NPC_END };
	typedef struct UIGroup_Script_Desc : public UIGROUP_DESC
	{
		NPC_TYPE eNpcType;
	}UIGROUP_SCRIPT_DESC;

private:
	CUIGroup_Script(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Script(const CUIGroup_Script& rhs);
	virtual ~CUIGroup_Script() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Set_DialogText(wstring wstrDialogText);

private:
	vector<CUI*>			m_vecUI;
	CUI_Script_DialogBox*	m_pDialogBox;

	NPC_TYPE				m_eNpcType;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Script*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
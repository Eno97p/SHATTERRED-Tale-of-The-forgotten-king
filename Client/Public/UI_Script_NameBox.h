#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Script_NameBox final : public CUI
{
public:
	enum NPC_TYPE { NPC_RLYA, NPC_VALNIR, NPC_CHORON, NPC_YAAK, NPC_END };
	typedef struct UI_Script_NameBox_Desc : public UI_DESC
	{
		NPC_TYPE	eNpcType;
	}UI_SCRIPT_NAMEBOX_DESC;
private:
	CUI_Script_NameBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Script_NameBox(const CUI_Script_NameBox& rhs);
	virtual ~CUI_Script_NameBox() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	NPC_TYPE		m_eNpcType = { NPC_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	_tchar* Setting_Text();

public:
	static CUI_Script_NameBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
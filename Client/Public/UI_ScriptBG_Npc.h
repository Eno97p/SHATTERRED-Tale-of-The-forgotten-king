#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_ScriptBG_Npc final : public CUI
{
public:
	typedef struct UI_Script_Desc : public UI_DESC
	{
		wstring			wstrTextureName;
	}UI_SCRIPT_DESC;

private:
	CUI_ScriptBG_Npc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScriptBG_Npc(const CUI_ScriptBG_Npc& rhs);
	virtual ~CUI_ScriptBG_Npc() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	HRESULT	Add_Components(wstring wstrTextureName);
	HRESULT	Bind_ShaderResources();

public:
	static CUI_ScriptBG_Npc*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_UpCompleted_Forge : public CUI
{
public:
	typedef struct UI_UpCom_Forge_Desc : public UI_DESC
	{
		wstring		wstrTextureName;
	}UI_UPCOM_FORGE_DESC;
private:
	CUI_UpCompleted_Forge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpCompleted_Forge(const CUI_UpCompleted_Forge& rhs);
	virtual ~CUI_UpCompleted_Forge() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_ItemIcon*			m_pItemIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_ItemIcon(wstring wstrTextureName);

public:
	static CUI_UpCompleted_Forge*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
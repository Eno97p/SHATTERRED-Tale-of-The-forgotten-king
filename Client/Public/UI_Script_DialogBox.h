#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Script_DialogBox final : public CUI
{
private:
	CUI_Script_DialogBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Script_DialogBox(const CUI_Script_DialogBox& rhs);
	virtual ~CUI_Script_DialogBox() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void	Setting_Text(wstring wstrDialogText);
	void	Setting_TextXY(_float fX, _float fY);

private:
	_float			m_fTextX = { 0.f };
	_float			m_fTextY = { 0.f };
	wstring			m_wstrDialogText;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Script_DialogBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
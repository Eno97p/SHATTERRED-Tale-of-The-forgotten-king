#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_ScriptBG_Aura final : public CUI
{
public:
	typedef struct UI_Aura_Desc : public UI_DESC
	{
		_uint		iTextureNum;
	}UI_AURA_DESC;

private:
	CUI_ScriptBG_Aura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScriptBG_Aura(const CUI_ScriptBG_Aura& rhs);
	virtual ~CUI_ScriptBG_Aura() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float			m_fFlowTimer = { 0.f };

	_uint			m_iTextureNum = { 0 };

	CTexture* m_pMaskTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_ScriptBG_Aura*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
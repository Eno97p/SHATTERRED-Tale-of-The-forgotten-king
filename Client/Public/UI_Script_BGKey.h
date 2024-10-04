#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Script_BGKey final : public CUI
{
	const _float ANIM_Y = 50.f;

public:
	typedef struct UI_Script_Key_Desc : public UI_DESC
	{
		_uint iKeyIdx;
	}UI_SCRIPT_KEY_DESC;

private:
	CUI_Script_BGKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Script_BGKey(const CUI_Script_BGKey& rhs);
	virtual ~CUI_Script_BGKey() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isUpAnim = { false };
	_uint			m_iKeyIdx = { 0 };
	_float			m_fDefaultY = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	UpAndDown_Anim(_float fTimeDelta);

public:
	static CUI_Script_BGKey*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
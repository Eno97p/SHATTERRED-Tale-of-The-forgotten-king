#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossHPBar final : public CUI
{
public:
	typedef struct UI_BossHPBar_Desc : public UI_DESC
	{
		_tchar*		wszBossName;
	}UI_BOSSHPBAR_DESC;

private:
	CUI_BossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossHPBar(const CUI_BossHPBar& rhs);
	virtual ~CUI_BossHPBar() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_tchar			m_wszBossName[MAX_PATH];

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_BossHPBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
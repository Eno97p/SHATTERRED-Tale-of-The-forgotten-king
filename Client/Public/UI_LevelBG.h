#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LevelBG final : public CUI
{
private:
	CUI_LevelBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LevelBG(const CUI_LevelBG& rhs);
	virtual ~CUI_LevelBG() = default;

public:
	_bool			Get_isDead() { return m_isDead; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isDead = { false };
	_float			m_fMoveTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_LevelBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
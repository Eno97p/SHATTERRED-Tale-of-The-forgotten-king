#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Cinematic final : public CUI
{
	const _float fMAXSIZE = g_iWinSizeY + 500.f;
	const _float fMINSIZE = g_iWinSizeY + 150.f;
private:
	CUI_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Cinematic(const CUI_Cinematic& rhs);
	virtual ~CUI_Cinematic() = default;

public:
	_bool			Get_isBigAnim() { return m_isAnimIn; }
	void			Set_isBigAim(_bool isAnimIn) { m_isAnimIn = isAnimIn; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isRend = { false };
	_bool			m_isAnimIn = { true }; // In : ÀÛ¾ÆÁü / Out : Ä¿Áü
	_float			m_fSizeTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Cinematic*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
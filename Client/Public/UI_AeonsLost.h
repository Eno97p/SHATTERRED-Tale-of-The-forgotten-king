#pragma once

#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_AeonsLost final : public CUI
{
private:
	CUI_AeonsLost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AeonsLost(const CUI_AeonsLost& rhs);
	virtual ~CUI_AeonsLost() = default;

public:
	_bool			Get_isEnd() { return m_isEnd; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isEnd = { false };
	_bool			m_isFadeOut = { true };
	_float			m_fDeadTimer = { 0.f };
	_float			m_fDisolveValue = { 0.f };
	_float			m_fColor = { 1.f };

	CTexture*		m_pDisolveTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_AeonsLost*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
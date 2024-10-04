#pragma once

#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_FadeInOut final : public CUI
{
public:
	enum FADE_TYPE { TYPE_ALPHA, TYPE_DISSOLVE, TYPE_END };
	typedef struct UI_FadeInOut_Desc : public UI_DESC
	{
		FADE_TYPE	eFadeType;
		LEVEL       eNetLevel;
		_bool		isFadeIn;
		_bool		isLevelChange;

	}UI_FADEINOUT_DESC;

private:
	CUI_FadeInOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FadeInOut(const CUI_FadeInOut& rhs);
	virtual ~CUI_FadeInOut() = default;

public:
	_bool			Get_isFadeAnimEnd() { return m_isFadeAnimEnd; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_isFadeIn = { false };
	_bool				m_isLevelChange = { false };
	_bool				m_isFadeAnimEnd = { false }; // Fade Out 종료 여부
	_float				m_fAlphaTimer = { 0.f };
	_float				m_fDisolveValue = { 0.f };
	_float				m_fAlphaTimerMul = { 0.f };

	FADE_TYPE			m_eFadeType = { TYPE_END };
	LEVEL				m_eNetLevel = { LEVEL_STATIC };

	CTexture*				m_pDisolveTextureCom = { nullptr };
	class CUI_Memento*		m_pMemento = { nullptr };
	class CUI_AeonsLost*	m_pAeonsLost = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	HRESULT	Create_Memento();
	HRESULT	Create_AeonsLost();
	LEVEL	Check_NextLevel();

public:
	static CUI_FadeInOut*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
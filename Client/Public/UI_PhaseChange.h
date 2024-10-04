#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PhaseChange final : public CUI
{
public:
	typedef struct UI_PhaseChange_Desc : public UI_DESC
	{
		_bool isFadeIn;
	}UI_PHASECHANGE_DESC;

private:
	CUI_PhaseChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PhaseChange(const CUI_PhaseChange& rhs);
	virtual ~CUI_PhaseChange() = default;

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
	_bool				m_isFadeAnimEnd = { false }; // Fade Out 종료 여부
	_float				m_fAlphaTimer = { 0.f };
	_float				m_fDeadTimer = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_PhaseChange*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Ring final : public CUI
{
	const _float fGOOD = 270.f; // 250
	const _float fPERFECT = 200.f; // 180
	const _float fEND = 140.f;

public:
	enum RING_TYPE { RING_RED = 0, RING_PINK, RING_BLUE, RING_GREEN, RING_END };
	enum RING_STATE { RS_PERFECT = 0, RS_GOOD, RS_BAD, RS_END };
	typedef struct UI_Ring_Desc : public UI_DESC
	{
		_bool		isDuo;
		RING_TYPE	eRingType;
	}UI_RING_DESC;

private:
	CUI_QTE_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Ring(const CUI_QTE_Ring& rhs);
	virtual ~CUI_QTE_Ring() = default;

public:
	_bool			Get_End() { return m_isEnd; }
	void			Set_End(_bool isEnd) { m_isEnd = isEnd; }

	RING_STATE		Get_RingState() { return m_eRingState; }

	_bool			Get_isAnimOn() { return m_isAnimOn; }
	void			Set_isAnimOn(_bool isAnimOn) { m_isAnimOn = isAnimOn; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isEnd = { false };
	_bool			m_isAnimOn = { false };
	_float			m_fSpeed = { 0.f };

	RING_TYPE		m_eRingType = { RING_END };
	RING_STATE		m_eRingState = { RS_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_QTE_Ring*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
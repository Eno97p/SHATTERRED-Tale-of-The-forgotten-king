#pragma once

#include "UI.h"

BEGIN(Client)

// BuffTimer 관련 UI들을 멤버 변수로 관리하는 클래스. UIGroup_BuffTimer가 해당 클래스 객체를 멤버 변수로 가질 예정임

class CUI_BuffTimer final : public CUI
{
public:
	enum BUFFTYPE { BUFFTYPE_DAMAGE, BUFFTYPE_SHIELD, BUFFTYPE_HP, BUFFTYPE_STAMINA, BUFFTYPE_END };
	typedef struct UI_BuffTimer_Desc : public UI_DESC
	{
		_uint		iBuffTimerIdx;
		wstring		wstrTextureName;
	}UI_BUFFTIMER_DESC;

private:
	CUI_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BuffTimer(const CUI_BuffTimer& rhs);
	virtual ~CUI_BuffTimer() = default;

public:
	void			Set_Index(_uint iBuffTimerIdx) { m_iBuffTimerIdx = iBuffTimerIdx; }
	wstring			Get_TextureName() { return m_wstrTextureName; }

	_bool			Get_isBuffEnd() { return m_isBuffEnd; }
	void			Reset_BuffTime() { m_fBuffTimer = 0.f; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool				m_isBuffEnd = { false };
	_uint				m_iBuffTimerIdx = { 0 };
	_float				m_fBuffTimer = { 0.f };
	wstring				m_wstrTextureName;

	vector<class CUI*>	m_vecUI;

	BUFFTYPE			m_eBuffType = { BUFFTYPE_END };

private:
	HRESULT	Create_UI(wstring wstrTextureName);

	void	Setting_BuffType();
	void	Setting_BuffFunction(_bool isOn);
	void	Setting_UIPosition();
	wstring	Setting_BuffText();
	void	Update_BuffTime();

public:
	static CUI_BuffTimer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Btn final : public CUI
{
public:
	enum SCORE_TYPE { SCORE_PERFECT = 0, SCORE_GOOD, SCORE_BAD, SCORE_END };
	typedef struct UI_Qte_Btn_Desc : public UI_DESC
	{
		_bool		isDuo;
		_uint		iBtnIndex;
	}UI_QTE_BTN_DESC;

private:
	CUI_QTE_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Btn(const CUI_QTE_Btn& rhs);
	virtual ~CUI_QTE_Btn() = default;

public:
	_bool			Get_Start() { return m_isStart; }
	void			Set_Start(_bool isStart) { m_isStart = isStart; }

	_bool			Get_isScore() { return m_isScore; }
	SCORE_TYPE		Get_ScoreType() { return m_eScoreType; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool					m_isStart = { false };
	_bool					m_isScore = { false }; // 끝나고 점수 매겨졌는지
	_bool					m_isDuo = { false };
	_uint					m_iBtnNum = { 0 };

	class CUI_QTE_Ring*		m_pRing = { nullptr };
	class CUI_QTE_Score*	m_pScore = { nullptr };


	SCORE_TYPE				m_eScoreType = { SCORE_BAD }; // 이거 디폴트값을 BAD로 해보기? >>> 테스트 필요


private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Ring();
	HRESULT	Create_Score();
	
	void	Setting_ScoreType();

public:
	static CUI_QTE_Btn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
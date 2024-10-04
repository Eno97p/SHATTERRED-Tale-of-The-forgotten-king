#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Score final : public CUI
{
public:
	enum SCORE_TYPE { SCORE_PERFECT, SCORE_GOOD, SCORE_BAD, SCORE_END };
	typedef struct UI_Score_Desc : public UI_DESC
	{
		SCORE_TYPE	eScoreType;
	}UI_SCORE_DESC;

private:
	CUI_QTE_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Score(const CUI_QTE_Score& rhs);
	virtual ~CUI_QTE_Score() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_QTE_Particle*		m_pParticle = { nullptr };
	class CUI_QTE_Shine*		m_pShine_Big = { nullptr }; // 한 개만 할 건지? 대각선으로 두 개도 괜찮을 듯
	class CUI_QTE_Shine*		m_pShine_Small = { nullptr }; // 한 개만 할 건지? 대각선으로 두 개도 괜찮을 듯

	SCORE_TYPE					m_eScoreType = { SCORE_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Particle();
	HRESULT	Create_Shine();

public:
	static CUI_QTE_Score*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
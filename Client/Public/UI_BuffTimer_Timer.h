#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BuffTimer_Timer final : public CUI
{
private:
	CUI_BuffTimer_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BuffTimer_Timer(const CUI_BuffTimer_Timer& rhs);
	virtual ~CUI_BuffTimer_Timer() = default;

public:
	void			Set_Ratio(_float fRatio) { m_fCurrentRatio = fRatio; }
	void			Set_BuffTime(_float fBuffTime) { m_fBuffTimer = fBuffTime; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Position(_float fX);

private:
	_float			m_fCurrentRatio = 1.f;
	_float			m_fPastRatio = 1.f;
	_float			m_fHudRatio = 1.f;
	_float			m_fBuffTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_BuffTimer_Timer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
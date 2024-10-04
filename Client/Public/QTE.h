#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CQTE final : public CGameObject
{
private:
	CQTE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE(const CQTE& rhs);
	virtual ~CQTE() = default;

public:
	_bool			Get_isEnd() { return m_isEnd; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool		Check_ResultScore();
	_bool		Check_End();

private:
	_bool						m_isEnd = { false };
	vector<class CUI_QTE_Btn*>	m_vecBtn;

private:
	HRESULT Create_QteBtn();

	void	Start_BtnEvent();

public:
	static CQTE*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
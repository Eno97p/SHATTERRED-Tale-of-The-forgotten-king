#pragma once

#include "UI.h"

BEGIN(Client)
class CMouse;

class CUI_Interaction abstract : public CUI
{
protected:
	CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Interaction(const CUI_Interaction& rhs);
	virtual ~CUI_Interaction() = default;

public:
	RECT*			Get_CollisionRect() { return &m_CollisionRect; }
	_bool			IsCollisionRect(RECT* SrcRect);
	
	_uint			Get_Select() { return m_isSelect; }
	void			Set_Select(_bool isSelect) { m_isSelect = isSelect; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

protected:
	_bool			m_isSelect = { false };

	RECT			m_CollisionRect = {};
	CMouse*			m_pMouse = { nullptr };

protected:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;

};

END
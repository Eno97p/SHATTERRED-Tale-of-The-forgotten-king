#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMouse final : public CUI
{
private:
	CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMouse(const CMouse& rhs);
	virtual ~CMouse() = default;

public:
	_float4*		Get_MousePos() { return &m_vMousePos; }		
	RECT*			Get_CollisionRect() { return &m_CollisionRect; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float4			m_vMousePos;

	RECT			m_CollisionRect = {};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMouse*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg) override;
	virtual void	Free() override;
};

END
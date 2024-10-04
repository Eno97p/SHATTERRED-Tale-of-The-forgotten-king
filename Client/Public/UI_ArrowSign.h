#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_ArrowSign final : public CUI
{
	const _float ANIM_Y = 0.3f;
public:
	typedef struct UI_ArrowSign_Desc : public UI_DESC
	{
		_vector vPos;
	}UI_ARROWSIGN_DESC;

private:
	CUI_ArrowSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ArrowSign(const CUI_ArrowSign& rhs);
	virtual ~CUI_ArrowSign() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isUpAnim = { false };
	_float			m_fDefaultY = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Move(_float fTimeDelta);

public:
	static CUI_ArrowSign*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
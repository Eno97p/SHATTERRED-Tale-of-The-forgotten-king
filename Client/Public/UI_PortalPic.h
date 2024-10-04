#pragma once

#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_PortalPic final : public CUI
{
public:
	typedef struct UI_PortalPic_Desc : UI_DESC
	{
		_uint	iPicNum;
		_float	fAngle;
		_float2	fScale;
		_vector vPos;
	}UI_PORTALPIC_DESC;

private:
	CUI_PortalPic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PortalPic(const CUI_PortalPic& rhs);
	virtual ~CUI_PortalPic() = default;

public:
	//void			Set_DeadDissolve() { m_isDeadDissolve = true; }
	void			Set_FadeOut() { m_isFadeIn = false; }
	_bool			Get_isFadeEnd() { return m_isFadeEnd; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	_bool			m_isFadeIn = { true };
	_bool			m_isFadeEnd = { false };
	_uint			m_iPicNum = { 0 };
	_float			m_fOpacityTimer = { 0.f };
	_float			m_fDisolveValue = { 0.f };
	_float2			m_fScale;

	CTexture* m_pOpacityTextureCom = { nullptr };
	CTexture* m_pDisolveTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_PortalPic*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
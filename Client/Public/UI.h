#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	typedef struct UI_Desc : public GAMEOBJECT_DESC
	{
		UI_Desc() : fX(0.f), fY(0.f), fSizeX(0.f), fSizeY(0.f), eLevel(LEVEL_END) {}
		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
		LEVEL	eLevel;
	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	_bool			Get_RenderOnAnim() { return m_isRenderOffAnim; }
	void			Set_RenderOnAnim(_bool isRenderOffAnim) { m_isRenderOffAnim = isRenderOffAnim; }

	_bool			Get_RenderAnimFinished() { return m_isRenderAnimFinished; }
	void			Set_RenderAnimFinised(_bool isRenderAnimFinished) { m_isRenderAnimFinished = isRenderAnimFinished; }

	_bool			Get_isSceneChange() { return m_bisSceneChange; }
public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Render_Animation(_float fTimeDelta, _float fSpeed = 3.f);
	void			Resset_Animation(_bool isOffAnim);
	_bool			isRender_End();

protected:
	_bool				m_isRenderOffAnim = { false }; // 켜지는 애니메이션인지 꺼지는 애니메이션인지
	_bool				m_isRenderAnimFinished = { false }; // 애니메이션 종료 여부
	_float				m_fRenderTimer = { 0.f };

	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	LEVEL				m_eLevel = { LEVEL_END };

	_bool				m_bisSceneChange = false;
protected:
	void		Setting_Position();

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END
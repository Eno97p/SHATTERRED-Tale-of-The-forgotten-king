#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup abstract : public CGameObject
{
public:
	typedef struct UIGroup_Desc : public GAMEOBJECT_DESC
	{
		LEVEL	eLevel;
	}UIGROUP_DESC;

protected:
	CUIGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup(const CUIGroup& rhs);
	virtual ~CUIGroup() = default;

public:
	_bool			Get_Rend() { return m_isRend; }
	void			Set_Rend(_bool isRend) { m_isRend = isRend; }

	_bool			Get_AnimFinished() { return m_isAnimFinished; }
	void			Set_AnimFinished(_bool isAnimFinished) { m_isAnimFinished = isAnimFinished; }

	_bool			Get_RenderOnAnim() { return m_isRenderOnAnim; }
	void			Set_RenderOnAnim(_bool isRenderOnAnim) { m_isRenderOnAnim = isRenderOnAnim; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_bool		m_isRend = { false };
	_bool		m_isAnimFinished = { false };
	_bool		m_isRenderOnAnim = { true };

	LEVEL		m_eLevel = { LEVEL_END };

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END
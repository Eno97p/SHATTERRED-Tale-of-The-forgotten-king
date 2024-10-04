#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Portal final : public CUIGroup
{
public:
	typedef struct UIGroup_Portal_Desc : public UIGROUP_DESC
	{
		_bool		isPic;
		_uint		iPicNum;
		_float		fAngle;
		_float		fDistance;
		_float2		fScale;
		_vector		vPos;
		LEVEL		ePortalLevel;
	}UIGROUP_PORTAL_DESC;

private:
	CUIGroup_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Portal(const CUIGroup_Portal& rhs);
	virtual ~CUIGroup_Portal() = default;

public:
	_bool			Get_isPic();
	void			Set_FadeOut();
	_bool			Get_isFadeEnd() { return m_isFadeEnd; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool					m_isFadeEnd = { false };

	class CUI_PortalPic*	m_pPic = { nullptr };
	class CUI_PortalText*	m_pText = { nullptr };

	LEVEL					m_ePortalLevel = { LEVEL_END };

private:
	HRESULT					Create_UI(void* pArg); // _bool isPic, _uint iPicNum, _float fAngle, _vector vPos, _float2 fScale

public:
	static CUIGroup_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
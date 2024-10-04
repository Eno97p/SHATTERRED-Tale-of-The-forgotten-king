#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_UP_Completed final : public CUIGroup
{
public:
	typedef struct UIGroup_Completed_Desc : public UIGROUP_DESC
	{
		_uint	iCurSlotIdx;
	}UIGROUP_COMPLETED_DESC;

public:
	_bool				Get_isEnd() { return m_isEnd; }

private:
	CUIGroup_UP_Completed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_UP_Completed(const CUIGroup_UP_Completed& rhs);
	virtual ~CUIGroup_UP_Completed() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool						m_isEnd = { false };
	_float						m_fCloseTimer = { 0.f };

	vector<class CUI*>			m_vecUI;

private:
	HRESULT					Create_UI(wstring wstrTextureName);
	wstring					Setting_TextureName(_uint iCurSlotIdx);

public:
	static CUIGroup_UP_Completed*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
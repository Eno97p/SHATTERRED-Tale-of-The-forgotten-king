#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_UpGPage_Value final : public CUI
{
public:
	enum VALUE_TYPE { VALUE_SOUL, VALUE_MATERIAL, VALUE_END };
	typedef struct UI_Value_Desc : public UI_DESC
	{
		VALUE_TYPE	eValueType;
	}UI_VALUE_DESC;

private:
	CUI_UpGPage_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPage_Value(const CUI_UpGPage_Value& rhs);
	virtual ~CUI_UpGPage_Value() = default;

public:
	_bool			Get_isEnough() { return m_isEnough; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Value(_uint iCurSlotIdx);

private:
	_bool			m_isEnough = { false };
	_uint			m_iPrive = { 0 };
	_uint			m_iValue = { 0 };
	_uint			m_iJemstone = { 0 };
	wstring			m_wstrText;

	VALUE_TYPE		m_eValueType = { VALUE_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Rend_Font();
	void	Update_Jemstone();

public:
	static CUI_UpGPage_Value*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
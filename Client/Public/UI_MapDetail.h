#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MapDetail final : public CUI
{
public:
	enum AREA_TYPE { AREA_MANTARI = 0, AREA_ACKBAR, AREA_GROTTO, AREA_PLAINS, AREA_WILDERNESS, AREA_END };
	typedef struct UI_MapDetail_Desc : public UI_DESC
	{
		_uint		iIdx;
	}UI_MAPDETAIL_DESC;
	
private:
	CUI_MapDetail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapDetail(const CUI_MapDetail& rhs);
	virtual ~CUI_MapDetail() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	wstring					m_wstrDetail;
	
	class CUI_MapIcon*		m_pMapIcon = { nullptr };

	AREA_TYPE				m_eAreaType = { AREA_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_AreaData(_uint iIdx);
	void	Create_MapIcon(_uint iIdx);

	// Create Icon 할 때 iIdx를 인자로 넣어주어 Icon 클래스에서 Texture 선택할 때 활용하도록 연결할 것

public:
	static CUI_MapDetail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
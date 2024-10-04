#pragma once
#include "Map_Element.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBackGround_Card final : public CMap_Element
{
public:
	typedef struct CARD_DESC : public CMap_Element::MAP_ELEMENT_DESC
	{
		_uint iTexNum = 0;
	};

private:
	CBackGround_Card(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround_Card(const CBackGround_Card& rhs);
	virtual ~CBackGround_Card() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
//virtual HRESULT Render_Bloom() override;

private:
	_uint m_iTexNum = 0;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBackGround_Card* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
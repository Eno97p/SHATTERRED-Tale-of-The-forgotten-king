#pragma once
#include "Map_Element.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBackGround_Moon final : public CMap_Element
{
public:
	typedef struct CARD_DESC : public CMap_Element::MAP_ELEMENT_DESC
	{
		_uint iTexNum = 0;
	};

private:
	CBackGround_Moon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround_Moon(const CBackGround_Moon& rhs);
	virtual ~CBackGround_Moon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
//virtual HRESULT Render_Bloom() override;

private:
	_uint m_iTexNum = 0;
	CTexture* m_pMoonTex = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBackGround_Moon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
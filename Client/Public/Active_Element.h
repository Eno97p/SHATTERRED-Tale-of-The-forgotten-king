#pragma once

#include "Client_Defines.h"
#include "Map_Element.h"

BEGIN(Client)

class CActive_Element : public CMap_Element
{
protected:
	CActive_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActive_Element(const CActive_Element& rhs);
	virtual ~CActive_Element() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Add_Components(MAP_ELEMENT_DESC* desc);
	HRESULT Bind_ShaderResources();

public:
	static CActive_Element* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Map_Element.h"

BEGIN(Client)

class CDeco_Element final : public CMap_Element
{
private:
	CDeco_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeco_Element(const CDeco_Element& rhs);
	virtual ~CDeco_Element() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Mirror() override;

public:
	HRESULT Add_Components(MAP_ELEMENT_DESC* desc);
	HRESULT Bind_ShaderResources();


private:
	vector<CPhysXComponent*> m_pPhysXCom = { nullptr };
	_bool			 m_bNoCullElement = false;
	_uint			 m_iTest = 0;

	_uint			m_iInstanceCount = 0;

	_bool			m_bRotateObj = false;
	_float			m_fRotateOffset = 1.f;
	_vector			m_vOriginPos = {0.f, 0.f, 0.f, 1.f};
	_vector			m_vRotateAxis = { 0.f, 1.f, 0.f ,0.f };
public:
	static CDeco_Element* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
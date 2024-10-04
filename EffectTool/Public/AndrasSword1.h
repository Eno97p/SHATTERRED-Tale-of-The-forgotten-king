#pragma once

#include "Effect_Define.h"
#include "Weapon.h"

BEGIN(Effect)

class CAndrasSword1 final : public CWeapon
{
private:
	CAndrasSword1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasSword1(const CAndrasSword1& rhs);
	virtual ~CAndrasSword1() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAndrasSword1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
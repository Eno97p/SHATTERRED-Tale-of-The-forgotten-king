#pragma once

#include "Effect_Define.h"
#include "Weapon.h"

BEGIN(Effect)

class CAndrasSword4 final : public CWeapon
{
private:
	CAndrasSword4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasSword4(const CAndrasSword4& rhs);
	virtual ~CAndrasSword4() = default;

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
	static CAndrasSword4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
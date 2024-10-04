#pragma once
#include "Weapon.h"

BEGIN(Client)
class CAndrasHead final : public CWeapon
{
private:
	CAndrasHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasHead(const CAndrasHead& rhs);
	virtual ~CAndrasHead() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
public:
	HRESULT Add_Components();
private:
	CGameObject* EyeTrail = nullptr;

public:
	static CAndrasHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END

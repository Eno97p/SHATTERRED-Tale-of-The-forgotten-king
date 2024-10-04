#pragma once
#include "Weapon.h"
#include "Effect_Define.h"

BEGIN(Effect)
class CPlayerHead final : public CWeapon
{
private:
	CPlayerHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerHead(const CPlayerHead& rhs);
	virtual ~CPlayerHead() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	static CPlayerHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
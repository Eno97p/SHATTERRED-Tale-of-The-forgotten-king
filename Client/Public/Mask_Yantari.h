#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CMask_Yantari final : public CWeapon
{
private:
	CMask_Yantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMask_Yantari(const CMask_Yantari& rhs);
	virtual ~CMask_Yantari() = default;

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
	static CMask_Yantari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CMask_Andras final : public CWeapon
{
private:
	CMask_Andras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMask_Andras(const CMask_Andras& rhs);
	virtual ~CMask_Andras() = default;

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
	static CMask_Andras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
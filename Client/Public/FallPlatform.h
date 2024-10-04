#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CCollider;
END

BEGIN(Client)
class CPlayer;

class CFallPlatform final : public CWeapon
{
private:
	CFallPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFallPlatform(const CFallPlatform& rhs);
	virtual ~CFallPlatform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	class CPlayer* m_pPlayer = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFallPlatform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
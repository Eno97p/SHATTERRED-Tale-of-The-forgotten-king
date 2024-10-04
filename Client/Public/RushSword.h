#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CRushSword final : public CWeapon
{
public:
	enum RUSHTYPE { TYPE_BABYLON, TYPE_SHOOTINGSTAR};

public:
	typedef struct RUSH_DESC : public WEAPON_DESC
	{
		RUSHTYPE eRushtype;
		_uint meshNum;
		_float fHeight;
		_bool bSound = false;
	}RUSH_DESC;

private:
	CRushSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRushSword(const CRushSword& rhs);
	virtual ~CRushSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fShootDelay = 1.f;
	_float m_fHeight;
	_uint m_iMeshNum;

	CGameObject* pParticle = nullptr;

	_bool m_bSound = false;


public:
	static CRushSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
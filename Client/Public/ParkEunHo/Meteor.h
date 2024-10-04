#pragma once
#include "Weapon.h"
#include "Client_Defines.h"
#include "Meteor_Core.h"
#include "Meteor_Wind.h"
#include "DefaultCylinder.h"

BEGIN(Client)
class CMeteor final : public CWeapon
{
public:
	typedef struct METEOR_DESC
	{
		_float fLifeTime = 0.f;
		_float fWindInterval = 0.2f;
		_float3 fStartOffset = { 0.f,0.f,0.f };
		_float4 vTargetPos = { 0.f,0.f,0.f,1.f };
		CMeteor_Core::METEOR_CORE  CoreDesc{};
		CMeteor_Wind::METEOR_WIND  WindDesc{};
		CDefaultCylinder::DEFAULTCYLINDER CylinderDesc{};
	};

private:
	CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeteor(const CMeteor& rhs);
	virtual ~CMeteor() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();
	HRESULT Generate_Wind();
	void Generate_Final_Particles();
private:
	shared_ptr<METEOR_DESC>					m_OwnDesc;
	vector<CGameObject*>					m_EffectClasses;
	XMVECTOR								m_vStartPos = XMVectorZero();
	_float									m_fWindSpawn = 0.f;
	_float									m_LifeRatio = 0.f;
	_float									m_CurLifeTime = 0.f;

	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

public:
	static CMeteor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
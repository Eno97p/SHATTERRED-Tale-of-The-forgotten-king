#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "AndrasLazer_Base.h"
#include "AndrasLazer_Cylinder.h"
#include "AndrasRain.h"
#include "ElectricCylinder.h"
#include "AndrasScrew.h"
#include "ShieldSphere.h"
#include "LazerCast.h"

BEGIN(Effect)
class CAndrasLazer : public CGameObject
{
public:
	typedef struct ANDRAS_LAZER_TOTALDESC
	{
		const _float4x4* ShooterMat = nullptr;
		_float fLifeTime = 0.f;
		_float3 vOffsetPos = { 0.f,0.f,0.f };
		_float ElectricInterval = 0.1f;
		_float ShieldInterval = 0.1f;
		CAndrasLazerBase::ANDRAS_LASER_BASE_DESC BaseDesc{};
		CAndrasLazerCylinder::ANDRAS_LASER_CYLINDER_DESC CylinderDesc{};
		CAndrasRain::ANDRAS_RAIN_DESC RainDesc{};
		CAndrasScrew::ANDRAS_SCREW_DESC ScrewDesc{};
		CElectricCylinder::ANDRAS_ELECTRIC_DESC ElectricDesc{};
		CShieldSphere::SHIELD_DESC ShieldDesc{};
		CLazerCast::LAZER_CAST	CastDesc{};
	};
private:
	CAndrasLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasLazer(const CAndrasLazer& rhs);
	virtual ~CAndrasLazer() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();
	void Generate_Electric();
	void Generate_Shield();

private:
	shared_ptr<ANDRAS_LAZER_TOTALDESC>	m_OwnDesc;
	_float CurInterval = 0.f;
	_float ShieldInterval = 0.f;

private:
	_bool	LazerSpawned = false;
	CGameObject* LazerCast = nullptr;
public:
	static CAndrasLazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
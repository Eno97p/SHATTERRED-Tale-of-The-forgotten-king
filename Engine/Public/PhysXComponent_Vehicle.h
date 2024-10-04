#pragma once

#include "PhysXComponent.h"




//해당 클래스는 static 오브젝트에 대한 사전 쿠킹을 위한 클래스

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_Vehicle final : public CPhysXComponent
{
public:
	/* 핸드 브레이크 미사용*/
	struct  VEHICLE_COMMAND
	{
		_float4x4 initTransform = {};

		PxTransform transform = PxTransform(PxIdentity);

		WheelConfiguration wheelConfig = {0,0};
		/*  0~1사이에의 값 
		0은 완전히 안 밟은 상태
		1은 완전히 밟은 상태*/
		PxReal throttle = 0.0f;

		PxF32 MaxHorsePower = 0.0f;

		/*  -1~1사이에의 값
		-1은 왼쪽으로 완전히 꺾은 상태
		1은 오른쪽으로 완전히 꺾은 상태
		*/
		PxReal steer = 0.0f;

	
		/*  0~1사이에의 값
		0은 완전히 안 밟은 상태
		1은 완전히 밟은 상태
		*/
		PxReal brake = 0.0f;
		PxReal handbrake = 0.0f;
		PxReal curSpeed = 0.0f;
		/* 기어 상태*/
		PxVehicleDirectDriveTransmissionCommandState::Enum gear = PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL;

		
		void Reset()
		{
			throttle = 0.0f;
			steer = 0.0f;
			brake = 0.0f;
			gear = PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL;
		}
	};
#ifdef _DEBUG
	typedef struct PhysX_Vehicle_Editable_Desc : CPhysXComponent::PhysX_Editable_Desc
	{
		PhysXActorVehicle* pPhysXActorVehicle = nullptr;


	}PhysX_Vehicle_Editable_Desc;
#endif // _DEBUG
private:
	explicit CPhysXComponent_Vehicle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_Vehicle(const CPhysXComponent_Vehicle& rhs);
	virtual ~CPhysXComponent_Vehicle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render()override;
	virtual PhysX_Vehicle_Editable_Desc* GetData() override { return &m_OutDesc; }
#endif

public:
	virtual void Tick(const _float fTimeDelta);
	virtual void Late_Tick(const _float fTimeDelta);


	void GetCommand(VEHICLE_COMMAND*& const command) { command = &m_Command; }
	
	void GetSteerRespon(PxVehicleSteerCommandResponseParams*& steerResponse) { m_pPhysXActorVehicle->GetSteerResponse(steerResponse);}
	PxRigidBody* GetRigidBody() { return m_pPhysXActorVehicle->getActor(); }
private:
	HRESULT CreateActor();
	
	void initMaterialFrictionTable();
private:
	//CVehicleDefault* m_pVehicle = nullptr;
	PxVehiclePhysXSimulationContext m_pVehicleSimulationContext;

	PhysXActorVehicle* m_pPhysXActorVehicle = nullptr;

	VEHICLE_COMMAND m_Command;
	_uint m_WheelCount = 0;

#ifdef _DEBUG
	PhysX_Vehicle_Editable_Desc m_OutDesc;
#endif
public:
	static CPhysXComponent_Vehicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END
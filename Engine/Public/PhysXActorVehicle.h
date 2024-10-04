#pragma once


#include"Base.h"
#include"BaseVehicle.h"

struct PhysXIntegrationParams
{
	PxVehiclePhysXRoadGeometryQueryParams physxRoadGeometryQueryParams;
	PxVehiclePhysXMaterialFrictionParams physxMaterialFrictionParams[PxVehicleLimits::eMAX_NB_WHEELS];
	PxVehiclePhysXSuspensionLimitConstraintParams physxSuspensionLimitConstraintParams[PxVehicleLimits::eMAX_NB_WHEELS];
	PxTransform physxActorCMassLocalPose;
	PxVec3 physxActorBoxShapeHalfExtents;
	PxTransform physxActorBoxShapeLocalPose;
	PxTransform physxWheelShapeLocalPoses[PxVehicleLimits::eMAX_NB_WHEELS];

	void create(const PxVehicleAxleDescription& axleDescription,
		const PxQueryFilterData& queryFilterData, 
		PxQueryFilterCallback* queryFilterCallback,
		PxVehiclePhysXMaterialFriction* materialFrictions,
		const PxU32 nbMaterialFrictions,
		const PxReal defaultFriction,
		const PxTransform& actorCMassLocalPose,
		const PxVec3& actorBoxShapeHalfExtents,
		const PxTransform& actorBoxShapeLocalPose);

	PhysXIntegrationParams transformAndScale(const PxVehicleFrame& srcFrame,
		const PxVehicleFrame& trgFrame,
		const PxVehicleScale& srcScale,
		const PxVehicleScale& trgScale) const;

	PX_FORCE_INLINE bool isValid(const PxVehicleAxleDescription& axleDesc) const
	{
		if (!physxRoadGeometryQueryParams.isValid())
			return false;
		for (PxU32 i = 0; i < axleDesc.nbWheels; i++)
		{
			const PxU32 wheelId = axleDesc.wheelIdsInAxleOrder[i];
			if (!physxMaterialFrictionParams[wheelId].isValid())
				return false;
			if (!physxSuspensionLimitConstraintParams[wheelId].isValid())
				return false;
		}
		return true;
	}
};



struct  PhysXIntegrationState
{
    PxVehiclePhysXActor physxActor;
    PxVehiclePhysXSteerState physxSteerState;
    PxVehiclePhysXConstraints physxConstraints; //제약조건    //필요할까?  너무 큰데..

    PX_FORCE_INLINE void setToDefault()
    {
        physxActor.setToDefault();
        physxSteerState.setToDefault();
        physxConstraints.setToDefault();
    }
	void create(const BaseVehicleParams& baseParams,
		const PhysXIntegrationParams& physxParams,
		PxPhysics& physics,
		const PxCookingParams& params,
		PxMaterial& defaultMaterial);

	void destroy();

};

void setPhysXIntegrationParams(const PxVehicleAxleDescription& axleDescription,
								PxVehiclePhysXMaterialFriction* physXMaterialFrictions,
								PxU32 nbPhysXMaterialFrictions,
								PxReal physXDefaultMaterialFriction,
								PhysXIntegrationParams& physXParams);














BEGIN(Engine)
class ENGINE_DLL PhysXActorVehicle
    :public BaseVehicle
    , public PxVehiclePhysXActorBeginComponent
    , public PxVehiclePhysXActorEndComponent
    , public PxVehiclePhysXConstraintComponent
    , public PxVehiclePhysXRoadGeometrySceneQueryComponent

	//나중에 차량의 종류??가 바껴야 한다면 PhysXActorVehicle상속을 시켜서 자식클래스에서 아래 클래스들을 상속 받기
	, public PxVehicleDirectDriveCommandResponseComponent
	, public PxVehicleDirectDriveActuationStateComponent
	, public PxVehicleDirectDrivetrainComponent
{
public:
	PhysXActorVehicle() {};
    virtual ~PhysXActorVehicle() = default;

public:
    bool initialize(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, const BaseVehicleDesc& BaseDesc, void* Vehicleparams);


    void setUpActor(PxScene& scene, const PxTransform& pose, const char* vehicleName);
	PxRigidBody* getActor() { return mPhysXState.physxActor.rigidBody; }
	virtual void destroy();
	virtual void initComponentSequence(bool addPhysXBeginEndComponents);


	virtual void getDataForPhysXActorBeginComponent(
		const PxVehicleAxleDescription*& axleDescription,
		const PxVehicleCommandState*& commands,
		const PxVehicleEngineDriveTransmissionCommandState*& transmissionCommands,
		const PxVehicleGearboxParams*& gearParams,
		const PxVehicleGearboxState*& gearState,
		const PxVehicleEngineParams*& engineParams,
		PxVehiclePhysXActor*& physxActor,
		PxVehiclePhysXSteerState*& physxSteerState,
		PxVehiclePhysXConstraints*& physxConstraints,
		PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates,
		PxVehicleEngineState*& engineState);
	
	virtual void getDataForPhysXActorEndComponent(
		const PxVehicleAxleDescription*& axleDescription,
		const PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams,
		PxVehicleArrayData<const PxTransform>& wheelShapeLocalPoses,
		PxVehicleArrayData<const PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates,
		PxVehicleArrayData<const PxVehicleWheelLocalPose>& wheelLocalPoses,
		const PxVehicleGearboxState*& gearState,
		const PxReal*& throttle,
		PxVehiclePhysXActor*& physxActor);

	virtual void getDataForPhysXConstraintComponent(
		const PxVehicleAxleDescription*& axleDescription,
		const PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams,
		PxVehicleArrayData<const PxVehiclePhysXSuspensionLimitConstraintParams>& suspensionLimitParams,
		PxVehicleArrayData<const PxVehicleSuspensionState>& suspensionStates,
		PxVehicleArrayData<const PxVehicleSuspensionComplianceState>& suspensionComplianceStates,
		PxVehicleArrayData<const PxVehicleRoadGeometryState>& wheelRoadGeomStates,
		PxVehicleArrayData<const PxVehicleTireDirectionState>& tireDirectionStates,
		PxVehicleArrayData<const PxVehicleTireStickyState>& tireStickyStates,
		PxVehiclePhysXConstraints*& constraints);
	

	virtual void getDataForPhysXRoadGeometrySceneQueryComponent(
		const PxVehicleAxleDescription*& axleDescription,
		const PxVehiclePhysXRoadGeometryQueryParams*& roadGeomParams,
		PxVehicleArrayData<const PxReal>& steerResponseStates,
		const PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams,
		PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams,
		PxVehicleArrayData<const PxVehiclePhysXMaterialFrictionParams>& materialFrictionParams,
		PxVehicleArrayData<PxVehicleRoadGeometryState>& roadGeometryStates,
		PxVehicleArrayData<PxVehiclePhysXRoadGeometryQueryState>& physxRoadGeometryStates);


	/*        Drive            */

	virtual void getDataForDirectDriveCommandResponseComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleSizedArrayData<const PxVehicleBrakeCommandResponseParams>& brakeResponseParams,
		const PxVehicleDirectDriveThrottleCommandResponseParams*& throttleResponseParams,
		const PxVehicleSteerCommandResponseParams*& steerResponseParams,
		PxVehicleSizedArrayData<const PxVehicleAckermannParams>& ackermannParams,
		const PxVehicleCommandState*& commands, const PxVehicleDirectDriveTransmissionCommandState*& transmissionCommands,
		const PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<PxReal>& brakeResponseStates, PxVehicleArrayData<PxReal>& throttleResponseStates,
		PxVehicleArrayData<PxReal>& steerResponseStates);


	virtual void getDataForDirectDriveActuationStateComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleArrayData<const PxReal>& brakeResponseStates,
		PxVehicleArrayData<const PxReal>& throttleResponseStates,
		PxVehicleArrayData<PxVehicleWheelActuationState>& actuationStates);


	virtual void getDataForDirectDrivetrainComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleArrayData<const PxReal>& brakeResponseStates,
		PxVehicleArrayData<const PxReal>& throttleResponseStates,
		PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams,
		PxVehicleArrayData<const PxVehicleWheelActuationState>& actuationStates,
		PxVehicleArrayData<const PxVehicleTireForce>& tireForces,
		PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates);











	PhysXIntegrationParams& getPhysXParams() { return mPhysXParams; }
	PhysXIntegrationState& getPhysXState() { return mPhysXState; }
	PxVehicleCommandState& getCommandState() { return mCommandState; }
	PxVehicleDirectDriveTransmissionCommandState& getTransmissionCommandState() { return mTransmissionCommandState; }
	DirectDrivetrainParams& getDirectDriveParams() { return mDirectDriveParams; }
	void GetSteerResponse(PxVehicleSteerCommandResponseParams*& steerResponse) { steerResponse = &mBaseParams.steerResponseParams; }
private:
	HRESULT ReadDescroption(const BaseVehicleDesc& BaseDesc);


	//////////////////////////////
	//Read the high level params
	//////////////////////////////
	HRESULT ReadAxleDesc(const AxleDescription& AxleDesc);
	HRESULT ReadFrameDesc(const VehicleFrame& FrameDesc);
	HRESULT ReadScaleDesc(const VehicleScale& ScaleDesc);

	//////////////////////////////
	//Read the rigid body params
	/////////////////////////////
	HRESULT ReadRigidDesc(const VehicleRigidBody& RigidDesc);

	//////////////////////////////
	//Read the suspension state calculation params.
	//////////////////////////////
	HRESULT ReadSuspensionStateCalculation(const VehicleSuspensionStateCalculation& SuspensionStateCalculation);


	///////////////////////////////
	//Read the command responses
	///////////////////////////////
	HRESULT ReadBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse);
	HRESULT ReadHandBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse);
	HRESULT ReadSteerResponse(const VehicleBrakeCommandResponse& SteerResponse);
	//HRESULT ReadBrakeCommandResponse(const PxVehicleAxleDescription& AxelDesc,_uint ArrayNum, const VehicleBrakeCommandResponse& BrakeCommandResponse);

	HRESULT ReadAckermann(const VehicleAckermann& Ackermann);

	///////////////////////////////////
	//Read the suspension params
	///////////////////////////////////
	HRESULT ReadSuspensionDesc(const VehicleSuspension* SuspensionDesc);
	HRESULT ReadSuspensionComplianceDesc(const VehicleSuspensionCompiance* SuspensionCompliance);
	HRESULT ReadSuspensionForce(const VehicleSuspensionForce* SuspensionForce);

	///////////////////////////////////
	//Read the tire params
	///////////////////////////////////
	HRESULT ReadTireForceDesc(const VehicleTireForce* TireDesc);

	//////////////////////////
	//Read the wheel params
	//////////////////////////
	HRESULT ReadWheelDesc(const VehicleWheel* WheelDesc);

	
	HRESULT ReadThottleResponse(const DirectDrivetrainParams& ThrottleResponse);
private:
	PhysXIntegrationParams mPhysXParams;
	PhysXIntegrationState mPhysXState;

	PxVehicleCommandState mCommandState;

	

	DirectDrivetrainParams mDirectDriveParams;
	DirectDrivetrainState mDirectDriveState;

	PxVehicleDirectDriveTransmissionCommandState mTransmissionCommandState;
public:
	static PhysXActorVehicle* Create(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, const BaseVehicleDesc& BaseDesc, void* Vehicleparams = nullptr);
	
	_uint Release();

};

END
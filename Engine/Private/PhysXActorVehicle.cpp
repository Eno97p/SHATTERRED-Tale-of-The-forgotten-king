#include "PhysXActorVehicle.h"


#pragma region PhysXIntegrationParams
void PhysXIntegrationParams::create(const PxVehicleAxleDescription& axleDescription,
	const PxQueryFilterData& queryFilterData,
	PxQueryFilterCallback* queryFilterCallback,
	PxVehiclePhysXMaterialFriction* materialFrictions,
	const PxU32 nbMaterialFrictions,
	const PxReal defaultFriction,
	const PxTransform& actorCMassLocalPose,
	const PxVec3& actorBoxShapeHalfExtents,
	const PxTransform& actorBoxShapeLocalPose)
{
	physxRoadGeometryQueryParams.roadGeometryQueryType = PxVehiclePhysXRoadGeometryQueryType::eRAYCAST;
	physxRoadGeometryQueryParams.defaultFilterData = queryFilterData;
	physxRoadGeometryQueryParams.filterCallback = queryFilterCallback;
	physxRoadGeometryQueryParams.filterDataEntries = NULL;

	for (PxU32 i = 0; i < axleDescription.nbWheels; i++)
	{
		const PxU32  wheelId = axleDescription.wheelIdsInAxleOrder[i];
		physxMaterialFrictionParams[wheelId].defaultFriction = defaultFriction;
		physxMaterialFrictionParams[wheelId].materialFrictions = materialFrictions;
		physxMaterialFrictionParams[wheelId].nbMaterialFrictions = nbMaterialFrictions;

		physxSuspensionLimitConstraintParams[wheelId].restitution = 0.0f;
		physxSuspensionLimitConstraintParams[wheelId].directionForSuspensionLimitConstraint = PxVehiclePhysXSuspensionLimitConstraintParams::eROAD_GEOMETRY_NORMAL;

		physxWheelShapeLocalPoses[wheelId] = PxTransform(PxIdentity);
	}

	physxActorCMassLocalPose = actorCMassLocalPose;
	physxActorBoxShapeHalfExtents = actorBoxShapeHalfExtents;
	physxActorBoxShapeLocalPose = actorBoxShapeLocalPose;

}

PhysXIntegrationParams PhysXIntegrationParams::transformAndScale(const PxVehicleFrame& srcFrame,
	const PxVehicleFrame& trgFrame,
	const PxVehicleScale& srcScale,
	const PxVehicleScale& trgScale) const
{

	PhysXIntegrationParams r = *this;
	r.physxRoadGeometryQueryParams = physxRoadGeometryQueryParams.transformAndScale(srcFrame, trgFrame, srcScale, trgScale);
	for (PxU32 i = 0; i < PxVehicleLimits::eMAX_NB_WHEELS; i++)
	{
		r.physxSuspensionLimitConstraintParams[i] = physxSuspensionLimitConstraintParams[i].transformAndScale(srcFrame, trgFrame, srcScale, trgScale);
	}
	r.physxActorCMassLocalPose = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorCMassLocalPose);
	r.physxActorBoxShapeHalfExtents = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorBoxShapeHalfExtents);
	r.physxActorBoxShapeLocalPose = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorBoxShapeLocalPose);
	return r;

}
#pragma endregion




#pragma region PhysXIntegrationState
void PhysXIntegrationState::create(const BaseVehicleParams& baseParams, const PhysXIntegrationParams& physxParams, PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial)
{
	setToDefault();


	//피직스에서 제공하는 기본적인 설정
	const PxVehiclePhysXRigidActorParams physxActorParams(baseParams.rigidBodyParams, NULL);
	const PxBoxGeometry boxGeom(physxParams.physxActorBoxShapeHalfExtents);
	const PxVehiclePhysXRigidActorShapeParams physxActorShapeParams(boxGeom, physxParams.physxActorBoxShapeLocalPose, defaultMaterial, PxShapeFlags(/*PxShapeFlag::eSIMULATION_SHAPE*/ ), PxFilterData(), PxFilterData());
	const PxVehiclePhysXWheelParams physxWheelParams(baseParams.axleDescription, baseParams.wheelParams);
	const PxVehiclePhysXWheelShapeParams physxWheelShapeParams(defaultMaterial, PxShapeFlags(0), PxFilterData(), PxFilterData());
	
	PxVehiclePhysXActorCreate(baseParams.frame,
		physxActorParams,
		physxParams.physxActorCMassLocalPose,
		physxActorShapeParams,
		physxWheelParams,
		physxWheelShapeParams,
		physics, params,
		physxActor);

	physxActor.rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//physxActor.rigidBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);

	//physxConstraints needs to be populated with constraints.
	PxVehicleConstraintsCreate(baseParams.axleDescription, physics, *physxActor.rigidBody, physxConstraints);
}


void PhysXIntegrationState::destroy()
{

	PxVehicleConstraintsDestroy(physxConstraints);
	PxVehiclePhysXActorDestroy(physxActor);
}

#pragma endregion



#pragma region PhysXActorVehicle
bool PhysXActorVehicle::initialize(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, const BaseVehicleDesc& BaseDesc, void* Vehicleparams)
{
	if (Vehicleparams != nullptr)
	{
		void** pVehicleparams = static_cast<void**>(Vehicleparams);
		mBaseParams = *static_cast<BaseVehicleParams*>(pVehicleparams[0]);
		mDirectDriveParams = *static_cast<DirectDrivetrainParams*>(pVehicleparams[1]);
	}
	else
	{
		if (FAILED(ReadDescroption(BaseDesc)))
			return false;
	}
	



	setPhysXIntegrationParams(mBaseParams.axleDescription,
		physXMaterialFrictions,
		nbPhysXMaterialFrictions,
		physXDefaultMaterialFriction,
		mPhysXParams);

	
	mTransmissionCommandState.setToDefault();

	mCommandState.setToDefault();

	if (!BaseVehicle::initialize())
		return false;

	if (!mPhysXParams.isValid(mBaseParams.axleDescription))
		return false;

	mPhysXState.create(mBaseParams, mPhysXParams, physics, params, defaultMaterial);



	initComponentSequence(true);
	return true;
}

void PhysXActorVehicle::setUpActor(PxScene& scene, const PxTransform& pose, const char* vehicleName)
{
	mPhysXState.physxActor.rigidBody->setGlobalPose(pose);
	scene.addActor(*mPhysXState.physxActor.rigidBody);

	if(vehicleName != nullptr)
		mPhysXState.physxActor.rigidBody->setName(vehicleName);
}
void PhysXActorVehicle::destroy()
{
	mPhysXState.destroy();

	BaseVehicle::destroy();
}

void PhysXActorVehicle::initComponentSequence(bool addPhysXBeginEndComponents)
{
	if (addPhysXBeginEndComponents)
		mComponentSequence.add(static_cast<PxVehiclePhysXActorBeginComponent*>(this));

	mComponentSequence.add(static_cast<PxVehicleDirectDriveCommandResponseComponent*>(this));
	mComponentSequence.add(static_cast<PxVehicleDirectDriveActuationStateComponent*>(this));

	mComponentSequence.add(static_cast<PxVehiclePhysXRoadGeometrySceneQueryComponent*>(this));

	//mComponentSequenceSubstepGroupHandle = mComponentSequence.beginSubstepGroup(3);
	mComponentSequence.add(static_cast<PxVehicleSuspensionComponent*>(this));
	mComponentSequence.add(static_cast<PxVehicleTireComponent*>(this));
	mComponentSequence.add(static_cast<PxVehiclePhysXConstraintComponent*>(this));
	mComponentSequence.add(static_cast<PxVehicleDirectDrivetrainComponent*>(this));
	mComponentSequence.add(static_cast<PxVehicleRigidBodyComponent*>(this));
	//mComponentSequence.endSubstepGroup();

	mComponentSequence.add(static_cast<PxVehicleWheelComponent*>(this));

	if (addPhysXBeginEndComponents)
		mComponentSequence.add(static_cast<PxVehiclePhysXActorEndComponent*>(this));
}

void PhysXActorVehicle::getDataForPhysXActorBeginComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleCommandState*& commands, const PxVehicleEngineDriveTransmissionCommandState*& transmissionCommands, const PxVehicleGearboxParams*& gearParams, const PxVehicleGearboxState*& gearState, const PxVehicleEngineParams*& engineParams, PxVehiclePhysXActor*& physxActor, PxVehiclePhysXSteerState*& physxSteerState, PxVehiclePhysXConstraints*& physxConstraints, PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates, PxVehicleEngineState*& engineState)
{
		axleDescription = &mBaseParams.axleDescription;
		commands = &mCommandState;
		physxActor = &mPhysXState.physxActor;
		physxSteerState = &mPhysXState.physxSteerState;
		physxConstraints = &mPhysXState.physxConstraints;
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);

		transmissionCommands = NULL;
		gearParams = NULL;
		gearState = NULL;
		engineParams = NULL;
		engineState = NULL;

}

void PhysXActorVehicle::getDataForPhysXActorEndComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams, PxVehicleArrayData<const PxTransform>& wheelShapeLocalPoses, PxVehicleArrayData<const PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates, PxVehicleArrayData<const PxVehicleWheelLocalPose>& wheelLocalPoses, const PxVehicleGearboxState*& gearState, const PxReal*& throttle, PxVehiclePhysXActor*& physxActor)
{
	
		axleDescription = &mBaseParams.axleDescription;
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelParams.setData(mBaseParams.wheelParams);
		wheelShapeLocalPoses.setData(mPhysXParams.physxWheelShapeLocalPoses);
		wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);
		wheelLocalPoses.setData(mBaseState.wheelLocalPoses);
		physxActor = &mPhysXState.physxActor;

		gearState = NULL;
		throttle = &mCommandState.throttle;
	

}

void PhysXActorVehicle::getDataForPhysXConstraintComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams, PxVehicleArrayData<const PxVehiclePhysXSuspensionLimitConstraintParams>& suspensionLimitParams, PxVehicleArrayData<const PxVehicleSuspensionState>& suspensionStates, PxVehicleArrayData<const PxVehicleSuspensionComplianceState>& suspensionComplianceStates, PxVehicleArrayData<const PxVehicleRoadGeometryState>& wheelRoadGeomStates, PxVehicleArrayData<const PxVehicleTireDirectionState>& tireDirectionStates, PxVehicleArrayData<const PxVehicleTireStickyState>& tireStickyStates, PxVehiclePhysXConstraints*& constraints)
{
	
		axleDescription = &mBaseParams.axleDescription;
		rigidBodyState = &mBaseState.rigidBodyState;
		suspensionParams.setData(mBaseParams.suspensionParams);
		suspensionLimitParams.setData(mPhysXParams.physxSuspensionLimitConstraintParams);
		suspensionStates.setData(mBaseState.suspensionStates);
		suspensionComplianceStates.setData(mBaseState.suspensionComplianceStates);
		wheelRoadGeomStates.setData(mBaseState.roadGeomStates);
		tireDirectionStates.setData(mBaseState.tireDirectionStates);
		tireStickyStates.setData(mBaseState.tireStickyStates);
		constraints = &mPhysXState.physxConstraints;
	
}

void PhysXActorVehicle::getDataForPhysXRoadGeometrySceneQueryComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehiclePhysXRoadGeometryQueryParams*& roadGeomParams, PxVehicleArrayData<const PxReal>& steerResponseStates, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams, PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams, PxVehicleArrayData<const PxVehiclePhysXMaterialFrictionParams>& materialFrictionParams, PxVehicleArrayData<PxVehicleRoadGeometryState>& roadGeometryStates, PxVehicleArrayData<PxVehiclePhysXRoadGeometryQueryState>& physxRoadGeometryStates)
{
	
		axleDescription = &mBaseParams.axleDescription;
		roadGeomParams = &mPhysXParams.physxRoadGeometryQueryParams;
		steerResponseStates.setData(mBaseState.steerCommandResponseStates);
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelParams.setData(mBaseParams.wheelParams);
		suspensionParams.setData(mBaseParams.suspensionParams);
		materialFrictionParams.setData(mPhysXParams.physxMaterialFrictionParams);
		roadGeometryStates.setData(mBaseState.roadGeomStates);
		physxRoadGeometryStates.setEmpty();
	
}

void PhysXActorVehicle::getDataForDirectDriveCommandResponseComponent(const PxVehicleAxleDescription*& axleDescription, PxVehicleSizedArrayData<const PxVehicleBrakeCommandResponseParams>& brakeResponseParams, const PxVehicleDirectDriveThrottleCommandResponseParams*& throttleResponseParams, const PxVehicleSteerCommandResponseParams*& steerResponseParams, PxVehicleSizedArrayData<const PxVehicleAckermannParams>& ackermannParams, const PxVehicleCommandState*& commands, const PxVehicleDirectDriveTransmissionCommandState*& transmissionCommands, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<PxReal>& brakeResponseStates, PxVehicleArrayData<PxReal>& throttleResponseStates, PxVehicleArrayData<PxReal>& steerResponseStates)
{
	axleDescription = &mBaseParams.axleDescription;
	brakeResponseParams.setDataAndCount(mBaseParams.brakeResponseParams, sizeof(mBaseParams.brakeResponseParams) / sizeof(PxVehicleBrakeCommandResponseParams));
	throttleResponseParams = &mDirectDriveParams.directDriveThrottleResponseParams;
	steerResponseParams = &mBaseParams.steerResponseParams;
	ackermannParams.setDataAndCount(mBaseParams.ackermannParams, sizeof(mBaseParams.ackermannParams) / sizeof(PxVehicleAckermannParams));
	commands = &mCommandState;
	transmissionCommands = &mTransmissionCommandState;
	rigidBodyState = &mBaseState.rigidBodyState;
	brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
	throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
	steerResponseStates.setData(mBaseState.steerCommandResponseStates);
}

void PhysXActorVehicle::getDataForDirectDriveActuationStateComponent(const PxVehicleAxleDescription*& axleDescription, PxVehicleArrayData<const PxReal>& brakeResponseStates, PxVehicleArrayData<const PxReal>& throttleResponseStates, PxVehicleArrayData<PxVehicleWheelActuationState>& actuationStates)
{
	axleDescription = &mBaseParams.axleDescription;
	brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
	throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
	actuationStates.setData(mBaseState.actuationStates);
}

void PhysXActorVehicle::getDataForDirectDrivetrainComponent(const PxVehicleAxleDescription*& axleDescription, PxVehicleArrayData<const PxReal>& brakeResponseStates, PxVehicleArrayData<const PxReal>& throttleResponseStates, PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams, PxVehicleArrayData<const PxVehicleWheelActuationState>& actuationStates, PxVehicleArrayData<const PxVehicleTireForce>& tireForces, PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates)
{
	axleDescription = &mBaseParams.axleDescription;
	brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
	throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
	wheelParams.setData(mBaseParams.wheelParams);
	actuationStates.setData(mBaseState.actuationStates);
	tireForces.setData(mBaseState.tireForces);
	wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);
}

PhysXActorVehicle* PhysXActorVehicle::Create(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, const BaseVehicleDesc& BaseDesc, void* Vehicleparams)
{
	PhysXActorVehicle* pInstance = new PhysXActorVehicle();

	if (!pInstance->initialize(physics, params, defaultMaterial, physXMaterialFrictions, nbPhysXMaterialFrictions, physXDefaultMaterialFriction, BaseDesc, Vehicleparams))
	{
		MSG_BOX("Failed To Created : CVehicleDefault");
		Safe_Vehicle_Release(pInstance);
		return nullptr;

	}
		return pInstance;


}

_uint PhysXActorVehicle::Release()
{

	destroy();
	delete this;


	return 0;
}

HRESULT PhysXActorVehicle::ReadDescroption(const BaseVehicleDesc& BaseDesc)
{
	
	
	if(FAILED(ReadAxleDesc(BaseDesc.axleDesc)))
		return E_FAIL;
	if (FAILED(ReadFrameDesc(BaseDesc.frameDesc)))
		return E_FAIL;
	if (FAILED(ReadScaleDesc(BaseDesc.scaleDesc)))
		return E_FAIL;
	if (FAILED(ReadRigidDesc(BaseDesc.rigidBodyDesc)))
		return E_FAIL;
	if (FAILED(ReadSuspensionStateCalculation(BaseDesc.suspensionStateCalculationDesc)))
		return E_FAIL;
	if (FAILED(ReadBrakeResponse(BaseDesc.brakeCommandResponseDesc[0])))
		return E_FAIL;
	if (FAILED(ReadHandBrakeResponse(BaseDesc.brakeCommandResponseDesc[1])))
		return E_FAIL;
	if (FAILED(ReadSteerResponse(BaseDesc.steerCommandResponseDesc)))
		return E_FAIL;
	if (FAILED(ReadAckermann(BaseDesc.ackermannDesc)))
		return E_FAIL;
	if (FAILED(ReadSuspensionDesc(BaseDesc.suspensionDesc)))
		return E_FAIL;
	if (FAILED(ReadSuspensionComplianceDesc(BaseDesc.suspensionCompianceDesc)))
		return E_FAIL;
	if (FAILED(ReadSuspensionForce(BaseDesc.suspensionForceDesc)))
		return E_FAIL;
	if (FAILED(ReadTireForceDesc(BaseDesc.tireForceDesc)))
		return E_FAIL;
	if (FAILED(ReadWheelDesc(BaseDesc.wheelDesc)))
		return E_FAIL;
	if (FAILED(ReadThottleResponse(BaseDesc.directDrivetrainParamsDesc)))
		return E_FAIL;
	


	//FrameDesc
	//BaseDesc.frameDesc.







	return S_OK;
}

HRESULT PhysXActorVehicle::ReadAxleDesc(const AxleDescription& AxleDesc)
{
	mBaseParams.axleDescription.setToDefault();
	

	//AxleDesc
	PxU32 AxelCounts = AxleDesc.AxleCount;

	if (AxelCounts > PxVehicleLimits::eMAX_NB_AXLES)
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}

	for (PxU32 i = 0; i < AxelCounts; i++)	//축갯수
	{
		const PxU32 nbWheelIds = AxleDesc.WheelCounts[i];
		if (nbWheelIds > PxVehicleLimits::eMAX_NB_WHEELS)
		{
			MSG_BOX("Failed To Read Description: Too many wheels on axle");
			return E_FAIL;
		}

		PxU32 axleWheelIds[PxVehicleLimits::eMAX_NB_WHEELS] = { 0 };
		for (PxU32 j = 0; j < nbWheelIds; j++)
		{
			axleWheelIds[j] = AxleDesc.WheelIds[i][j];
		}
		mBaseParams.axleDescription.addAxle(nbWheelIds, axleWheelIds);

	}

	return S_OK;
}

HRESULT PhysXActorVehicle::ReadFrameDesc(const VehicleFrame& FrameDesc)
{
	mBaseParams.frame.setToDefault();

	const PxU32 lngAxis = FrameDesc.lngAxis;
	const PxU32 latAxis = FrameDesc.latAxis;
	const PxU32 vrtAxis = FrameDesc.vrtAxis;

	if ((lngAxis == latAxis) || (lngAxis == vrtAxis) || (latAxis == vrtAxis))
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}
	mBaseParams.frame.lngAxis = static_cast<PxVehicleAxes::Enum>(lngAxis);
	mBaseParams.frame.latAxis = static_cast<PxVehicleAxes::Enum>(latAxis);
	mBaseParams.frame.vrtAxis = static_cast<PxVehicleAxes::Enum>(vrtAxis);



	return S_OK;
}

HRESULT PhysXActorVehicle::ReadScaleDesc(const VehicleScale& ScaleDesc)
{
	mBaseParams.scale.setToDefault();

	if(ScaleDesc.scale < 0.0f)
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}
	mBaseParams.scale.scale = ScaleDesc.scale;
	return S_OK;
}

HRESULT PhysXActorVehicle::ReadRigidDesc(const VehicleRigidBody& RigidDesc)
{
	

	mBaseParams.rigidBodyParams.mass = RigidDesc.mass;
	if(!RigidDesc.moi.isFinite())
		return E_FAIL;
	mBaseParams.rigidBodyParams.moi = RigidDesc.moi;


	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSuspensionStateCalculation(const VehicleSuspensionStateCalculation& SuspensionStateCalculation)
{
	

	mBaseParams.suspensionStateCalculationParams.suspensionJounceCalculationType = static_cast<PxVehicleSuspensionJounceCalculationType::Enum>(SuspensionStateCalculation.JounceCalculationType);
	mBaseParams.suspensionStateCalculationParams.limitSuspensionExpansionVelocity = SuspensionStateCalculation.LimitSuspensionExpansionVelocity;

	return S_OK;
}

HRESULT PhysXActorVehicle::ReadBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse)
{
	mBaseParams.brakeResponseParams[0].maxResponse = BrakeResponse.maxResponse;

	const PxU32 nbWheels = mBaseParams.axleDescription.nbWheels;

	const PxU32 Size = BrakeResponse.numWheelsMulipliers;

	if (nbWheels < Size)
		return E_FAIL;

	if (Size > PxVehicleLimits::eMAX_NB_WHEELS)
		return E_FAIL;

	for (PxU32 i = 0; i < Size; i++)
	{
		mBaseParams.brakeResponseParams[0].wheelResponseMultipliers[i] = BrakeResponse.wheelResponseMultipliers[i];
	}

	if (BrakeResponse.nonlinearResponse.nbCommandValues > 0)
	{
		mBaseParams.brakeResponseParams[0].nonlinearResponse = BrakeResponse.nonlinearResponse;


	}

	return S_OK;

}

HRESULT PhysXActorVehicle::ReadHandBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse)
{

	mBaseParams.brakeResponseParams[1].maxResponse = BrakeResponse.maxResponse;

	const PxU32 nbWheels = mBaseParams.axleDescription.nbWheels;

	const PxU32 Size = BrakeResponse.numWheelsMulipliers;

	if (nbWheels < Size)
		return E_FAIL;

	if (Size > PxVehicleLimits::eMAX_NB_WHEELS)
		return E_FAIL;

	for (PxU32 i = 0; i < Size; i++)
	{
		mBaseParams.brakeResponseParams[1].wheelResponseMultipliers[i] = BrakeResponse.wheelResponseMultipliers[i];
	}

	if (BrakeResponse.nonlinearResponse.nbCommandValues > 0)
	{
		mBaseParams.brakeResponseParams[1].nonlinearResponse = BrakeResponse.nonlinearResponse;


	}


	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSteerResponse(const VehicleBrakeCommandResponse& SteerResponse)
{
	mBaseParams.steerResponseParams.maxResponse = SteerResponse.maxResponse;

	const PxU32 nbWheels = mBaseParams.axleDescription.nbWheels;

	const PxU32 Size = SteerResponse.numWheelsMulipliers;

	if (nbWheels < Size)
		return E_FAIL;

	if (Size > PxVehicleLimits::eMAX_NB_WHEELS)
		return E_FAIL;

	for (PxU32 i = 0; i < Size; i++)
	{
		mBaseParams.steerResponseParams.wheelResponseMultipliers[i] = SteerResponse.wheelResponseMultipliers[i];
	}

	if (SteerResponse.nonlinearResponse.nbCommandValues > 0)
	{
		mBaseParams.steerResponseParams.nonlinearResponse = SteerResponse.nonlinearResponse;
	}
	

	return S_OK;
}

HRESULT PhysXActorVehicle::ReadAckermann(const VehicleAckermann& Ackermann)
{
	mBaseParams.ackermannParams->wheelBase = Ackermann.wheelBase;
	mBaseParams.ackermannParams->trackWidth = Ackermann.trackWidth;
	mBaseParams.ackermannParams->strength = Ackermann.strength;


	for(PxU32 i = 0; i < 2; i++)
	{
		mBaseParams.ackermannParams->wheelIds[i] = Ackermann.wheelIds[i];
	}
	


	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSuspensionDesc(const VehicleSuspension* SuspensionDesc)
{
	const PxU32 Wheels = mBaseParams.axleDescription.getNbWheels();

	for (PxU32 i = 0; i < Wheels; i++)
	{
		mBaseParams.suspensionParams[i].suspensionAttachment = SuspensionDesc[i].suspensionAttachment;
		mBaseParams.suspensionParams[i].suspensionTravelDir = SuspensionDesc[i].suspensionTravelDir;
		mBaseParams.suspensionParams[i].suspensionTravelDist = SuspensionDesc[i].suspensionTravelDist;
		mBaseParams.suspensionParams[i].wheelAttachment = SuspensionDesc[i].wheelAttachment;
		



	}


	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSuspensionComplianceDesc(const VehicleSuspensionCompiance* SuspensionCompliance)
{

	const PxU32 Wheels = mBaseParams.axleDescription.getNbWheels();

	for (PxU32 i = 0; i < Wheels; i++)
	{
		mBaseParams.suspensionComplianceParams[i].suspForceAppPoint = SuspensionCompliance[i].suspForceAppPoint;
		mBaseParams.suspensionComplianceParams[i].tireForceAppPoint = SuspensionCompliance[i].tireForceAppPoint;
		mBaseParams.suspensionComplianceParams[i].wheelCamberAngle = SuspensionCompliance[i].wheelCamberAngle;
		mBaseParams.suspensionComplianceParams[i].wheelToeAngle = SuspensionCompliance[i].wheelToeAngle;



	}



	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSuspensionForce(const VehicleSuspensionForce* SuspensionForce)
{
	const PxU32 Wheels = mBaseParams.axleDescription.getNbWheels();

	for (PxU32 i = 0; i < Wheels; i++)
	{
		mBaseParams.suspensionForceParams[i].damping = SuspensionForce[i].damping;
		mBaseParams.suspensionForceParams[i].sprungMass = SuspensionForce[i].sprungMass;
		mBaseParams.suspensionForceParams[i].stiffness = SuspensionForce[i].stiffness;

	}




	return S_OK;
}

HRESULT PhysXActorVehicle::ReadTireForceDesc(const VehicleTireForce* TireDesc)
{
	const PxU32 Wheels = mBaseParams.axleDescription.getNbWheels();

	for (PxU32 i = 0; i < Wheels; i++)
	{
		mBaseParams.tireForceParams[i].latStiffX = TireDesc[i].latStiffX;
		mBaseParams.tireForceParams[i].latStiffY = TireDesc[i].latStiffY;
		mBaseParams.tireForceParams[i].longStiff = TireDesc[i].longStiff;
		mBaseParams.tireForceParams[i].camberStiff = TireDesc[i].camberStiff;
		mBaseParams.tireForceParams[i].restLoad = TireDesc[i].restLoad;

		for (PxU32 j = 0; j < 3; j++)
		{
			mBaseParams.tireForceParams[i].frictionVsSlip[j][0] = TireDesc[i].frictionVsSlip[j][0];
			mBaseParams.tireForceParams[i].frictionVsSlip[j][1] = TireDesc[i].frictionVsSlip[j][1];
		}
		for (PxU32 j = 0; j < 2; j++)
		{
			mBaseParams.tireForceParams[i].loadFilter[j][0] = TireDesc[i].loadFilter[j][0];
			mBaseParams.tireForceParams[i].loadFilter[j][1] = TireDesc[i].loadFilter[j][1];
		}


	}



	return S_OK;
}

HRESULT PhysXActorVehicle::ReadWheelDesc(const VehicleWheel* WheelDesc)
{
	const PxU32 Wheels = mBaseParams.axleDescription.getNbWheels();

	for (PxU32 i = 0; i < Wheels; i++)
	{
		mBaseParams.wheelParams[i].dampingRate = WheelDesc[i].dampingRate;
		mBaseParams.wheelParams[i].halfWidth = WheelDesc[i].halfWidth;
		mBaseParams.wheelParams[i].mass = WheelDesc[i].mass;
		mBaseParams.wheelParams[i].moi = WheelDesc[i].moi;
		mBaseParams.wheelParams[i].radius = WheelDesc[i].radius;



	}



	return S_OK;
}

HRESULT PhysXActorVehicle::ReadThottleResponse(const DirectDrivetrainParams& ThrottleResponse)
{

	mDirectDriveParams.directDriveThrottleResponseParams = ThrottleResponse.directDriveThrottleResponseParams;


	return S_OK;

}









#pragma endregion




















void setPhysXIntegrationParams(const PxVehicleAxleDescription& axleDescription, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, PhysXIntegrationParams& physXParams)
{
	const PxQueryFilterData queryFilterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);
	PxQueryFilterCallback* queryFilterCallback = NULL;
	const PxTransform physxActorCMassLocalPose(PxVec3(0.0f, 0.55f, 1.594f), PxQuat(PxIdentity));
	const PxVec3 physxActorBoxShapeHalfExtents(0.84097f, 0.65458f, 2.46971f);
	const PxTransform physxActorBoxShapeLocalPose(PxVec3(0.0f, 0.830066f, 1.37003f), PxQuat(PxIdentity));


	physXParams.create(
		axleDescription,
		queryFilterData, queryFilterCallback,
		physXMaterialFrictions, nbPhysXMaterialFrictions, physXDefaultMaterialFriction,
		physxActorCMassLocalPose,
		physxActorBoxShapeHalfExtents, physxActorBoxShapeLocalPose);
}

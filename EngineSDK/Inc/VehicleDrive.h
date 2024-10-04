#pragma once
#include"Base.h"
#include"Components_Vehicle.h"
#include"PhysXActorVehicle.h"


BEGIN(Engine)

class ENGINE_DLL CVehicleDefault
	: public PhysXActorVehicle
	, public PxVehicleDirectDriveCommandResponseComponent
	, public PxVehicleDirectDriveActuationStateComponent
	, public PxVehicleDirectDrivetrainComponent
{


};

END
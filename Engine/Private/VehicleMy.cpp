#include "VehicleMy.h"

void VehicleMy::destroy()
{
	PhysXActorVehicle::destroy();
}

void VehicleMy::initComponentSequence(bool addPhysXBeginEndComponents)
{

	if (addPhysXBeginEndComponents)
		mComponentSequence.add(static_cast<PxVehiclePhysXActorBeginComponent*>(this));

	mComponentSequence.add(static_cast<PxVehiclePhysXRoadGeometrySceneQueryComponent*>(this));
}

#pragma once

#include "Base.h"
#include "PhysXActorVehicle.h"

BEGIN(Engine)
class VehicleMy : public PhysXActorVehicle
{


public:
	virtual void destroy();
	virtual void initComponentSequence(bool addPhysXBeginEndComponents);


public:


	//virtual void Free() override;
};

END

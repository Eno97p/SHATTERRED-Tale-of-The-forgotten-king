#pragma once

#include"Base.h"



BEGIN(Engine)
class ENGINE_DLL ComponentBegin : public PxVehicleComponent
{	
public:   
      ComponentBegin() = default;
      virtual ~ComponentBegin() = default;


    virtual void getDataForComponentBegin(const  VehicleBeginParams*& beginParam, PxVehicleArrayData<VehicleBeginState>& beginStates) = 0;

	virtual bool update(const PxReal dt, const PxVehicleSimulationContext& context) override;
 
	void CFunctionBegin(const VehicleBeginParams* beginParam, PxVehicleArrayData<VehicleBeginState>& beginStates, PxReal dt);


};

class ENGINE_DLL ComponentMiddle : public PxVehicleComponent
{
public:
    ComponentMiddle() = default;
	virtual ~ComponentMiddle() = default;

	virtual void getDataForComponentMiddle(const  VehicleMiddleParams*& middleParam, PxVehicleArrayData<const VehicleBeginState>& beginStates, PxVehicleArrayData<VehicleMiddleState>& middleStates) = 0;

	virtual bool update(const PxReal dt, const PxVehicleSimulationContext& context) override;

	void CFunctionMiddle(const VehicleMiddleParams* middleParam, PxVehicleArrayData<const VehicleBeginState>& beginStates, PxVehicleArrayData<VehicleMiddleState>& middleStates, PxReal dt);

};

class ENGINE_DLL ComponentEnd : public PxVehicleComponent
{
public:
    ComponentEnd() = default;
	virtual ~ComponentEnd() = default;

	virtual void getDataForComponentEnd(const  VehicleEndParams*& endParam, PxVehicleArrayData<const VehicleMiddleState>& middleStates, PxVehicleArrayData<VehicleEndState>& endStates) = 0;

	virtual bool update(const PxReal dt, const PxVehicleSimulationContext& context) override;

	void CFunctionEnd(const VehicleEndParams* endParam, PxVehicleArrayData<const VehicleMiddleState>& middleStates, PxVehicleArrayData<VehicleEndState>& endStates, PxReal dt);

};


END
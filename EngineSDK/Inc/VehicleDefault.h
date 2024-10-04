#pragma once
#include"Base.h"
#include"Components_Vehicle.h"
#include"PhysXActorVehicle.h"


BEGIN(Engine)

class ENGINE_DLL CVehicleDefault :
    public ComponentBegin,
    public ComponentMiddle,
    public ComponentEnd
    //public PxVehicleRigidBodyComponent
{
    
private:
    CVehicleDefault(PxU32 numWheels);
    virtual ~CVehicleDefault() = default;

public:
    HRESULT createSequence();
    HRESULT createActor(PxPhysics* pPhysics, PxScene* pScene,const char* Name);
    virtual bool update(const PxReal dt, const PxVehicleSimulationContext& context) override;
   

    VehicleBeginParams& getBeginParams() { return m_beginParams; }
    VehicleMiddleParams& getMiddleParams() { return m_middleParams; }
    VehicleEndParams& getEndParams() { return m_endParams; }
private:
    HRESULT Initialize();




private:
    virtual void getDataForComponentBegin(
        const VehicleBeginParams*& beginParam,
        PxVehicleArrayData<VehicleBeginState>& beginStates) override;
    virtual void getDataForComponentMiddle(
        const VehicleMiddleParams*& middleParam,
        PxVehicleArrayData<const VehicleBeginState>& beginStates,
        PxVehicleArrayData<VehicleMiddleState>& middleStates) override;
    virtual void getDataForComponentEnd(const VehicleEndParams*& endParam,
        PxVehicleArrayData<const VehicleMiddleState>& middleStates,
        PxVehicleArrayData<VehicleEndState>& endStates) override;



public:
    void updateSequence(const PxReal dt, const PxVehicleSimulationContext& context);

private:
    VehicleBeginParams m_beginParams;
    VehicleMiddleParams m_middleParams;
    VehicleEndParams m_endParams;

    
    VehicleBeginState m_beginState[MAX_NUM_WHEELS];
    VehicleMiddleState m_middleState[MAX_NUM_WHEELS];
    VehicleEndState m_endState[MAX_NUM_WHEELS];


    PxVehicleComponentSequence mSequence;
    PxU32 m_numWheels;

    PxRigidDynamic* mActor = nullptr;
protected:
    _uint		m_iRefCnt = { 0 };
public:
    static CVehicleDefault* Create(PxU32 numWheels);
    _uint AddRef();
    _uint Release();
    virtual void Free();

};

END
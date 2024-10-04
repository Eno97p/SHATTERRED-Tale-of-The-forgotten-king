#pragma once
#include"Base.h"



BEGIN(Engine)
class CSimulationCallBack : public PxSimulationEventCallback
{



public:
    //필수로 구현해야함
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)override { PX_UNUSED(constraints); PX_UNUSED(count); }
	virtual void onWake(PxActor** actors, PxU32 count)override { PX_UNUSED(actors); PX_UNUSED(count); }
	virtual void onSleep(PxActor** actors, PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override { PX_UNUSED(pairs); PX_UNUSED(count); }
	virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)override {}



protected:
    void OnCollisionEnter(const PxContactPairHeader& pairHeader, const PxContactPair& cp);


    void OnCollisionStay(const PxContactPairHeader& pairHeader, const PxContactPair& cp);
  

    void OnCollisionExit(const PxContactPairHeader& pairHeader, const PxContactPair& cp);
  



};

END
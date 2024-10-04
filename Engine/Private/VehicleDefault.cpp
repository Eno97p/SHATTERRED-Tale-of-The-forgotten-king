#include"VehicleDefault.h"
#include"GameInstance.h"

#include"CPhysX.h"

CVehicleDefault::CVehicleDefault(PxU32 numWheels)
	:m_numWheels(numWheels)
{

}

HRESULT CVehicleDefault::createSequence()
{
	if (!mSequence.add(static_cast<ComponentBegin*>(this)))
		return E_FAIL;

	if(!mSequence.add(static_cast<ComponentMiddle*>(this)))
		return E_FAIL;

	if(!mSequence.add(static_cast<ComponentEnd*>(this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVehicleDefault::createActor(PxPhysics* pPhysics, PxScene* pScene, const char* Name)
{
	//이미 있으면 실패
	if(mActor)
		return E_FAIL;

	PxVec3 position = PxVec3(0.f, 10.f, 0.f);

	mActor= pPhysics->createRigidDynamic(PxTransform(position));
	mActor->setName(Name);
	if(!mActor) //생성 실패 하면	실패
		return E_FAIL;
	mActor->setMass(m_beginParams.mass);

	PxVec3 momentOfInertia = PxVec3(
		m_beginParams.mass * (m_beginParams.dimensions.y * m_beginParams.dimensions.y + m_beginParams.dimensions.z * m_beginParams.dimensions.z) / 12,
		m_beginParams.mass * (m_beginParams.dimensions.x * m_beginParams.dimensions.x + m_beginParams.dimensions.z * m_beginParams.dimensions.z) / 12,
		m_beginParams.mass * (m_beginParams.dimensions.x * m_beginParams.dimensions.x + m_beginParams.dimensions.y * m_beginParams.dimensions.y) / 12
	);
	mActor->setMassSpaceInertiaTensor(momentOfInertia);


	PxBoxGeometry boxGeometry(m_beginParams.dimensions.x * 0.5f, m_beginParams.dimensions.y * 0.5f, m_beginParams.dimensions.z * 0.5f);
	PxMaterial* material = pPhysics->createMaterial(0.5f, 0.5f, 0.1f);
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*mActor, boxGeometry, *material);


	if(!pScene->addActor(*mActor))
		return E_FAIL;

	
	return S_OK;
}

bool CVehicleDefault::update(const PxReal dt, const PxVehicleSimulationContext& context)
{
	bool result = ComponentBegin::update(dt, context);
	if (result) result = ComponentMiddle::update(dt, context);
	if (result) result = ComponentEnd::update(dt, context);

	if (mActor)
	{
		const VehicleEndState& endState = m_endState[0];
		PxTransform pose = endState.finalPose;
		PxVec3 velocity = endState.finalVelocity;
		PxVec3 angularVelocity = endState.finalAngularVelocity;

		if (pose.isValid())
		{
			mActor->setGlobalPose(pose);
			mActor->setLinearVelocity(velocity);
			mActor->setAngularVelocity(angularVelocity);
		}
	}

	

	return result;

}




HRESULT CVehicleDefault::Initialize()
{
	m_beginParams.setToDefault();
	m_middleParams.setToDefault();
	m_endParams.setToDefault();
	for (PxU32 i = 0; i < m_numWheels; i++)
	{
		m_beginState[i].setToDefault();
		m_middleState[i].setToDefault();
		m_endState[i].setToDefault();
		m_middleState[i].bodyRotation = PxQuat(PxIdentity);
		

	}
//	m_middleState[0].bodyRotation = PxQuat(PxIdentity);

	return S_OK;
}

void CVehicleDefault::getDataForComponentBegin(const VehicleBeginParams*& beginParam, PxVehicleArrayData<VehicleBeginState>& beginStates)
{
	beginParam = &m_beginParams;
	beginStates.setData(m_beginState);
	
	
}

void CVehicleDefault::getDataForComponentMiddle(const VehicleMiddleParams*& middleParam, PxVehicleArrayData<const VehicleBeginState>& beginStates, PxVehicleArrayData<VehicleMiddleState>& middleStates)
{
	middleParam = &m_middleParams;
	beginStates.setData(m_beginState);
	middleStates.setData(m_middleState);

}

void CVehicleDefault::getDataForComponentEnd(const VehicleEndParams*& endParam, PxVehicleArrayData<const VehicleMiddleState>& middleStates, PxVehicleArrayData<VehicleEndState>& endStates)
{
	endParam = &m_endParams;
	middleStates.setData(m_middleState);
	endStates.setData(m_endState);

}


void CVehicleDefault::updateSequence(const PxReal dt, const PxVehicleSimulationContext& context)
{
	PROFILE_CALL("TestVehicle", mSequence.update(dt, context));
	
}

CVehicleDefault* CVehicleDefault::Create(PxU32 numWheels)
{
	CVehicleDefault* pInstance = new CVehicleDefault(numWheels);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CVehicleDefault");
		delete pInstance;
		pInstance = nullptr;
		return nullptr;
	}

	return pInstance;
}

_uint CVehicleDefault::AddRef()
{
	return ++m_iRefCnt;
}

_uint CVehicleDefault::Release()
{
	if (0 == m_iRefCnt)
	{
		Free();
		delete this;

		return 0;
	}

	else
		return m_iRefCnt--;
}

void CVehicleDefault::Free()
{
	Safe_physX_Release(mActor);

}


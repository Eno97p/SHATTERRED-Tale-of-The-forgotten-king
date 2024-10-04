#include"Components_Vehicle.h"



bool ComponentBegin::update(const PxReal dt, const PxVehicleSimulationContext& context)
{
   
  const VehicleBeginParams* beginParam;
  PxVehicleArrayData<VehicleBeginState> beginStates;
  getDataForComponentBegin(beginParam, beginStates);


  if (beginStates.isEmpty())
  {
	  // �����Ͱ� ������ ���� ó�� �Ǵ� early return
	  return false;
  }

  CFunctionBegin(beginParam, beginStates, dt);


  return true;
   


}

void ComponentBegin::CFunctionBegin(const VehicleBeginParams* beginParam, PxVehicleArrayData<VehicleBeginState>& beginStates, PxReal dt)
{
	PxReal mass = beginParam->mass;
	PxVec3 dimensions = beginParam->dimensions;




	VehicleBeginState& bodyState = beginStates[0];
	bodyState.transform = PxTransform(PxVec3(0, dimensions.y * 0.5f, 0));
	bodyState.linearVelocity = PxVec3(0.0f);
	bodyState.angularVelocity = PxVec3(0.0f);



	for (PxU32 i = 0; i < beginParam->numWheels; i++)
	{
		VehicleBeginState& state = beginStates[i];


		// �� ��ġ ��� (������ ����, �����δ� �� ������ �� ����)
		PxVec3 wheelPos = PxVec3(	/*X*/	(i % 2 == 0 ? -1 : 1) * dimensions.x * 0.5f,
									/*Y*/	-dimensions.y * 0.5f + beginParam->wheelRadius[i],
									/*Z*/	(i < 3 ? 1 : -1) * dimensions.z * 0.4f);
		state.transform = PxTransform(wheelPos);
		state.linearVelocity = PxVec3(0.0f);
		state.angularVelocity = PxVec3(0.0f);



	}




}

bool ComponentMiddle::update(const PxReal dt, const PxVehicleSimulationContext& context)
{
	


	//PxVehicleCommandState

	const VehicleMiddleParams* middleParam;
	PxVehicleArrayData<const VehicleBeginState> beginStates;
	PxVehicleArrayData<VehicleMiddleState> middleStates;
	getDataForComponentMiddle(middleParam, beginStates, middleStates);

	CFunctionMiddle(middleParam, beginStates, middleStates, dt);
	return true;
}

void ComponentMiddle::CFunctionMiddle(const VehicleMiddleParams* middleParam, PxVehicleArrayData<const VehicleBeginState>& beginStates, PxVehicleArrayData<VehicleMiddleState>& middleStates, PxReal dt)
{
	PxReal steeringAngle = middleParam->steeringAngle;
	const PxReal maxSteeringAngle = PxPi * 0.25f; // �ִ� ���� ���� (45��)

	steeringAngle = PxClamp(steeringAngle, -maxSteeringAngle, maxSteeringAngle);

	const VehicleBeginState& initialBodyState = beginStates[0];
	initialBodyState.transform;

	// ������ ���� ������ �������� ���ο� ȸ���� ����մϴ�
	PxQuat steeringRotation = PxQuat(steeringAngle * dt, PxVec3(0, 1, 0));
	middleStates[0].bodyRotation = steeringRotation * middleStates[0].bodyRotation;
	
	// ����ȭ�� ���� ȸ���� �ùٸ��� Ȯ���մϴ�
	middleStates[0].bodyRotation.normalize();

	
	for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
	{
		middleStates[i].wheelSpeeds[i] += middleParam->engineTorque * dt;

		if (middleStates[i].wheelSpeeds[i] > 0)
			middleStates[i].wheelSpeeds[i] = PxMax(0.0f, middleStates[i].wheelSpeeds[i] - middleParam->brakeTorque * dt);

		// �ſ� �⺻���� ������� �ùķ��̼�
		middleStates[i].suspensionCompression[i] = 0.1f; // ��� �� ���
	}
}

bool ComponentEnd::update(const PxReal dt, const PxVehicleSimulationContext& context)
{
	const VehicleEndParams* endParam;
	PxVehicleArrayData<const VehicleMiddleState> middleStates;
	PxVehicleArrayData<VehicleEndState> endStates;
	getDataForComponentEnd(endParam, middleStates, endStates);


	CFunctionEnd(endParam, middleStates, endStates, dt);
	return true;
}

void ComponentEnd::CFunctionEnd(const VehicleEndParams* endParam, PxVehicleArrayData<const VehicleMiddleState>& middleStates, PxVehicleArrayData<VehicleEndState>& endStates, PxReal dt)
{
	PxVec3 totalVelocity(0.0f);
	for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
	{
		totalVelocity.x += middleStates[i].wheelSpeeds[i];
	}
	totalVelocity.x /= MAX_NUM_WHEELS;

	if (totalVelocity.magnitude() > endParam->maxSpeed)
	{
		totalVelocity.normalize();
		totalVelocity *= endParam->maxSpeed;
	}

	VehicleEndState& bodyEndState = endStates[0];

	PxVec3 rotatedVelocity = middleStates[0].bodyRotation.rotate(totalVelocity);

	bodyEndState.finalVelocity = rotatedVelocity;
	bodyEndState.finalPose.p += rotatedVelocity * dt;

	// ���� ȸ�� ���� (���� ��� ���� �Ҵ�)
	bodyEndState.finalPose.q = middleStates[0].bodyRotation;

	// Quaternion ����ȭ
	bodyEndState.finalPose.q.normalize();

	// ���ӵ� ��� (������ ����)
	bodyEndState.finalAngularVelocity = PxVec3(0, middleStates[0].bodyRotation.getAngle() / dt, 0);
}


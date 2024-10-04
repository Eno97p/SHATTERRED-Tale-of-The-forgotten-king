#include "..\Public\PhysXComponent_Vehicle.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"

PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
PxU32 gNbPhysXMaterialFrictions = 0;
PxReal gPhysXDefaultMaterialFriction = 1.0f;

PxMaterial* gMaterial = nullptr;

CPhysXComponent_Vehicle::CPhysXComponent_Vehicle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_Vehicle::CPhysXComponent_Vehicle(const CPhysXComponent_Vehicle & rhs)
	: CPhysXComponent{ rhs }
{
}

HRESULT CPhysXComponent_Vehicle::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CPhysXComponent_Vehicle::Initialize(void * pArg)
{

	if (FAILED(CreateActor()))
		return E_FAIL;

	VEHICLE_COMMAND command = *static_cast<VEHICLE_COMMAND*>(pArg);
	_uint wheelCount = command.wheelConfig.frontWheels + command.wheelConfig.rearWheels;
	PxTransform inittransform = Convert_DxMat_To_PxTrans(command.initTransform);

	initMaterialFrictionTable();

	BaseVehicleDesc desc;
	BaseVehicleParams pBaseVehicleParams;
	DirectDrivetrainParams pDirectDrivetrainParams;
	_bool isLoad = false;
	decltype(auto) Load_data = Load_Data<BaseVehicleParams, DirectDrivetrainParams>(L"../Bin/DataFiles/Vehicle.bin");
	//optional<tuple<BaseVehicleParams, DirectDrivetrainParams>> Load_data = Load_Data<BaseVehicleParams, DirectDrivetrainParams>(L"../Bin/DataFiles/Vehicle.dat");
	if (!Load_data)
	{

		desc.setToDefault();

		desc.axleDesc.AxleCount = 2;
		desc.axleDesc.WheelCounts[0] = command.wheelConfig.frontWheels;
		desc.axleDesc.WheelCounts[1] = command.wheelConfig.rearWheels;

		for (_uint i = 0; i < command.wheelConfig.frontWheels; ++i)
			desc.axleDesc.WheelIds[0][i] = i;

		for (_uint i = 0; i < command.wheelConfig.rearWheels; ++i)
			desc.axleDesc.WheelIds[1][i] = command.wheelConfig.frontWheels + i;

	/*	desc.axleDesc.WheelIds[0][0] = 0;
		desc.axleDesc.WheelIds[0][1] = 1;
		desc.axleDesc.WheelIds[1][0] = 2;
		desc.axleDesc.WheelIds[1][1] = 3;*/

		desc.frameDesc.lngAxis = PxVehicleAxes::ePosZ;
		desc.frameDesc.latAxis = PxVehicleAxes::ePosX;
		desc.frameDesc.vrtAxis = PxVehicleAxes::ePosY;

		desc.scaleDesc.scale = 1.0f;
		desc.rigidBodyDesc.mass = 2000.0f;
		desc.rigidBodyDesc.moi = PxVec3(3200, 3200.0f, 2500.0f);		//���� ���Ʈ Y���� ���� ����	ȸ���� ����

		desc.suspensionStateCalculationDesc.JounceCalculationType = 1;
		desc.suspensionStateCalculationDesc.LimitSuspensionExpansionVelocity = false;

		// VehicleBrakeCommandResponse (Brake)
		desc.brakeCommandResponseDesc[0].maxResponse = 2500.0f;
		desc.brakeCommandResponseDesc[0].numWheelsMulipliers = wheelCount;
		for (_uint i = 0; i < wheelCount; ++i)
		{
			desc.brakeCommandResponseDesc[0].wheelResponseMultipliers[i] = 1.0f;
		}

		// �ڵ�극��ũ ���� (�޹������� ����)
		desc.brakeCommandResponseDesc[1].maxResponse = 0.0f;
		desc.brakeCommandResponseDesc[1].numWheelsMulipliers = wheelCount;
		for (_uint i = 0; i < wheelCount; ++i)
		{
			desc.brakeCommandResponseDesc[1].wheelResponseMultipliers[i] = (i < command.wheelConfig.frontWheels) ? 0.0f : 1.0f;
		}
		//// VehicleBrakeCommandResponse (Handbrake)
		//desc.brakeCommandResponseDesc[1].maxResponse = 0.0f;
		//desc.brakeCommandResponseDesc[1].wheelResponseMultipliers[0] = 0.0f;
		//desc.brakeCommandResponseDesc[1].wheelResponseMultipliers[1] = 0.0f;
		//desc.brakeCommandResponseDesc[1].wheelResponseMultipliers[2] = 1.0f;
		//desc.brakeCommandResponseDesc[1].wheelResponseMultipliers[3] = 1.0f;
		//desc.brakeCommandResponseDesc[1].numWheelsMulipliers = 4;

		// ���� ���� (�չ������� ����)
		desc.steerCommandResponseDesc.maxResponse = XMConvertToRadians(45.0f);
		desc.steerCommandResponseDesc.numWheelsMulipliers = wheelCount;
		for (int i = 0; i < wheelCount; ++i) 
		{
			desc.steerCommandResponseDesc.wheelResponseMultipliers[i] = (i < command.wheelConfig.frontWheels) ? 1.0f : 0.0f;
		}

		//// VehicleBrakeCommandResponse (Steer)
		//desc.steerCommandResponseDesc.maxResponse = /*0.5235990285873413f;*/ XMConvertToRadians(45.0f);
		//desc.steerCommandResponseDesc.wheelResponseMultipliers[0] = 1.0f;
		//desc.steerCommandResponseDesc.wheelResponseMultipliers[1] = 1.0f;
		//desc.steerCommandResponseDesc.wheelResponseMultipliers[2] = 0.0f;
		//desc.steerCommandResponseDesc.wheelResponseMultipliers[3] = 0.0f;
		//desc.steerCommandResponseDesc.numWheelsMulipliers = 4;

		if (command.wheelConfig.frontWheels >= 2)
		{
			// �չ����� 2�� �̻��� ���� Ackermann ������ ����
			desc.ackermannDesc.wheelIds[0] = 0;
			desc.ackermannDesc.wheelIds[1] = 1;
			desc.ackermannDesc.wheelBase = 2.863219976425171f;
			desc.ackermannDesc.trackWidth = 1.5510799884796143f;
			desc.ackermannDesc.strength = 1.0f;
		}
		else if (command.wheelConfig.frontWheels == 1 && command.wheelConfig.rearWheels >= 2)
		{
			// �չ����� 1���̰� �޹����� 2�� �̻��� ���� �޹����� Ackermann ������ ����
			desc.ackermannDesc.wheelIds[0] = command.wheelConfig.frontWheels;
			desc.ackermannDesc.wheelIds[1] = command.wheelConfig.frontWheels + 1;
			desc.ackermannDesc.wheelBase = 2.863219976425171f;
			desc.ackermannDesc.trackWidth = 1.5510799884796143f;
			desc.ackermannDesc.strength = 1.0f;
		}

		//// VehicleAckermann
		//desc.ackermannDesc.wheelIds[0] = 0;
		//desc.ackermannDesc.wheelIds[1] = 1;
		//desc.ackermannDesc.wheelBase = 2.863219976425171f;
		//desc.ackermannDesc.trackWidth = 1.5510799884796143f;
		//desc.ackermannDesc.strength = 1.0f;

		// VehicleSuspension
		for (_uint i = 0; i < wheelCount; ++i)
		{
			bool isFrontWheel = (i < command.wheelConfig.frontWheels);
			int wheelIndex = isFrontWheel ? i : i - command.wheelConfig.frontWheels;
			int wheelCountInAxle = isFrontWheel ? command.wheelConfig.frontWheels : command.wheelConfig.rearWheels;

			desc.suspensionDesc[i].wheelId = i;
			desc.suspensionDesc[i].suspensionTravelDir = PxVec3(0.0f, -1.0f, 0.0f);
			desc.suspensionDesc[i].suspensionTravelDist = 0.5f;
			desc.suspensionDesc[i].wheelAttachment = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));


			float xPos, zPos;
			if (wheelCountInAxle == 1)
			{
				xPos = 0.0f; // ������ 1���� ��� �߾ӿ� ��ġ
			}
			else
			{
				xPos = -0.8f + (wheelIndex * 1.6f / (wheelCountInAxle - 1)); // ������ ���� ���� ��� �յ� ����
			}
			zPos = isFrontWheel ? 1.5f : -1.5f; // �չ����� �޹����� Z ��ġ

			desc.suspensionDesc[i].suspensionAttachment = PxTransform(PxVec3(xPos, -0.1f, zPos), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));

			// ������� ���� ����
			desc.suspensionForceDesc[i].stiffness = isFrontWheel ? 49249.9501953125f * 1.5f : 39073.5f * 1.5f;
			desc.suspensionForceDesc[i].damping = isFrontWheel ? 12792.18017578125f * 1.5f : 10148.955322265625f * 1.5f;
			desc.suspensionForceDesc[i].sprungMass = isFrontWheel ? 830.6609802246094f : 659.0234985351563f;

			// Ÿ�̾� ���� ����
			desc.tireForceDesc[i].longStiff = 36787.5f;
			desc.tireForceDesc[i].latStiffX = 0.3f;
			desc.tireForceDesc[i].camberStiff = 0.0f;
			desc.tireForceDesc[i].latStiffY = isFrontWheel ? 118699.637252138f * 1.5f : 143930.84033118f * 1.5f;
			desc.tireForceDesc[i].restLoad = isFrontWheel ? 5628.72314453125f : 4604.3134765625f;


			// ���� �� ���� ���� ����
			desc.tireForceDesc[i].frictionVsSlip[0][0] = 0.0f;
			desc.tireForceDesc[i].frictionVsSlip[0][1] = 1.0f;
			desc.tireForceDesc[i].frictionVsSlip[1][0] = 0.10000000149011612f;
			desc.tireForceDesc[i].frictionVsSlip[1][1] = 1.0f;
			desc.tireForceDesc[i].frictionVsSlip[2][0] = 1.0f;
			desc.tireForceDesc[i].frictionVsSlip[2][1] = 1.0f;

			desc.tireForceDesc[i].loadFilter[0][0] = 0.0f;
			desc.tireForceDesc[i].loadFilter[0][1] = 0.23080000281333924f;
			desc.tireForceDesc[i].loadFilter[1][0] = 3.0f;
			desc.tireForceDesc[i].loadFilter[1][1] = 3.0f;



			// ���� ����
			desc.wheelDesc[i].radius = 0.29190240383148193f;
			desc.wheelDesc[i].halfWidth = 0.1892214059829712f;
			desc.wheelDesc[i].mass = 25.0f;
			desc.wheelDesc[i].moi = 1.5231969833374023f;
			desc.wheelDesc[i].dampingRate = 80.f;		//������ ���ߴ� �ӵ� 	0~ Infinit //  ���� ���� �� ������ ����
		}

		//// Suspension attachments for each wheel
		//desc.suspensionDesc[0].suspensionAttachment = PxTransform(PxVec3(-0.7952629923820496f, -0.10795199871063233f, 1.269219994544983f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
		//desc.suspensionDesc[1].suspensionAttachment = PxTransform(PxVec3(0.7952629923820496f, -0.10795000195503235f, 1.269219994544983f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
		//desc.suspensionDesc[2].suspensionAttachment = PxTransform(PxVec3(-0.7952629923820496f, -0.10795199871063233f, -1.593999981880188f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
		//desc.suspensionDesc[3].suspensionAttachment = PxTransform(PxVec3(0.7952629923820496f, -0.10795299708843231f, -1.593999981880188f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));

		// VehicleSuspensionForce
		//desc.suspensionForceDesc[0] = { 49249.9501953125f, 12792.18017578125f, 830.6609802246094f };
		//desc.suspensionForceDesc[1] = { 49249.9501953125f, 12792.18017578125f, 830.6609802246094f };
		//desc.suspensionForceDesc[2] = { 39073.5f, 10148.955322265625f, 659.0234985351563f };
		//desc.suspensionForceDesc[3] = { 39073.5f, 10148.955322265625f, 659.0234985351563f };

		//desc.suspensionForceDesc[0].stiffness = 49249.9501953125f * 1.2f;
		//desc.suspensionForceDesc[0].damping = 12792.18017578125f;
		//desc.suspensionForceDesc[0].sprungMass = 830.6609802246094f;

		//desc.suspensionForceDesc[1].stiffness = 49249.9501953125f * 1.2f;
		//desc.suspensionForceDesc[1].damping = 12792.18017578125f;
		//desc.suspensionForceDesc[1].sprungMass = 830.6609802246094f;

		//desc.suspensionForceDesc[2].stiffness = 39073.5f * 1.2f;
		//desc.suspensionForceDesc[2].damping = 10148.955322265625f;
		//desc.suspensionForceDesc[2].sprungMass = 659.0234985351563f;

		//desc.suspensionForceDesc[3].stiffness = 39073.5f * 1.2f;
		//desc.suspensionForceDesc[3].damping = 10148.955322265625f;
		//desc.suspensionForceDesc[3].sprungMass = 659.0234985351563f;
		/*desc.suspensionForceDesc[0] = { 32833.30078125f, 8528.1201171875f, 553.7739868164063f };
		desc.suspensionForceDesc[1] = { 33657.3984375f, 8742.1904296875f, 567.6749877929688f };
		desc.suspensionForceDesc[2] = { 26049.0f, 6765.97021484375f, 439.3489990234375f };
		desc.suspensionForceDesc[3] = { 26894.099609375f, 6985.47998046875f, 453.6029968261719f };*/

		// VehicleTireForce
		//for (int i = 0; i < 4; ++i)
		//{
		//	desc.tireForceDesc[i].longStiff = 36787.5f; // ������ ���� ����
		//	desc.tireForceDesc[i].latStiffX = 0.3f; // Ⱦ���� ���� ����
		//	desc.tireForceDesc[i].camberStiff = 0.0f;

		//	desc.tireForceDesc[i].frictionVsSlip[0][0] = 0.0f;
		//	desc.tireForceDesc[i].frictionVsSlip[0][1] = 1.5f;
		//	desc.tireForceDesc[i].frictionVsSlip[1][0] = 0.10000000149011612f;
		//	desc.tireForceDesc[i].frictionVsSlip[1][1] = 1.5f;
		//	desc.tireForceDesc[i].frictionVsSlip[2][0] = 1.0f;
		//	desc.tireForceDesc[i].frictionVsSlip[2][1] = 1.5f;

		//	desc.tireForceDesc[i].loadFilter[0][0] = 0.0f;
		//	desc.tireForceDesc[i].loadFilter[0][1] = 0.23080000281333924f;
		//	desc.tireForceDesc[i].loadFilter[1][0] = 3.0f;
		//	desc.tireForceDesc[i].loadFilter[1][1] = 3.0f;
		//}

		//// Front wheels
		//desc.tireForceDesc[0].latStiffY = 118699.637252138f * 2.5f; //���� ���� 150% ����
		//desc.tireForceDesc[0].restLoad = 5628.72314453125f;
		//desc.tireForceDesc[1].latStiffY = 118699.637252138f * 2.5f; //���� ���� 150% ����
		//desc.tireForceDesc[1].restLoad = 5628.72314453125f;

		//// Rear wheels
		//desc.tireForceDesc[2].latStiffY = 143930.84033118f * 2.5f; //���� ���� 150% ����
		//desc.tireForceDesc[2].restLoad = 4604.3134765625f;
		//desc.tireForceDesc[3].latStiffY = 143930.84033118f * 2.5f; //���� ���� 150% ����
		//desc.tireForceDesc[3].restLoad = 4604.3134765625f;

		//// VehicleWheel
		//for (int i = 0; i < 4; ++i)
		//{
		//	desc.wheelDesc[i].radius = 0.29190240383148193f; // �ݰ� ����
		//	desc.wheelDesc[i].halfWidth = 0.1892214059829712f; // �� ����
		//	desc.wheelDesc[i].mass = 25.0f; // ���� ����
		//	desc.wheelDesc[i].moi = 1.5231969833374023f; // ���� ���Ʈ ����
		//	desc.wheelDesc[i].dampingRate = 80.f;		//���� ���� ������		0~ Infinit //  ���� ���� �� ������ ����
		//}

		desc.directDrivetrainParamsDesc.directDriveThrottleResponseParams.maxResponse = 20000.f;
		for (int i = 0; i < wheelCount; ++i)
		{
			desc.directDrivetrainParamsDesc.directDriveThrottleResponseParams.wheelResponseMultipliers[i] = 1.0f;
		}

		PxVehicleCommandNonLinearResponseParams& nonLinearResponse = desc.directDrivetrainParamsDesc.directDriveThrottleResponseParams.nonlinearResponse;
		nonLinearResponse.clear();

		// Throttle 0.00
		{
			PxVehicleCommandValueResponseTable responseTable;
			responseTable.commandValue = 0.00f;
			responseTable.speedResponses.addPair(0.0f, 0.00f);
			responseTable.speedResponses.addPair(20.0f, 0.00f);
			responseTable.speedResponses.addPair(60.0f, 0.0f);
			nonLinearResponse.addResponse(responseTable);
		}

		// Throttle 0.25
		{
			PxVehicleCommandValueResponseTable responseTable;
			responseTable.commandValue = 0.25f;
			responseTable.speedResponses.addPair(0.0f, 0.25f);
			responseTable.speedResponses.addPair(20.0f, 0.25f);
			responseTable.speedResponses.addPair(60.0f, 0.0f);
			nonLinearResponse.addResponse(responseTable);
		}

		// Throttle 0.50
		{
			PxVehicleCommandValueResponseTable responseTable;
			responseTable.commandValue = 0.50f;
			responseTable.speedResponses.addPair(0.0f, 0.50f);
			responseTable.speedResponses.addPair(20.0f, 0.50f);
			responseTable.speedResponses.addPair(60.0f, 0.0f);
			nonLinearResponse.addResponse(responseTable);
		}

		// Throttle 0.75
		{
			PxVehicleCommandValueResponseTable responseTable;
			responseTable.commandValue = 0.75f;
			responseTable.speedResponses.addPair(0.0f, 0.75f);
			responseTable.speedResponses.addPair(20.0f, 0.75f);
			responseTable.speedResponses.addPair(60.0f, 0.0f);
			nonLinearResponse.addResponse(responseTable);
		}

		// Throttle 1.00
		{
			PxVehicleCommandValueResponseTable responseTable;
			responseTable.commandValue = 1.00f;
			responseTable.speedResponses.addPair(0.0f, 1.00f);
			responseTable.speedResponses.addPair(20.0f, 1.00f);
			responseTable.speedResponses.addPair(60.0f, 1.00f);
			responseTable.speedResponses.addPair(80.0f, 1.00f);
			responseTable.speedResponses.addPair(100.0f, 1.00f);
			
			nonLinearResponse.addResponse(responseTable);
			
		}

	}
	else
	{
		isLoad = true;
		pBaseVehicleParams = get<0>(*Load_data);
		pDirectDrivetrainParams = get<1>(*Load_data);

	}
	void* vehicleParams[2] = { &pBaseVehicleParams ,&pDirectDrivetrainParams };


	m_pPhysXActorVehicle = PhysXActorVehicle::Create(*m_pGameInstance->GetPhysics(),
		PxCookingParams(m_pGameInstance->GetPhysics()->getTolerancesScale()),
		*gMaterial,
		gPhysXMaterialFrictions,
		gNbPhysXMaterialFrictions,
		gPhysXDefaultMaterialFriction,
		desc, isLoad ? &vehicleParams :nullptr);

	m_pPhysXActorVehicle->getTransmissionCommandState().gear = PxVehicleDirectDriveTransmissionCommandState::eNEUTRAL;
	m_pPhysXActorVehicle->setUpActor(*m_pGameInstance->GetScene(), inittransform, "VehicleTest");

	m_pPhysXActorVehicle->getActor()->setAngularDamping(10.0f);
	m_pPhysXActorVehicle->getActor()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

	m_pVehicleSimulationContext.setToDefault();
	m_pVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	m_pVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	m_pVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
	m_pVehicleSimulationContext.scale.scale = WORLD_METER;
	
	m_pVehicleSimulationContext.gravity = m_pGameInstance->GetScene()->getGravity();
	m_pVehicleSimulationContext.physxScene = m_pGameInstance->GetScene();
	m_pVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
	
#ifdef _DEBUG
	m_OutDesc.pPhysXActorVehicle = m_pPhysXActorVehicle;

	
#endif
	
	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent_Vehicle::Render()
{
	
		//m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

		//m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
		//m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));


		//m_pShader->Begin(1);

		//for (auto& pPhysXBuffer : m_pBuffer)
		//{
		//	if (nullptr != pPhysXBuffer)
		//	{
		//		pPhysXBuffer->Bind_Buffers();
		//		pPhysXBuffer->Render();
		//	}


		//}

	return S_OK;
}
#endif

void CPhysXComponent_Vehicle::Tick(const _float fTimeDelta)
{
	
	m_pPhysXActorVehicle->getCommandState().brakes[0] = m_Command.brake;
	m_pPhysXActorVehicle->getCommandState().brakes[1] = m_Command.handbrake;
	m_pPhysXActorVehicle->getCommandState().nbBrakes = 2;
	m_pPhysXActorVehicle->getCommandState().throttle = m_Command.throttle;
	m_pPhysXActorVehicle->getCommandState().steer = m_Command.steer;
	m_pPhysXActorVehicle->getTransmissionCommandState().gear = m_Command.gear;
	//m_pPhysXActorVehicle->getDirectDriveParams().directDriveThrottleResponseParams.maxResponse = m_Command.MaxHorsePower;
	m_pPhysXActorVehicle->step(fTimeDelta, m_pVehicleSimulationContext);
}

void CPhysXComponent_Vehicle::Late_Tick(const _float fTimeDelta)
{
	m_Command.transform = 	m_pPhysXActorVehicle->getPhysXState().physxActor.rigidBody->getGlobalPose();
}






HRESULT CPhysXComponent_Vehicle::CreateActor()
{





	return S_OK;
}

void CPhysXComponent_Vehicle::initMaterialFrictionTable()
{
	gMaterial = m_pGameInstance->GetPhysics()->createMaterial(0.8f, 0.5f, 0.6f);

	gPhysXMaterialFrictions[0].friction = 1.0f;
	gPhysXMaterialFrictions[0].material = gMaterial;
	gPhysXDefaultMaterialFriction = 1.2f;
	gNbPhysXMaterialFrictions = 1.5F;
}



CPhysXComponent_Vehicle * CPhysXComponent_Vehicle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPhysXComponent_Vehicle*		pInstance = new CPhysXComponent_Vehicle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_Vehicle");
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent_Vehicle::Clone(void * pArg)
{
	CPhysXComponent_Vehicle*		pInstance = new CPhysXComponent_Vehicle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_Vehicle");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_Vehicle::Free()
{
	__super::Free();
	//Safe_Release(m_pVehicle);
	Safe_Release(m_pPhysXActorVehicle);
	

	

}

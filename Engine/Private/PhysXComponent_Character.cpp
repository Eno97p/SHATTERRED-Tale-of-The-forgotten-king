#include "..\Public\PhysXComponent_Character.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"

#include"Transform.h"
#include"CHitReport.h"

CPhysXComponent_Character::CPhysXComponent_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_Character::CPhysXComponent_Character(const CPhysXComponent_Character& rhs)
	: CPhysXComponent{ rhs }
#ifdef _DEBUG
	, m_OutDesc{rhs.m_OutDesc }
	,m_pHitReport{ CHitReport::GetInstance() }
#endif // _DEBUG


{
}

HRESULT CPhysXComponent_Character::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysXComponent_Character::Initialize(void* pArg)
{
	ControllerDesc* pObjectdesc = static_cast<ControllerDesc*>(pArg);


	m_pCCTFilerCallBack = CCCTFilterCallBack::Create(pObjectdesc->pFilterCallBack);
	m_pTransform = pObjectdesc->pTransform;
	Safe_AddRef(m_pTransform);


	m_fJumpSpeed = pObjectdesc->fJumpSpeed;

	PxCapsuleControllerDesc desc;
	desc.setToDefault();

	desc.height = pObjectdesc->height;
	desc.radius = pObjectdesc->radius;
	desc.position = pObjectdesc->position;

	desc.material = m_pGameInstance->GetPhysics()->createMaterial(pObjectdesc->fMatterial.x, pObjectdesc->fMatterial.y, pObjectdesc->fMatterial.z);
	desc.stepOffset = pObjectdesc->stepOffset;
	desc.upDirection = pObjectdesc->upDirection;
	desc.slopeLimit = pObjectdesc->slopeLimit;
	desc.contactOffset = pObjectdesc->contactOffset;
	desc.nonWalkableMode = pObjectdesc->nonWalkableMode;
	desc.volumeGrowth = 1.5f;
	desc.reportCallback = m_pHitReport;
	desc.userData = pObjectdesc->pGameObject;
	
	


	PxFilterData filterData = pObjectdesc->filterData;

	m_pController = m_pGameInstance->GetControllerManager()->createController(desc);
	
	if (m_pController == nullptr)
	{
		MSG_BOX("Failed to Create : Physx_Controller");
		return E_FAIL;
	}
	if (pObjectdesc->pName)
		m_pController->getActor()->setName(pObjectdesc->pName);

	
	if (m_pController)
	{
		PxRigidDynamic* actor = m_pController->getActor();
		if (actor)
		{
			PxShape* shpae;
			PxU32 numShapes = actor->getNbShapes();
			for (PxU32 i = 0; i < numShapes; ++i)
			{
				if (actor->getShapes(&shpae, numShapes))
				{
					shpae->setSimulationFilterData(filterData);
					shpae->setQueryFilterData(filterData);
					
					
	
				}
			}
			//actor->setMaxDepenetrationVelocity(4.0f);
		}
	}




#ifdef _DEBUG
	if (FAILED(Init_Buffer()))
		return E_FAIL;

	m_OutDesc.pController = m_pController;
#endif
	
	



	return S_OK;
}
void CPhysXComponent_Character::SetFilterData(PxFilterData filterData)
{

	if (m_pController)
	{
		PxRigidDynamic* actor = m_pController->getActor();
		if (actor)
		{
			PxShape* shpae;
			PxU32 numShapes = actor->getNbShapes();
			for (PxU32 i = 0; i < numShapes; ++i)
			{
				if (actor->getShapes(&shpae, 1) == 1)
				{

					shpae->setSimulationFilterData(filterData);
				}

			}
				


		}
			


	}
		



}
#ifdef _DEBUG
HRESULT CPhysXComponent_Character::Init_Buffer()
{


	PxShape** Shapes = new PxShape * [10];

	//여기서 문제인듯? //받아오려는 갯수가 많으면 속도가 느린가?
	//많아도 문제 없음 :확인 완료
	PxU32 numShapes = m_pController->getActor()->getShapes(Shapes, 10);
	for (PxU32 i = 0; i < numShapes; ++i)
	{
		PxShape* shape = Shapes[i];

		const PxGeometryHolder geometry = shape->getGeometry();
		switch (geometry.getType())
		{
		case PxGeometryType::eCAPSULE:
		{
			PxCapsuleGeometry capsuleGeometry = geometry.capsule();

			_float fRadius = capsuleGeometry.radius;
			_float fHalfHeight = capsuleGeometry.halfHeight;

			vector<_float3> vecVertices = CreateCapsuleVertices(fRadius, fHalfHeight);
			m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, vecVertices));
			break;
		}

		}


	}

	delete[] Shapes;
	Shapes = nullptr;



	return S_OK;
}


HRESULT CPhysXComponent_Character::Render()
{


	//??왜??
	if (this->m_OutDesc.bIsOnDebugRender)
	{
		//랜더 문제 (와이어프레임)때문에 따로 함수를 오버라이딩함
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));


		m_pShader->Begin(0);

		for (auto& pPhysXBuffer : m_pBuffer)
		{
			if (nullptr != pPhysXBuffer)
			{
				pPhysXBuffer->Bind_Buffers();
				pPhysXBuffer->Render();
			}


		}
	}
	return S_OK;
}
#endif // _DEBUG

HRESULT CPhysXComponent_Character::Go_Up(_float fTimeDelta)
{
	_vector vTransformUp = m_pTransform->Get_State(CTransform::STATE_UP);
	_float3 fUp;
	XMStoreFloat3(&fUp, vTransformUp);

	PxVec3 moveVector = PxVec3(fUp.x, fUp.y, fUp.z) * m_fJumpSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);



	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_Straight(_float fTimeDelta)
{
	_vector vTransformLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_float3 fLook;
	XMStoreFloat3(&fLook, vTransformLook);

	PxVec3 moveVector = PxVec3(fLook.x, 0, fLook.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
 	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);



	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_BackWard(_float fTimeDelta)
{
	_vector vTransformLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_float3 fLook;
	XMStoreFloat3(&fLook, -vTransformLook);

	PxVec3 moveVector = PxVec3(fLook.x, 0, fLook.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	
		//
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_Left(_float fTimeDelta)
{
	_vector vTransformLeft = -m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 fLeft;
	XMStoreFloat3(&fLeft, vTransformLeft);

	PxVec3 moveVector = PxVec3(fLeft.x, 0, fLeft.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_Right(_float fTimeDelta)
{
	_vector vTransformRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 fRight;
	XMStoreFloat3(&fRight, vTransformRight);

	PxVec3 moveVector = PxVec3(fRight.x, 0, fRight.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_LeftFront(_float fTimeDelta)
{
	_vector vTransformLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_float3 fLook;
	XMStoreFloat3(&fLook, vTransformLook);

	_vector vTransformLeft = -m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 fLeft;
	XMStoreFloat3(&fLeft, vTransformLeft);

	PxVec3 moveVector = PxVec3(fLook.x + fLeft.x, 0, fLook.z + fLeft.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_RightFront(_float fTimeDelta)
{
	_vector vTransformLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_float3 fLook;
	XMStoreFloat3(&fLook, vTransformLook);

	_vector vTransformRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 fRight;
	XMStoreFloat3(&fRight, vTransformRight);

	PxVec3 moveVector = PxVec3(fLook.x + fRight.x, 0, fLook.z + fRight.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_OrbitCW(_float fTimeDelta, CTransform* pTargetTransform)
{
	_vector vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 fRight;
	vRight = XMVector3Normalize(vRight);
	XMStoreFloat3(&fRight, vRight);
	PxVec3 moveVector = PxVec3(-fRight.x, 0.f, -fRight.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);


	_float3 fScale = m_pTransform->Get_Scaled();
	vLook = pTargetTransform->Get_State(CTransform::STATE_POSITION) - vPos;
	vLook.m128_f32[1] = 0.f;

	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	m_pTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pTransform->Set_Scale(fScale.x, fScale.y, fScale.z);

	return S_OK;
}

HRESULT CPhysXComponent_Character::Go_OrbitCCW(_float fTimeDelta, CTransform* pTargetTransform)
{
	_vector vRight = m_pTransform->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 fRight;
	vRight = XMVector3Normalize(vRight);
	XMStoreFloat3(&fRight, vRight);
	PxVec3 moveVector = PxVec3(fRight.x, 0.f, fRight.z) * m_fSpeed * fTimeDelta;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);


	_float3 fScale = m_pTransform->Get_Scaled();
	vLook = pTargetTransform->Get_State(CTransform::STATE_POSITION) - vPos;
	vLook.m128_f32[1] = 0.f;

	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	m_pTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pTransform->Set_Scale(fScale.x, fScale.y, fScale.z);

	return S_OK;
}


HRESULT CPhysXComponent_Character::Go_Jump(_float fTimeDelta, _float fJumpSpeed)
{
	m_fCurrentY_Velocity = m_fJumpSpeed;

	m_bIsJump = true;

	return S_OK;
}

void CPhysXComponent_Character::Set_Position(_vector vPos)
{
	PxExtendedVec3 vPosition = PxExtendedVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
	m_pController->setFootPosition(vPosition);
}

_float CPhysXComponent_Character::Get_LengthFromGround()
{
	PxExtendedVec3 vFootPosition = m_pController->getFootPosition();

	PxVec3 origin(static_cast<_float>(vFootPosition.x), static_cast<_float>(vFootPosition.y), static_cast<_float>(vFootPosition.z));
	PxVec3 direction(0, -1, 0); // 레이의 방향
	float maxDistance = 100.f; // 최대 검사 거리

	PxRaycastBuffer hit;
	bool hasHit = m_pGameInstance->GetScene()->raycast(origin, direction, maxDistance, hit);

	if (hasHit) {
		PxVec3 hitPoint = hit.block.position;

		return vFootPosition.y - hitPoint.y;
	}
	else
	{
		return 100.f;
	}
}


void CPhysXComponent_Character::Tick(_float fTimeDelta)
{
	//m_pGameInstance->GetControllerManager()->computeInteractions(fTimeDelta, m_pFilterCallBack);

	
	if (m_bGravity)
	{
		m_fCurrentY_Velocity += m_fGravity * fTimeDelta;
	}
	// 변위 계산: s = ut + 0.5 * a * t^2
	float displacement = m_fCurrentY_Velocity * fTimeDelta + 0.5f * m_fGravity * fTimeDelta * fTimeDelta;


	PxVec3 moveVector = PxVec3(0, displacement, 0);


	PxControllerFilters filters;
	filters.mCCTFilterCallback = m_pCCTFilerCallBack;
	
	PxControllerCollisionFlags flags = m_pController->move(moveVector, 0.001f, fTimeDelta, filters, nullptr);
	
#ifdef _DEBUG
	m_OutDesc.fPosition = { static_cast<_float>(m_pController->getFootPosition().x), static_cast<_float>(m_pController->getFootPosition().y), static_cast<_float>(m_pController->getFootPosition().z) };
	
#endif // _DEBUG

	
	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_bIsJump = false;
		m_fCurrentY_Velocity = 0.f;
	}


	
}


void CPhysXComponent_Character::Late_Tick(_float fTimeDelta)
{
	PxExtendedVec3 vFootPosition = m_pController->getFootPosition();

	_float3 fPos = { static_cast<_float>(vFootPosition.x),static_cast<_float>(vFootPosition.y), static_cast<_float>(vFootPosition.z) };

	_vector PhysxPosition = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.0f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, PhysxPosition);

	m_WorldMatrix = *m_pTransform->Get_WorldFloat4x4();

#ifdef _DEBUG
	//m_pController->setFootPosition(PxExtendedVec3(m_OutDesc.fPosition.x, m_OutDesc.fPosition.y, m_OutDesc.fPosition.z));
#endif // _DEBUG

}


CPhysXComponent_Character* CPhysXComponent_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysXComponent_Character* pInstance = new CPhysXComponent_Character(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_Character");
		return nullptr;
	}

	return pInstance;
}

CComponent* CPhysXComponent_Character::Clone(void* pArg)
{
	CPhysXComponent_Character* pInstance = new CPhysXComponent_Character(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_Character");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_Character::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pCCTFilerCallBack);
	Safe_physX_Release(m_pController);
	
	
	//m_pController->release();
	
	
}

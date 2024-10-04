#include "RotateGate.h"

CRotateGate::CRotateGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActive_Element(pDevice, pContext)
{
}

CRotateGate::CRotateGate(const CRotateGate& rhs)
	: CActive_Element(rhs)
{
}

HRESULT CRotateGate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRotateGate::Initialize(void* pArg)
{

	CGameObject::GAMEOBJECT_DESC td{};

	td.fSpeedPerSec = 3.f;
	td.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&td)))
		return E_FAIL;




	//if (nullptr != pArg)
	{

		CActive_Element::MAP_ELEMENT_DESC* pDesc = (CActive_Element::MAP_ELEMENT_DESC*)pArg;
		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
		//m_pTransformCom->Scaling(-1.8f, 1.7f, 1.f);`
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-258.8f, 10.692f, -2.148f, 1.f));

		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f, 1.f }, XMConvertToRadians(90.f));
		//m_pTransformCom->Rotation({ 1.f, 0.f, 0.f, 1.f }, XMConvertToRadians(180.f));
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRotateGate::Priority_Tick(_float fTimeDelta)
{

}

void CRotateGate::Tick(_float fTimeDelta)
{
	/*if (m_bIsRotating)
	{
		m_fRotationTime += fTimeDelta;

		if (m_fRotationTime >= m_fTargetRotationTime)
		{
			m_bIsRotating = false;
			m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fTotalRotationAngle - (m_fTotalRotationAngle * (m_fRotationTime - fTimeDelta) / m_fTargetRotationTime));
		}
		else
		{
			_float fRotationAngle = m_fTotalRotationAngle * (fTimeDelta / m_fTargetRotationTime);
			m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fRotationAngle);
		}
	}*/

	//m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 1.f), fTimeDelta);

	//_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector newPos = XMVectorSetY(currentPos, XMVectorGetY(currentPos) + 0.3f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

	m_pPhysXCom->Tick(m_pTransformCom->Get_WorldFloat4x4());

}

void CRotateGate::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif
	m_pPhysXCom->Late_Tick(m_pTransformCom->Get_WorldFloat4x4Ref());

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CRotateGate::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;

		//if ( i != 29)
		
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
}

void CRotateGate::Rotate_Gate(_float fTime)
{
	if (!m_bIsRotating)
	{
		m_bIsRotating = true;
		m_fRotationTime = 0.f;
		m_fTargetRotationTime = fTime;
	}
}

HRESULT CRotateGate::Add_Components(void* pArg)
{
	CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

	/*wstring wstr = pDesc->wstrModelName;*/

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY,/* wstr.c_str()*/TEXT("Prototype_Component_Model_RotateGate"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	CPhysXComponent::PHYSX_DESC		PhysXDesc{};
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.fBoxProperty = _float3(0.1f, 0.1f, 0.1f);				//박스 크기
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&PhysXDesc.fOffsetMatrix, XMMatrixRotationX(XMConvertToRadians(0.0f)) * XMMatrixTranslation(0.f, 0.f, 0.f));  //오프셋 위치


	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.eGeometryType = PxGeometryType::eBOX;
	PhysXDesc.pName = "RotateGate";
	PhysXDesc.filterData.word0 = GROUP_ENVIRONMENT;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	m_pPhysXCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);

	return S_OK;
}

HRESULT CRotateGate::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;

#pragma endregion 모션블러

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CRotateGate* CRotateGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotateGate* pInstance = new CRotateGate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CRotateGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRotateGate::Clone(void* pArg)
{
	CRotateGate* pInstance = new CRotateGate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRotateGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotateGate::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysXCom);
}

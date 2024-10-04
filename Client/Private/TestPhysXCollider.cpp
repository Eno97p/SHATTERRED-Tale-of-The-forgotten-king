#include "stdafx.h"
#include "TestPhysXCollider.h"


#include"GameInstance.h"



CTestPhysXCollider::CTestPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestPhysXCollider::CTestPhysXCollider(const CTestPhysXCollider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestPhysXCollider::Initialize_Prototype()
{
	


	return S_OK;
}

HRESULT CTestPhysXCollider::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (FAILED(Add_PxActor()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.0f, 400.0f, 0.0f, 1.0f));

	return S_OK;
}

void CTestPhysXCollider::Priority_Tick(_float fTimeDelta)
{
}

void CTestPhysXCollider::Tick(_float fTimeDelta)
{
	
	m_pPhysXCom->Tick(m_pTransformCom->Get_WorldFloat4x4());


}

void CTestPhysXCollider::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_pPhysXCom->Late_Tick(m_pTransformCom->Get_WorldFloat4x4Ref());



}

HRESULT CTestPhysXCollider::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CTestPhysXCollider::Add_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hoverboard"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;




	CPhysXComponent::PHYSX_DESC PhysXDesc;
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.fBoxProperty = _float3(10000.f, 3.0f, 10000.f);				//박스 크기
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&PhysXDesc.fOffsetMatrix, XMMatrixRotationX(XMConvertToRadians(0.0f)) * XMMatrixTranslation(0.f, 0.f, 0.f));  //오프셋 위치
	PhysXDesc.eGeometryType = PxGeometryType::eBOX;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom),&PhysXDesc)))
		return E_FAIL;

	m_pPhysXCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	return S_OK;
}

HRESULT CTestPhysXCollider::Bind_ShaderResources()
{


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	return S_OK;
}

HRESULT CTestPhysXCollider::Add_PxActor()
{
	

	
	return S_OK;
}

CTestPhysXCollider* CTestPhysXCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestPhysXCollider* pInstance = new CTestPhysXCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTestPhysXCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTestPhysXCollider::Clone(void* pArg)
{
	CTestPhysXCollider* pInstance = new CTestPhysXCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTestPhysXCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestPhysXCollider::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCom);
}

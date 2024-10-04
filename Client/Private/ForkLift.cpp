#include "stdafx.h"
#include "..\Public\ForkLift.h"

#include "GameInstance.h"
#include "ComputeShader_Buffer.h"

CForkLift::CForkLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CForkLift::CForkLift(const CForkLift& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CForkLift::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CForkLift::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 3.f, 5.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);

	return S_OK;
}

void CForkLift::Priority_Tick(_float fTimeDelta)
{
	//s
}

void CForkLift::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	float* fResult = (float*)m_pCSBufferCom->Compute(1, 1, 1);
	float ff1 = fResult[0];
	float ff2 = fResult[1];
	float ff3 = fResult[2];
	float ff4 = fResult[3];

	_float4* fResult2 = (_float4*)m_pCSBufferCom2->Compute(1, 1, 1);
}

void CForkLift::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CForkLift::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

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

HRESULT CForkLift::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CForkLift::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PretorianSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

#pragma region ComputeShader_Buffer 예시(float)
	_float f1[4] = { 1.f, 2.f, 3.f, 4.f }; // 매개변수1
	_float f2[4] = { 5.f, 6.f, 7.f, 8.f }; // 매개변수2
	_float* f[2] = { f1, f2 };
	size_t size = sizeof(f1[0]);
	size_t num = sizeof(f1) / sizeof(f1[0]);

	CComputeShader_Buffer::CSBUFFER_DESC csbufferDesc;
	csbufferDesc.numInputVariables = 2; // 매개변수 갯수
	csbufferDesc.arg = (void**)f; // 매개변수 정보
	csbufferDesc.variableSize = size; // 자료형 크기
	csbufferDesc.variableNum = num; // 변수 당 배열 index수
	csbufferDesc.variableFormat = DXGI_FORMAT_R32_FLOAT; // 자료형 타입

	/* For.Com_CSBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Calculate"),
		TEXT("Com_CSBuffer"), reinterpret_cast<CComponent**>(&m_pCSBufferCom), &csbufferDesc)))
		return E_FAIL;
#pragma endregion ComputeShader_Buffer 예시(float)


#pragma region ComputeShader_Buffer 예시(float4)
	_float4 ff1[1] = { _float4(1.f, 2.f, 3.f, 4.f) }; // 매개변수1
	_float4 ff2[1] = { _float4(5.f, 6.f, 7.f, 8.f) }; // 매개변수2
	_float4* ff[2] = { ff1, ff2 };
	size_t size2 = sizeof(ff1[0]);
	size_t num2 = sizeof(ff1) / sizeof(ff1[0]);

	CComputeShader_Buffer::CSBUFFER_DESC csbufferDesc2;
	csbufferDesc2.numInputVariables = 2; // 매개변수 갯수
	csbufferDesc2.arg = (void**)ff; // 매개변수 정보
	csbufferDesc2.variableSize = size2; // 자료형 크기
	csbufferDesc2.variableNum = num2; // 변수 당 배열 index수
	csbufferDesc2.variableFormat = DXGI_FORMAT_R32G32B32A32_FLOAT; // 자료형 타입

	/* For.Com_CSBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Float4"),
		TEXT("Com_CSBuffer2"), reinterpret_cast<CComponent**>(&m_pCSBufferCom2), &csbufferDesc2)))
		return E_FAIL;
#pragma endregion ComputeShader_Buffer 예시(float4)

	return S_OK;
}

HRESULT CForkLift::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CForkLift* CForkLift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CForkLift* pInstance = new CForkLift(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CForkLift");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CForkLift::Clone(void* pArg)
{
	CForkLift* pInstance = new CForkLift(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CForkLift");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CForkLift::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pCSBufferCom);
	Safe_Release(m_pCSBufferCom2);
}

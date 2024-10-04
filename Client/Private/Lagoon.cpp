#include "Lagoon.h"
#include "GameInstance.h"

CLagoon::CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CLagoon::CLagoon(const CLagoon& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CLagoon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLagoon::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CMap_Element::MAP_ELEMENT_DESC * pDesc = (CMap_Element::MAP_ELEMENT_DESC*)(pArg);
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));

		//TRIGGER STATE SET
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLagoon::Priority_Tick(_float fTimeDelta)
{

}

void CLagoon::Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if (m_fAccTime > 100000.f)
	{
		m_fAccTime = 0;
	}
}

void CLagoon::Late_Tick(_float fTimeDelta)
{
	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_MIRROR, this);

	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CLagoon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_CAUSTIC]->Bind_ShaderResource(m_pShaderCom, "g_CausticTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_NORMAL1]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture1", 0)))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_NORMAL2]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture2", 0)))
			return E_FAIL;	
		
		if (FAILED(m_pTexture[TEX_FOAMMASK]->Bind_ShaderResource(m_pShaderCom, "g_FoamMaskTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_FOAM]->Bind_ShaderResources(m_pShaderCom, "g_FoamTexture")))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_FOAMNORMAL]->Bind_ShaderResources(m_pShaderCom, "g_FoamTextureNormal")))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamWaveFrequency", &m_fFoamWaveFrequency, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamWaveAmplitude", &m_fFoamWaveAmplitude, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamMaskScale", &m_fFoamMaskScale, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamMaskSpeed", &m_fFoamMaskSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamBlendStrength", &m_fFoamBlendStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFoamFresnelStrength", &m_fFoamFresnelStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength0", &m_fNormalStrength0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength1", &m_fNormalStrength1, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength2", &m_fNormalStrength2, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterAlpha", &m_fWaterAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterDepth", &m_fWaterDepth, sizeof(_float))))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPosition", &m_vLightPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &m_vLightDir, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &m_vLightDiffuse, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &m_vLightAmbient, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &m_vLightSpecular, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlSpecular", &m_vMtrlSpecular, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &m_fLightRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowSpeed", &m_fFlowSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCausticStrength", &m_fCausticStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFresnelStrength", &m_fFresnelStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyColor", &m_vSkyColor, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
			return E_FAIL;

		m_pShaderCom->Begin(7);  // Lagoon_7 패스 사용
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLagoon::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_CAUSTIC]->Bind_ShaderResource(m_pShaderCom, "g_CausticTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_NORMAL1]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture1", 0)))
			return E_FAIL;

		if (FAILED(m_pTexture[TEX_NORMAL2]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture2", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength0", &m_fNormalStrength0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength1", &m_fNormalStrength1, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength2", &m_fNormalStrength2, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterAlpha", &m_fWaterAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterDepth", &m_fWaterDepth, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomIntensity", &m_fBloomIntensity, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPosition", &m_vLightPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &m_vLightDir, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &m_vLightDiffuse, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &m_vLightAmbient, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &m_vLightSpecular, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlSpecular", &m_vMtrlSpecular, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &m_fLightRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowSpeed", &m_fFlowSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCausticStrength", &m_fCausticStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFresnelStrength", &m_fFresnelStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyColor", &m_vSkyColor, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
			return E_FAIL;

		m_pShaderCom->Begin(5);  // Lagoon_7 패스 사용
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLagoon::Render_Mirror()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Unbind_SRVs();

		// 기본 텍스처 바인딩
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		// 노말 맵 바인딩 (프레넬 효과 계산에 사용)
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		// 시간 값 바인딩 (노말 맵 애니메이션에 사용될 수 있음)
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;

		// 프레넬 효과 강도
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFresnelStrength", &m_fFresnelStrength, sizeof(_float))))
			return E_FAIL;

		// 알파 값
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterAlpha", &m_fWaterAlpha, sizeof(_float))))
			return E_FAIL;

		// 카메라 위치 (뷰 방향 계산에 필요)
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
			return E_FAIL;

		// 노말 맵 블렌딩을 위한 강도 값들
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength0", &m_fNormalStrength0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength1", &m_fNormalStrength1, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalStrength2", &m_fNormalStrength2, sizeof(_float))))
			return E_FAIL;

		// 추가 노말 맵 바인딩
		if (FAILED(m_pTexture[TEX_NORMAL1]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture1", 0)))
			return E_FAIL;
		if (FAILED(m_pTexture[TEX_NORMAL2]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture2", 0)))
			return E_FAIL;

		// 노말 맵 애니메이션을 위한 flow speed
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowSpeed", &m_fFlowSpeed, sizeof(_float))))
			return E_FAIL;

		m_pShaderCom->Begin(8);  // Mirror 패스 사용

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CLagoon::Add_Components(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lagoon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxLagoon"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Normal1 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WaterNormal1"),
		TEXT("Com_Normal1"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_NORMAL1]))))
		return E_FAIL;

	/* For.Com_Normal2 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WaterNormal2"),
		TEXT("Com_Normal2"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_NORMAL2]))))
		return E_FAIL;

	/* For.Com_Normal3 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WaterCaustic"),
		TEXT("Com_Caustic"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_CAUSTIC]))))
		return E_FAIL;

	/* For.Com_Normal3 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FoamMask"),
		TEXT("Com_FoamMask"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_FOAMMASK]))))
		return E_FAIL;


	/* For.Com_Normal3 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Foam"),
		TEXT("Com_Foam"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_FOAM]))))
		return E_FAIL;

	/* For.Com_Normal3 */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FoamNormal"),
		TEXT("Com_FoamNormal"), reinterpret_cast<CComponent**>(&m_pTexture[TEX_FOAMNORMAL]))))
		return E_FAIL;

	PxPhysics* pPhysics = m_pGameInstance->GetPhysics();
	PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	m_pgroundPlane = PxCreatePlane(*pPhysics, PxPlane(0, 1, 0, -300), *pMaterial);
	m_pGameInstance->GetScene()->addActor(*m_pgroundPlane);

	return S_OK;
}

HRESULT CLagoon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CLagoon* CLagoon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLagoon* pInstance = new CLagoon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLagoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLagoon::Clone(void* pArg)
{
	CLagoon* pInstance = new CLagoon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLagoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLagoon::Free()
{
	__super::Free();
	for (auto& texture : m_pTexture)
		Safe_Release(texture);

	Safe_physX_Release(m_pgroundPlane);
	
	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
}

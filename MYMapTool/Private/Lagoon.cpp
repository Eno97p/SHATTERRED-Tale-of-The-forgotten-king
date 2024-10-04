#include "Lagoon.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CLagoon::CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CLagoon::CLagoon(const CLagoon& rhs)
	: CToolObj(rhs)
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

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Lagoon");
		strcpy_s(m_szLayer, "Layer_Active_Element");
		strcpy_s(m_szModelName, "Prototype_Component_Model_Lagoon");
		m_eModelType = CModel::TYPE_NONANIM;
		m_iTriggerType = pDesc->TriggerType;

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

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
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
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

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(8);

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLagoon::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
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


	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_AABB;

	// 월드 매트릭스에서 스케일 추출
	_float3 vScale;
	vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	// 기본 크기에 스케일 적용
	ColliderDesc.vExtents = _float3(0.5f * vScale.x, 1.f * vScale.y, 0.5f * vScale.z);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

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
	Safe_Release(m_pColliderCom);
	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
}

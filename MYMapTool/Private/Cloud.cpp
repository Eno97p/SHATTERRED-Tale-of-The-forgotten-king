#include "Cloud.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CCloud::CCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CCloud::CCloud(const CCloud& rhs)
	: CToolObj(rhs)
{
}

HRESULT CCloud::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCloud::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Cloud");
		strcpy_s(m_szLayer, "Layer_Trigger");
		strcpy_s(m_szModelName, "Prototype_Component_Model_BasicCube");
		m_eModelType = CModel::TYPE_NONANIM;
		m_iTriggerType = pDesc->TriggerType;

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));

		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	_matrix mat = {
		0.02, 0, 0.f, 0.f,
		0, -0.02, 0.f, 0.f,
		0.f, 0.f, 0.02, 0.f,
		-71.919f, 426.108f, -49.122f, 1.f
	};
	m_pTransformCom->Set_WorldMatrix(mat);

	return S_OK;
}

void CCloud::Priority_Tick(_float fTimeDelta)
{

}

void CCloud::Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if(m_fAccTime > 10000.f)
	{
		m_fAccTime = 0.f;
	}
}

void CCloud::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);
}

HRESULT CCloud::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudDensity", &m_fCloudDensity, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudScale", &m_fCloudScale, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudSpeed", &m_fCloudSpeed, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudHeight", &m_fCloudHeight, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudColor", &m_vCloudColor, sizeof(_float3))))
			return E_FAIL;
		/*if (FAILED(m_pShaderCom->Bind_RawValue("g_SphereTracingThreshold", &m_fSphereTracingThreshold, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxRayDistance", &m_fMaxRayDistance, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxSteps", &m_iMaxSteps, sizeof(int))))*/
		//	return E_FAIL;

		 // 새로운 최적화 관련 값들 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoarseStepSize", &m_fCoarseStepSize, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFineStepSize", &m_fFineStepSize, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaxCoarseSteps", &m_iMaxCoarseSteps, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaxFineSteps", &m_iMaxFineSteps, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDensityThreshold", &m_fDensityThreshold, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaThreshold", &m_fAlphaThreshold, sizeof(float))))
			return E_FAIL;

		// 새로운 노이즈 관련 값들 바인딩
		   // 펄린 노이즈 옥타브 및 주파수 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iPerlinOctaves", &m_iPerlinOctaves, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinFrequency", &m_fPerlinFrequency, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinPersistence", &m_fPerlinPersistence, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinLacunarity", &m_fPerlinLacunarity, sizeof(float))))
			return E_FAIL;

		// Worley 노이즈 주파수 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWorleyFrequency", &m_fWorleyFrequency, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWorleyJitter", &m_fWorleyJitter, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinWorleyMix", &m_fPerlinWorleyMix, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseRemapLower", &m_fNoiseRemapLower, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseRemapUpper", &m_fNoiseRemapUpper, sizeof(float))))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_vector))))
			return E_FAIL;

		//_float4 lightPos;
		//XMStoreFloat4(&lightPos, dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPosition", &m_vLightPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &m_fLightRange, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &m_vLightDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &m_vLightAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &m_vLightSpecular, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &m_vLightDir, sizeof(_float4))))
			return E_FAIL;	
		

		m_pShaderCom->Bind_RawValue("CLOUD_VOLUME_SIZE", &m_fCloudVolumeSize, sizeof(float));
		m_pShaderCom->Bind_RawValue("CLOUD_VOLUME_OFFSET", &m_vCloudVolumeOffset, sizeof(XMFLOAT3));


		m_pShaderCom->Begin(m_iShaderPath);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCloud::Render_Reflection()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_float4x4 ViewMatrix;
	const _float4x4* matCam = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	// 원래 뷰 행렬 로드
	XMMATRIX mOriginalView = XMLoadFloat4x4(matCam);

	// 카메라 위치 추출
	XMVECTOR vCamPos = XMVector3Transform(XMVectorZero(), mOriginalView);

	// 바닥 평면의 높이 (물 표면 높이)
	float floorHeight = 300.f; // 적절한 값으로 조정 필요

	// 반사된 카메라 위치 계산 (Y 좌표만 반전)
	XMVECTOR vReflectedCamPos = vCamPos;
	vReflectedCamPos = XMVectorSetY(vReflectedCamPos, 2 * floorHeight - XMVectorGetY(vCamPos));

	// 카메라 방향 벡터 추출
	XMVECTOR vCamLook = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 0, 1, 0), mOriginalView) - vCamPos);
	XMVECTOR vCamUp = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 1, 0, 0), mOriginalView) - vCamPos);

	// 반사된 카메라 방향 벡터 계산
	XMVECTOR vReflectedCamLook = XMVectorSetY(vCamLook, -XMVectorGetY(vCamLook));
	XMVECTOR vReflectedCamUp = XMVectorSetY(vCamUp, -XMVectorGetY(vCamUp));

	// 반사된 뷰 행렬 생성
	XMMATRIX mReflectedView = XMMatrixLookToLH(vReflectedCamPos, vReflectedCamLook, vReflectedCamUp);

	// 변환된 행렬 저장
	XMStoreFloat4x4(&ViewMatrix, mReflectedView);

	// 반사된 뷰 행렬을 셰이더에 바인딩
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	// 물 표면 높이를 셰이더에 전달
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fReflectionPlaneHeight", &floorHeight, sizeof(_float))))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudDensity", &m_fCloudDensity, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudScale", &m_fCloudScale, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudSpeed", &m_fCloudSpeed, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudHeight", &m_fCloudHeight, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_CloudColor", &m_vCloudColor, sizeof(_float3))))
			return E_FAIL;
		/*if (FAILED(m_pShaderCom->Bind_RawValue("g_SphereTracingThreshold", &m_fSphereTracingThreshold, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxRayDistance", &m_fMaxRayDistance, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxSteps", &m_iMaxSteps, sizeof(int))))*/
		//	return E_FAIL;

		 // 새로운 최적화 관련 값들 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoarseStepSize", &m_fCoarseStepSize, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFineStepSize", &m_fFineStepSize, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaxCoarseSteps", &m_iMaxCoarseSteps, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaxFineSteps", &m_iMaxFineSteps, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDensityThreshold", &m_fDensityThreshold, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaThreshold", &m_fAlphaThreshold, sizeof(float))))
			return E_FAIL;

		// 새로운 노이즈 관련 값들 바인딩
		   // 펄린 노이즈 옥타브 및 주파수 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iPerlinOctaves", &m_iPerlinOctaves, sizeof(int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinFrequency", &m_fPerlinFrequency, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinPersistence", &m_fPerlinPersistence, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinLacunarity", &m_fPerlinLacunarity, sizeof(float))))
			return E_FAIL;

		// Worley 노이즈 주파수 바인딩
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWorleyFrequency", &m_fWorleyFrequency, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fWorleyJitter", &m_fWorleyJitter, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fPerlinWorleyMix", &m_fPerlinWorleyMix, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseRemapLower", &m_fNoiseRemapLower, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseRemapUpper", &m_fNoiseRemapUpper, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_vector))))
			return E_FAIL;

		//_float4 lightPos;
		//XMStoreFloat4(&lightPos, dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPosition", &m_vLightPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &m_fLightRange, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &m_vLightDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &m_vLightAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &m_vLightSpecular, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &m_vLightDir, sizeof(_float4))))
			return E_FAIL;

		//리플렉션 최적화
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fReflectionQuality", &m_fReflectionQuality, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fReflectionOpacity", &m_fReflectionOpacity, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fReflectionDensityScale", &m_fReflectionDensityScale, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBaseSkyColor", &m_vBaseSkyColor, sizeof(_float4))))
			return E_FAIL;

		m_pShaderCom->Bind_RawValue("CLOUD_VOLUME_SIZE", &m_fCloudVolumeSize, sizeof(float));
		m_pShaderCom->Bind_RawValue("CLOUD_VOLUME_OFFSET", &m_vCloudVolumeOffset, sizeof(XMFLOAT3));


		m_pShaderCom->Begin(3);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCloud::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, /*wstr.c_str()*/TEXT("Prototype_Component_Model_CloudDome"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCloud"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	
	
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture3D_Noise"),
		TEXT("Com_Noise3D"), reinterpret_cast<CComponent**>(&m_pNoise3DCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCloud::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	// CCloud::Render() 함수 내에서
	XMMATRIX viewProjMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW)) *
		XMLoadFloat4x4(m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));
	XMMATRIX invViewProjMatrix = XMMatrixInverse(nullptr, viewProjMatrix);
	XMFLOAT4X4 invViewProjFloat4x4;
	XMStoreFloat4x4(&invViewProjFloat4x4, invViewProjMatrix);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_InvViewProjMatrix", &invViewProjFloat4x4)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNoise3DCom->Bind_ShaderResource(m_pShaderCom, "g_3DNoiseTexture")))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CCloud* CCloud::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCloud* pInstance = new CCloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCloud::Clone(void* pArg)
{
	CCloud* pInstance = new CCloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCloud::Free()
{
	__super::Free();

	/*Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);*/
}

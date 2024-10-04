#include "Grass.h"
#include "ToolObj.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CGrass::CGrass(const CGrass& rhs)
	: CToolObj(rhs)
{
}

HRESULT CGrass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;



	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;

		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));
		 
		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_vPivotPos.y += 10.f;
		XMStoreFloat3(&m_vPivotPos, vPos);


		strcpy_s(m_szName, pDesc->szObjName);
		strcpy_s(m_szLayer, pDesc->szLayer);
		m_eModelType = CModel::TYPE_END;
		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);

	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//
	//CVIBuffer_Terrain* pTerrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	m_pVIBufferCom->Initial_InsideCircle(CImgui_Manager::GetInstance()->Get_GlobalColorPicker().z * 255.f, m_vPivotPos);
	//m_pVIBufferCom->Setup_Onterrain(pTerrain);
	//m_pVIBufferCom->Initial_RandomOffset(pTerrain);


	GRASS_DESC* gd = static_cast<GRASS_DESC*>(pArg);
	m_vTopCol = gd->vTopCol;
	m_vBotCol = gd->vBotCol;

	//랜덤으로 탑색 변경 살짞 어둡게
	_float randFloat = RandomFloat(0.f, 0.2f);
	m_vTopColorOffset = { randFloat , randFloat , randFloat };
	m_vTopCol.x -= randFloat;
	m_vTopCol.y -= randFloat;
	m_vTopCol.z -= randFloat;

	return S_OK;
}

void CGrass::Priority_Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if (m_fAccTime > 100000.f)
	{
		m_fAccTime = 0.f;
	}

	// 바람 세기 변화 로직
	m_fWindChangeTime += fTimeDelta;

	// 새로운 목표 풍속 설정
	if (m_fWindChangeTime >= m_fWindChangeDuration)
	{
		m_fTargetWindStrength = (rand() % 100) / 100.f * 5.f + 10.f; // 범위 축소
		m_fWindChangeTime = 0.f;
	}

	// 현재 풍속을 목표 풍속으로 부드럽게 보간
	float t = m_fWindChangeTime / m_fWindChangeDuration;
	float smoothT = t * t * (3 - 2 * t); // 부드러운 보간을 위한 에르미트 보간
	m_fWindStrength = Lerp(m_fWindStrength, m_fTargetWindStrength, smoothT);

	// 주기적인 변동 제거

	// 풍속 범위 제한
	m_fWindStrength = Clamp(m_fWindStrength, 10.f, 15.f);
}

void CGrass::Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Drop(fTimeDelta);
	_float3 campos;
	XMStoreFloat3(&campos, m_pGameInstance->Get_CamPosition());
	//m_pVIBufferCom->Culling_Instance(campos, 500.f);
}

void CGrass::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CGrass::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

_uint CGrass::Get_NumInstance()
{
	return m_pVIBufferCom->Get_Instance_Desc()->iNumInstance;
}

vector<_float4x4*> CGrass::Get_VtxMatrices()
{
	return m_pVIBufferCom->Get_VtxMatrices();
}

HRESULT CGrass::Add_Components(void* pArg)
{
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};

	/* For.Prototype_Component_VIBuffer_Instance_Point*/
	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.iNumInstance = CImgui_Manager::GetInstance()->Get_GrassCount();
	InstanceDesc.vOffsetPos = _float3(0.0f, 0.f, 0.0f);
	InstanceDesc.vPivotPos = m_vPivotPos;
	InstanceDesc.vRange = _float3(CImgui_Manager::GetInstance()->Get_GlobalColorPicker().z * 255.f, 0.f, CImgui_Manager::GetInstance()->Get_GlobalColorPicker().z * 255.f);
	InstanceDesc.vSize = _float2(1.f, 5.f);
	InstanceDesc.vSpeed = _float2(1.f, 7.f);
	InstanceDesc.vLifeTime = _float2(10.f, 15.f);
	InstanceDesc.isLoop = true;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Grass"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (rand() % 100 > 50)
	{
		strcpy_s(m_szModelName, "Prototype_Component_Texture_Grass_TT");

		/* For.Com_Texture */
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_TT"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		/* For.Com_Texture */
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_TT_Normal"),
			TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pNormalCom))))
			return E_FAIL;
	}
	else
	{
		strcpy_s(m_szModelName, "Prototype_Component_Texture_Grass_TF");

		/* For.Com_Texture */
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_TF"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		/* For.Com_Texture */
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_TF_Normal"),
			TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pNormalCom))))
			return E_FAIL;
	}
	

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pNormalCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTopColor", &m_vTopCol, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBotColor", &m_vBotCol, sizeof(_float3))))
		return E_FAIL;

	_float bill = CImgui_Manager::GetInstance()->Get_BillboardFactor();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBillboardFactor", &bill, sizeof(_float))))
		return E_FAIL;

	_float elasticity = CImgui_Manager::GetInstance()->Get_ElasticityFactor();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fElasticityFactor", &elasticity, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;
	_float wf = CImgui_Manager::GetInstance()->Get_GlobalWindStrenth();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlobalWindFactor", &wf, sizeof(_float))))
		return E_FAIL;	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWindStrength", &m_fWindStrength, sizeof(_float))))
		return E_FAIL;

	_float3 WindDir = CImgui_Manager::GetInstance()->Get_GlobalWindDir();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vWindDirection", &WindDir, sizeof(_float3))))
		return E_FAIL;

	// 새로 추가된 변수들 ImGui 매니저에서 가져와서 바인딩
	_float planeOffset = CImgui_Manager::GetInstance()->Get_PlaneOffset();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPlaneOffset", &planeOffset, sizeof(_float))))
		return E_FAIL;	
	
	// 새로 추가된 변수들 ImGui 매니저에서 가져와서 바인딩
	_float planeVertOffset = CImgui_Manager::GetInstance()->Get_PlaneVertOffset();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPlaneVertOffset", &planeVertOffset, sizeof(_float))))
		return E_FAIL;

	_float lodDistance1 = CImgui_Manager::GetInstance()->Get_LODDistance1();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fLODDistance1", &lodDistance1, sizeof(_float))))
		return E_FAIL;

	_float lodDistance2 = CImgui_Manager::GetInstance()->Get_LODDistance2();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fLODDistance2", &lodDistance2, sizeof(_float))))
		return E_FAIL;

	_float grassAmplitude = CImgui_Manager::GetInstance()->Get_GrassAmplitude();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGrassAmplitude", &grassAmplitude, sizeof(_float))))
		return E_FAIL;

	_float grassFrequency = CImgui_Manager::GetInstance()->Get_GrassFrequency();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGrassFrequency", &grassFrequency, sizeof(_float))))
		return E_FAIL;

	_uint lodPlaneCount1 = CImgui_Manager::GetInstance()->Get_LODPlaneCount1();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iLODPlaneCount1", &lodPlaneCount1, sizeof(_uint))))
		return E_FAIL;

	_uint lodPlaneCount2 = CImgui_Manager::GetInstance()->Get_LODPlaneCount2();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iLODPlaneCount2", &lodPlaneCount2, sizeof(_uint))))
		return E_FAIL;

	_uint lodPlaneCount3 = CImgui_Manager::GetInstance()->Get_LODPlaneCount3();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iLODPlaneCount3", &lodPlaneCount3, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrass* pInstance = new CGrass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pInstance = new CGrass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrass::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseCom);
	Safe_Release(m_pNormalCom);
	Safe_Release(m_pShaderCom);
}

#include "Terrain.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "VIBuffer_Terrain.h"
//#include "Calculator.h"
#include "ToolObj_Manager.h"

#include "Monster.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
	, m_pImgui_Manager{ CImgui_Manager::GetInstance() }
	, m_pToolObj_Manager{CToolObj_Manager::GetInstance()}
{

	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pToolObj_Manager);
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject{rhs}
	, m_pImgui_Manager{rhs.m_pImgui_Manager}
	, m_pToolObj_Manager{rhs.m_pToolObj_Manager }
{
	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pToolObj_Manager);
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iVerticesX = 512;
	m_iVerticesZ = 512;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 네비 불러온 거 담기?
	//CNaviMgr::GetInstance()->Load_NaviData(m_pNavigationCom);

	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain::Tick(_float fTimeDelta)
{
	if (nullptr != m_pTransformCom && nullptr != m_pImgui_Manager)
	{
		//if (m_pGameInstance->Is_Mouse_Clicked(DIM_RB))
		//{
		//	//_vector vPickingPos = Check_Picking();
		//	_bool	isPick = { false };
		//	_vector vPickingPos = m_pGameInstance->Picking(&isPick);

		//	if (vPickingPos.m128_f32[3] != -1) // Terrain 위에 제대로 피킹 되었다면
		//	{
		//		if (m_pImgui_Manager->Get_IsNaviMode()) // Navi Mode가 활성화 된 경우
		//			CNaviMgr::GetInstance()->Input_NaviData(m_pNavigationCom, vPickingPos); // Navi 생성
		//		else
		//			Add_ToolObj(vPickingPos); // Obj 생성
		//	}
		//}
	}

	/*if (m_pImgui_Manager->Get_IsNaviClear())
	{
		m_pNavigationCom->Clear_Navigation();
		CNaviMgr::GetInstance()->Clear_Cell();
		m_pImgui_Manager->Set_IsNaviClear();
	}*/
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0); // pass 선택
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

_vector CTerrain::Check_Picking()
{
	/*return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pVIBufferCom, m_pTransformCom,
		m_pVIBufferCom->Get_NumVerticesX(), m_pVIBufferCom->Get_NumVerticesZ());*/
	return XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

HRESULT CTerrain::Add_ToolObj(_vector vPosition)
{
	/*CMonster::MONSTER_DESC pDesc = {};

	pDesc.vPosition = vPosition;*/

	// Calculator의 Picking_OnTerrain( ) 호출 후 반환 값이 -1, -1, -1이 아니면 해당 위치에 모델 생성
	/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &pDesc)))
		return E_FAIL;*/

	//int iObjIdx = m_pImgui_Manager->Get_ObjIdx(); // imgui로부터 Obj Idx를 받아옴
	//int iLayerIdx = m_pImgui_Manager->Get_LayerIdx(); 

	int iObjIdx = 0; // imgui로부터 Obj Idx를 받아옴
	int iLayerIdx = 0;

	if (FAILED(m_pToolObj_Manager->Add_CloneObj(iLayerIdx, iObjIdx, vPosition)))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Setting_LoadTerrain(void* pArg)
{
	m_isReLoad = true;

	TERRAIN_DESC* pDesc = (TERRAIN_DESC*)pArg;

	m_iVerticesX = pDesc->iVerticesX;
	m_iVerticesZ = pDesc->iVerticesZ;
}

HRESULT CTerrain::Add_Components()
{
	//CVIBuffer_Terrain::TERRAIN_VIBUFFER_DESC pDesc{};

	//// 요거 값 imgui로부터 받아와서 넣어주기. 기본 크기 잡아두고
	//pDesc.iVerticesX = 512;
	//pDesc.iVerticesZ = 512;

	// Com VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)/*, &pDesc*/)))
		return E_FAIL;

	// Com Shader
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com Diffuse
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_DIFFUSE]))))
		return E_FAIL;

	// Com Mask
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_MASK]))))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_BRUSH]))))
		return E_FAIL;

	//// Com Calculator
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Calculator"),
	//	TEXT("Com_Calculator"), reinterpret_cast<CComponent**>(&m_pCalculatorCom))))
	//	return E_FAIL;

	//// Com Navigation
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//// 조명 정보 Shader에 던져주기
	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//// 지형 Shader에 카메라의 위치 던져주기(Specular를 위해)
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom[TEX_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	//if (FAILED(m_pTextureCom[TEX_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
	//	return E_FAIL;
	//if (FAILED(m_pTextureCom[TEX_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
	//	return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	
	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pCalculatorCom);

	// calculator는?
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pToolObj_Manager);
}

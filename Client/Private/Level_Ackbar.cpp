#include "stdafx.h"
#include "..\Public\Level_Ackbar.h"
#include "GameInstance.h"
#include "UI_Manager.h"

//Cameras
#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "SideViewCamera.h"

#include "Map_Element.h"
#include "Monster.h"

#include "Light.h"
#include "UI_FadeInOut.h"

#include "Level_Loading.h"
#include "FireEffect.h"

#include"NPC_Rlya.h"
#include"Item.h"

#include "Tree.h"

#include"BlastWall.h"
#include"CInitLoader.h"


#include"EventTrigger.h"


#include "SavePoint.h"
#include "Decal.h"
CLevel_Ackbar::CLevel_Ackbar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())

{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_Ackbar::Initialize()
{

	CRenderer::FOG_DESC fogDesc{};
	fogDesc.vFogColor = { 0.134f, 0.177f, 0.216f, 1.f };
	fogDesc.vFogColor2 = { 0.740f, 0.740f, 0.740f, 1.f };
	fogDesc.fFogRange = 150.5f;
	fogDesc.fFogHeightFalloff = 0.5f;
	fogDesc.fFogGlobalDensity = 0.928f;
	fogDesc.fFogTimeOffset = 4.326f;
	fogDesc.fFogTimeOffset2 = 2.596f;
	fogDesc.fNoiseIntensity = 1.154f;
	fogDesc.fNoiseIntensity2 = 1.923f;
	fogDesc.fNoiseSize = 0.037019f;
	fogDesc.fNoiseSize2 = 0.003365f;
	fogDesc.fFogBlendFactor = 0.269f;

	m_pGameInstance->Set_FogOption(fogDesc);
	//m_pGameInstance->Set_FogOption({ 0.235f, 0.260f, 0.329f, 1.f }, 230.f, 0.87f, 1.f, 10.f, 0.2f, 0.1f);

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trigger()))
		return E_FAIL;

	if (FAILED(Ready_Item()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	Load_LevelData(TEXT("../Bin/MapData/Stage_Ackbar.bin"));
	Load_Data_Effects();
	Load_Data_Decals();
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	m_pUI_Manager->Render_UIGroup(true, "HUD_State");
	m_pUI_Manager->Render_UIGroup(true, "HUD_WeaponSlot");

	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};
	pDesc.isFadeIn = true;
	pDesc.isLevelChange = true;
	pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
		return E_FAIL;

	m_iCamSize =  m_pGameInstance->Get_GameObjects_Ref(/*m_pGameInstance->Get_CurrentLevel()*/LEVEL_ACKBAR, TEXT("Layer_Camera")).size();

	_vector vEye = { 86.f, 100.f, -163.f, 1.f };
	_vector vFocus = { 86.f, 0.f, 0.f, 1.f };
	m_pGameInstance->Set_ShadowEyeFocus(vEye, vFocus, 0.3f);


	////비동기 저장
	//auto futures = m_pGameInstance->AddWork([this]() {
	//	//초기값 몬스터 저장
	//	vector<_tagMonsterInit_Property> vecMonsterInitProperty;
	//	list<CGameObject*> pList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_ACKBAR, L"Layer_Monster");
	//	m_pvecMonsterInitProperty.resize(pList.size());
	//	_uint iIndex = 0;
	//	for (auto& pMonster : pList)
	//	{
	//		m_pvecMonsterInitProperty[iIndex].vPos = dynamic_cast<CMonster*>(pMonster)->Get_InitPos();
	//		m_pvecMonsterInitProperty[iIndex].strMonsterTag = pMonster->Get_ProtoTypeTag();
	//		iIndex++;

	//	}
	//	wstring wstrlevelName = Get_CurLevelName(m_pGameInstance->Get_CurrentLevel());
	//	wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrlevelName + L".dat";
	//	Engine::Save_Data(wstrFilePath.c_str(), false, m_pvecMonsterInitProperty.size(), m_pvecMonsterInitProperty.data());
	//});

	CInitLoader<LEVEL, const wchar_t*>* initLoader = new CInitLoader<LEVEL, const wchar_t*>(&initLoader);
	initLoader->Save_Start(LEVEL_ACKBAR, L"Layer_Monster");


	CInitLoader<LEVEL, const wchar_t*>* initTriggerLoader = new CInitLoader<LEVEL, const wchar_t*>(&initTriggerLoader);
	initTriggerLoader->Save_TriggerStart(LEVEL_ACKBAR, L"Layer_Trigger");

	// UI Manaver로 UI Level 생성하기
	CUI_Manager::GetInstance()->Create_LevelUI();

	m_pGameInstance->StopAll();
	m_pGameInstance->Disable_Echo();
	//m_pGameInstance->PlayBGM(TEXT("BGM_Ackbar.mp3"), 0.2f);

	return S_OK;
}

void CLevel_Ackbar::Tick(_float fTimeDelta)
{
	m_pUI_Manager->Tick(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_pGameInstance->LightOn(0);
	}
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		m_pGameInstance->LightOff(0);
	}

	

#ifdef _DEBUG
	//카메라 전환 ~ 키
	//카메라 전환 ~ 키
	//카메라 전환 ~ 키
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		m_iMainCameraIdx++;
		if (m_iCamSize <= m_iMainCameraIdx)
		{
			m_iMainCameraIdx = 0;
		}
		m_pGameInstance->Set_MainCamera(m_iMainCameraIdx);
	}

	SetWindowText(g_hWnd, TEXT("LEVEL ACKBAR"));
#endif

	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		m_pGameInstance->Scene_Change(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_JUGGLAS));
	}
}

void CLevel_Ackbar::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

HRESULT CLevel_Ackbar::Ready_Lights()
{
	m_pGameInstance->Light_Clear();

	//FireFly Light
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 15.f;
	LightDesc.vDiffuse = _float4(1.f, 1.0f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.0f, 0.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);
	m_pGameInstance->LightOff(0);

	//Player Ambient Light
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	LightDesc.fRange = 15.f;
	LightDesc.vDiffuse = _float4(44.f / 255.f, 41.f / 255.f, 50.f / 255.f, 1.f);
	LightDesc.vAmbient = _float4(108.f / 255.f, 108.f / 255.f, 108.f / 255.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.0f, 0.f, 1.f);
	m_pGameInstance->Add_Light(LightDesc);

	//Yantari Ambient Light
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(-1000.f, -1000.f, -1000.f, 1.f);
	LightDesc.fRange = 20.f;
	LightDesc.vDiffuse = _float4(138.f / 255.f, 138.f / 255.f, 138.f / 255.f, 1.f);
	LightDesc.vAmbient = _float4(140.f / 255.f, 221.f / 255.f, 221.f / 255.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.0f, 0.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);

	Load_Lights();

	m_pGameInstance->LightOn(1);
	m_pGameInstance->LightOn(2);

	//LIGHT_DESC			LightDesc{};

	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	//m_pGameInstance->Add_Light(LightDesc);


	//ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
	//LightDesc.fRange = 20.f;
	//LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//m_pGameInstance->Add_Light(LightDesc);

	//ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(40.f, 5.f, 20.f, 1.f);
	//LightDesc.fRange = 20.f;
	//LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//m_pGameInstance->Add_Light(LightDesc);
	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_Camera(const wstring & strLayerTag)
{
	m_pGameInstance->Clear_Cameras();
	CFreeCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;

	//CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f); // _float4(71.1f, 542.f, 78.f, 1.f);
	//CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f); // _float4(71.1f, 522.f, 98.f, 1.f);

	CameraDesc.vEye = _float4(140.f, 542.f, 78.f, 1.f);
	CameraDesc.vAt = _float4(140.f, 522.f, 98.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);	

	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
		return E_FAIL;

	 CThirdPersonCamera::THIRDPERSONCAMERA_DESC pTPCDesc = {};

	 pTPCDesc.fSensor = 0.1f;

	 pTPCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	 pTPCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	 pTPCDesc.fFovy = XMConvertToRadians(60.f);
	 pTPCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	 pTPCDesc.fNear = 0.1f;
	 pTPCDesc.fFar = 3000.f;

	 pTPCDesc.fSpeedPerSec = 40.f;
	 pTPCDesc.fRotationPerSec = XMConvertToRadians(90.f);
	 pTPCDesc.pPlayerTrans = dynamic_cast<CTransform*>( m_pGameInstance->Get_Component(LEVEL_ACKBAR, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_ThirdPersonCamera"), &pTPCDesc)))
		 return E_FAIL;

	/* CSideViewCamera::SIDEVIEWCAMERA_DESC pSVCDesc = {};

	 pSVCDesc.fSensor = 0.1f;

	 pSVCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	 pSVCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	 pSVCDesc.fFovy = XMConvertToRadians(60.f);
	 pSVCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	 pSVCDesc.fNear = 0.1f;
	 pSVCDesc.fFar = 3000.f;

	 pSVCDesc.fSpeedPerSec = 40.f;
	 pSVCDesc.fRotationPerSec = XMConvertToRadians(90.f);
	 pSVCDesc.pPlayerTrans = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_ACKBAR, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_SideViewCamera"), &pSVCDesc)))
		 return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Particle_Rect"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
	//	return E_FAIL;

	//for (size_t i = 0; i < 60; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Explosion"))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_LandObjects()
{
	/*CLandObject::LANDOBJ_DESC		LandObjDesc= {};
	
	LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_ACKBAR, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_ACKBAR, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));*/

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;



	 //Npc
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Rlya"))))
		return E_FAIL;

	//Npc
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Valnir"))))
		return E_FAIL;





	CSavePoint::_tagSavePoint_Desc savePointDesc;
	savePointDesc.vPosition = _float3(-17.661, 9.068f, -42.096f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_SavePoint"), TEXT("Prototype_GameObject_SavePoint"), &savePointDesc)))
		return E_FAIL;




	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_Trigger()
{
	_float4x4 WorldMatrix;
	//For . GrassLand
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranslation(225.1f, 3.71f, -151.766f));
		CMap_Element::MAP_ELEMENT_DESC pDesc{};

		pDesc.mWorldMatrix = WorldMatrix;
		pDesc.TriggerType = CEventTrigger::TRIGGER_TYPE::TRIG_SCENE_CHANGE;

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
			return E_FAIL;
	}

	// Portal UI
	CUIGroup_Portal::UIGROUP_PORTAL_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.ePortalLevel = LEVEL_GRASSLAND;
	pDesc.isPic = true;
	pDesc.iPicNum = 3;
	pDesc.fAngle = 230.f; // 190
	pDesc.fDistance = 15.f;
	pDesc.vPos = XMVectorSet(223.1f, 7.71f, -155.766f, 1.f); // 225.1f, 7.71f, -151.766f
	pDesc.fScale = _float2(11.f, 19.f);
	CUI_Manager::GetInstance()->Create_PortalUI(&pDesc);



	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Item()
{

	_float3 fPosArray[] = {
	_float3(166.3f, 14.7f, 13.0f),
	_float3(193.0f, 21.3f, -25.3f),
	_float3(-4.4f, 27.0f, -122.7f),
	_float3(102.0f, 18.5f, -110.0f),
	_float3(191.0f, 8.8f, -46.0f)
	};
	_uint arraySize = sizeof(fPosArray) / sizeof(_float3);

	for (int i = 0; i < arraySize; i++)
	{
		CItem::ITEM_DESC desc = {};
		desc.vPosition = fPosArray[i];
		
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &desc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_Player(const wstring & strLayerTag)
{
	CLandObject::LANDOBJ_DESC LandObjDesc = {};

	LandObjDesc.mWorldMatrix._41 = 101.1f;
	LandObjDesc.mWorldMatrix._42 = 63.5f;
	LandObjDesc.mWorldMatrix._43 = 4.4f;
	LandObjDesc.mWorldMatrix._44 = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Player"), &LandObjDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Ackbar::Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Monster"), pLandObjDesc)))
	//		return E_FAIL;
	//}0

	// 테스트 위해 임의로 생성
	/*CMonster::MST_DESC* pDesc = static_cast<CMonster::MST_DESC*>(pLandObjDesc);

	pDesc->eLevel = LEVEL_ACKBAR;*/

	// Prototype_GameObject_Boss_Juggulus   Prototype_GameObject_Ghost    Prototype_GameObject_Legionnaire_Gun
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Boss_Juggulus"), pLandObjDesc)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Legionnaire_Gun"), pLandObjDesc)))
	//	return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Ghost"), pLandObjDesc)))
	//	return E_FAIL;
	//

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Homonculus"), pLandObjDesc)))
	//	return E_FAIL;




	////for (size_t i = 0; i < 5; i++)
	//{
		CLandObject::LANDOBJ_DESC LandObjDesc = {};
		LandObjDesc.mWorldMatrix._41 = 101.1f;
		LandObjDesc.mWorldMatrix._42 = 63.5f;
		LandObjDesc.mWorldMatrix._43 = 4.4f;
		LandObjDesc.mWorldMatrix._44 = 1.f;
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, strLayerTag, TEXT("Prototype_GameObject_Mantari"), &LandObjDesc)))
			return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Ackbar::Load_LevelData(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
		return E_FAIL;

	char szName[MAX_PATH] = "";
	char szLayer[MAX_PATH] = "";
	char szModelName[MAX_PATH] = "";
	_float4x4 WorldMatrix;
	CModel::MODELTYPE eModelType = CModel::TYPE_END;
	DWORD dwByte(0);
	_tchar wszName[MAX_PATH] = TEXT("");
	_tchar wszLayer[MAX_PATH] = TEXT("");
	_tchar wszModelName[MAX_PATH] = TEXT("");
	_uint   iTriggerType = 0;

	// 모델 종류별로 월드 매트릭스를 저장할 맵
	map<wstring, vector<_float4x4*>> modelMatrices;

	
	// 생성된 객체 로드
	while (true)
	{
		ZeroMemory(wszName, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszLayer, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszModelName, sizeof(_tchar) * MAX_PATH);

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
		{
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CMap_Element::MAP_ELEMENT_DESC pDesc{};

			pDesc.mWorldMatrix = WorldMatrix;
			pDesc.TriggerType = iTriggerType;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
				return E_FAIL;
		}
		else if (strcmp(szName, "Prototype_GameObject_TreasureChest") == 0)
		{
			ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
			ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CMap_Element::MAP_ELEMENT_DESC pChestDesc{};

			MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

			pChestDesc.wstrLayer = wszLayer;
			pChestDesc.wstrModelName = wszModelName;
			pChestDesc.mWorldMatrix = WorldMatrix;
			pChestDesc.TriggerType = iTriggerType;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_TreasureChest"), TEXT("Prototype_GameObject_TreasureChest"), &pChestDesc)))
				return E_FAIL;
		}
		else
		{
			ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

			MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

			if (0 == dwByte)
				break;


			if (wstring(wszName) == TEXT("Prototype_GameObject_Passive_Element"))
			{
				// 모델 이름별로 월드 매트릭스 저장
				_float4x4* pWorldMatrix = new _float4x4(WorldMatrix);
				modelMatrices[wszModelName].push_back(pWorldMatrix);
			}
			else if(0 == strcmp(szLayer, ("Layer_Monster")))
			{
				// 다른 객체들은 개별적으로 생성

			
				const TCHAR* pModifiedName = nullptr;

				CLandObject::LANDOBJ_DESC pDesc{};
				pDesc.mWorldMatrix = WorldMatrix;
				if (wstring(wszName) == TEXT("Prototype_GameObject_Mantari"))
				{
					pModifiedName = TEXT("Prototype_GameObject_Yantari");
				}
				else
				{
					pModifiedName = wszName;
				}
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, wszLayer, pModifiedName, &pDesc)))
					return E_FAIL;

			}
			else
			{
				// 다른 객체들은 개별적으로 생성
				CMap_Element::MAP_ELEMENT_DESC pDesc{};

				pDesc.mWorldMatrix = WorldMatrix;
				pDesc.wstrModelName = wszModelName;
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, wszLayer, wszName, &pDesc)))
					return E_FAIL;
			}
		}
	

	
	}

	CloseHandle(hFile);

	 //Passive_Element 객체들을 인스턴싱하여 생성
	for (const auto& pair : modelMatrices)
	{
		CMap_Element::MAP_ELEMENT_DESC pDesc{};
		pDesc.WorldMats = pair.second;
		pDesc.iInstanceCount = pair.second.size();
		pDesc.wstrModelName = pair.first;

		pDesc.mWorldMatrix = WorldMatrix;
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Passive_Element"),
			TEXT("Prototype_GameObject_Passive_Element"), &pDesc)))
			return E_FAIL;
	}

	// 동적 할당된 메모리 해제
	//for (auto& pair : modelMatrices)
	//{
	//	for (auto pWorldMatrix : pair.second)
	//	{
	//		Safe_Delete(pWorldMatrix);
	//	}
	//}

	return S_OK;
}


HRESULT CLevel_Ackbar::Load_Data_Effects()
{
	const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_Ackbar_Effects.bin";
	HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
		return E_FAIL;

	DWORD dwByte(0);
	_uint iEffectCount = 0;

	// 이펙트 개수 읽기
	ReadFile(hFile, &iEffectCount, sizeof(_uint), &dwByte, nullptr);

	// 사용자 정의 비교 함수
	auto cmp = [](const tuple<wstring, float, float, float, bool>& a, const tuple<wstring, float, float, float, bool>& b) {
		if (get<0>(a) != get<0>(b)) return get<0>(a) < get<0>(b);
		if (get<1>(a) != get<1>(b)) return get<1>(a) < get<1>(b);
		if (get<2>(a) != get<2>(b)) return get<2>(a) < get<2>(b);
		if (get<3>(a) != get<3>(b)) return get<3>(a) < get<3>(b);
		return get<4>(a) < get<4>(b);
		};

	// Tree 객체들을 그룹화하기 위한 맵
	// key: (모델 이름, LeafCol, isBloom)의 조합, value: 해당 그룹의 월드 매트릭스 벡터
	map<tuple<wstring, float, float, float, bool>, vector<_float4x4*>, decltype(cmp)> treeGroups(cmp);

	for (_uint i = 0; i < iEffectCount; ++i)
	{
		char szName[MAX_PATH] = "";
		char szModelName[MAX_PATH] = "";
		char szLayer[MAX_PATH] = "";
		_float4x4 WorldMatrix;

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		wstring wsName, wsModelName, wsLayer;
		int nNameLen = MultiByteToWideChar(CP_ACP, 0, szName, -1, NULL, 0);
		int nModelNameLen = MultiByteToWideChar(CP_ACP, 0, szModelName, -1, NULL, 0);
		int nLayerLen = MultiByteToWideChar(CP_ACP, 0, szLayer, -1, NULL, 0);
		wsName.resize(nNameLen);
		wsModelName.resize(nModelNameLen);
		wsLayer.resize(nLayerLen);
		MultiByteToWideChar(CP_ACP, 0, szName, -1, &wsName[0], nNameLen);
		MultiByteToWideChar(CP_ACP, 0, szModelName, -1, &wsModelName[0], nModelNameLen);
		MultiByteToWideChar(CP_ACP, 0, szLayer, -1, &wsLayer[0], nLayerLen);

		if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Fire_Effect") == 0)
		{
			CFireEffect::FIREEFFECTDESC FireDesc{};
			FireDesc.vStartPos = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };
			FireDesc.vStartScale = { 1.f, 1.f };
			FireDesc.mWorldMatrix = WorldMatrix;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, wsLayer.c_str(), wsName.c_str(), &FireDesc)))
				return E_FAIL;
		}
		//else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Grass") == 0)
		//{
		//	CGrass::GRASS_DESC GrassDesc{};
		//	GrassDesc.mWorldMatrix = WorldMatrix;

		//	// 추가 정보 읽기
		//	_float3 TopCol, BotCol;
		//	ReadFile(hFile, &TopCol, sizeof(_float3), &dwByte, nullptr);
		//	ReadFile(hFile, &BotCol, sizeof(_float3), &dwByte, nullptr);

		//	GrassDesc.vTopCol = TopCol;
		//	GrassDesc.vBotCol = BotCol;

		//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, wsLayer.c_str(), wsName.c_str(), &GrassDesc)))
		//		return E_FAIL;
		//}

		else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Tree") == 0)
		{
			_float3 LeafCol;
			bool isBloom;
			ReadFile(hFile, &LeafCol, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &isBloom, sizeof(bool), &dwByte, nullptr);

			// 트리 그룹화
			auto key = make_tuple(wsModelName, LeafCol.x, LeafCol.y, LeafCol.z, isBloom);
			_float4x4* pWorldMatrix = new _float4x4(WorldMatrix);
			treeGroups[key].emplace_back(pWorldMatrix);
		}
	}

	CloseHandle(hFile);

	// Tree 객체들을 인스턴싱하여 생성
	for (const auto& group : treeGroups)
	{
		CTree::TREE_DESC TreeDesc{};
		TreeDesc.wstrModelName = get<0>(group.first);
		TreeDesc.vLeafCol = _float3(get<1>(group.first), get<2>(group.first), get<3>(group.first));
		TreeDesc.isBloom = get<4>(group.first);
		TreeDesc.WorldMats = group.second;
		TreeDesc.iInstanceCount = group.second.size();

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Vegetation"), TEXT("Prototype_GameObject_Tree"), &TreeDesc)))
			return E_FAIL;
	}

	//for (auto& group : treeGroups)
	//{
	//	for (auto& pMatrix : group.second)
	//	{
	//		delete pMatrix;
	//	}
	//}

#ifdef _DEBUG
//	MSG_BOX("Effect Data Load");
#endif

	return S_OK;
}


HRESULT CLevel_Ackbar::Load_Data_Decals()
{
	const wchar_t* wszFileName = L"../Bin/MapData/DecalsData/Stage_Ackbar_Decals.bin";
	HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte(0);
	_uint iDecalCount = 0;
	ReadFile(hFile, &iDecalCount, sizeof(_uint), &dwByte, nullptr);
	if (0 == dwByte)
	{
		CloseHandle(hFile);
		return S_OK; // 파일이 비어있는 경우
	}

	for (_uint i = 0; i < iDecalCount; ++i)
	{
		_char szName[MAX_PATH] = "";
		_char szLayer[MAX_PATH] = "";
		_float4x4 WorldMatrix;
		_uint     decalIdx = 0;

		ReadFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &decalIdx, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CDecal::DECAL_DESC pDesc{};

		pDesc.mWorldMatrix = WorldMatrix;
		pDesc.iDecalIdx = decalIdx;

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_Decal"), &pDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);


#ifdef _DEBUG
	//MSG_BOX("Decal Data Loaded");
#endif
	return S_OK;
}

void CLevel_Ackbar::Load_Lights()
{
	list<CLight*> lights = m_pGameInstance->Get_Lights();
	for (auto& iter : lights)
	{
		iter->LightOff();
	}

	//switch (iStageIdx)
	//{
	//case STAGE_HOME:
	//	m_LightsDataPath = L"../Bin/MapData/LightsData/Stage_Lights.dat";
	//	break;
	//case STAGE_ONE:
	//	m_LightsDataPath = L"../Bin/MapData/LightsData/Stage1_Lights.dat";
	//	break;
	//case STAGE_TWO:
	//	m_LightsDataPath = L"../Bin/MapData/LightsData/Stage2_Lights.dat";
	//	break;
	//case STAGE_THREE:
	//	m_LightsDataPath = L"../Bin/MapData/LightsData/Stage3_Lights.dat";
	//	break;
	//case STAGE_BOSS:
	//	m_LightsDataPath = L"../Bin/MapData/LightsData/Stage4_Lights.dat";
	//	break;
	//default:
	//	MSG_BOX("Setting File Name is Failed");
	//	return;
	//}

	wstring LightsDataPath = L"../Bin/MapData/LightsData/Ackbar_Lights.dat";

	HANDLE hFile = CreateFile(LightsDataPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
	{
		MSG_BOX("hFile is nullptr");
		return;
	}

	DWORD dwByte(0);
	_uint iLightCount = 0;
	ReadFile(hFile, &iLightCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLightCount; ++i)
	{
		LIGHT_DESC desc{};
		ReadFile(hFile, &desc.eType, sizeof(LIGHT_DESC::TYPE), &dwByte, nullptr);
		ReadFile(hFile, &desc.vDiffuse, sizeof(XMFLOAT4), &dwByte, nullptr);
		ReadFile(hFile, &desc.vAmbient, sizeof(XMFLOAT4), &dwByte, nullptr);
		ReadFile(hFile, &desc.vSpecular, sizeof(XMFLOAT4), &dwByte, nullptr);

		//desc.vDiffuse = _float4(1.0f,0.0f,0.0f,1.0f);
		switch (desc.eType)
		{
		case LIGHT_DESC::TYPE_DIRECTIONAL:
			ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
			break;
		case LIGHT_DESC::TYPE_POINT:
			ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
			break;
		case LIGHT_DESC::TYPE_SPOTLIGHT:
			ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
			ReadFile(hFile, &desc.innerAngle, sizeof(float), &dwByte, nullptr);
			ReadFile(hFile, &desc.outerAngle, sizeof(float), &dwByte, nullptr);
			break;
		}

		if (0 == dwByte)
			break;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
		{
			MSG_BOX("Failed to Add Light");
			return;
		}
	}

	CloseHandle(hFile);

#ifdef _DEBUG
	//MSG_BOX("Lights Data Load");
#endif
	return;
}

CLevel_Ackbar * CLevel_Ackbar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Ackbar*		pInstance = new CLevel_Ackbar(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Ackbar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ackbar::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Manager);
	

}

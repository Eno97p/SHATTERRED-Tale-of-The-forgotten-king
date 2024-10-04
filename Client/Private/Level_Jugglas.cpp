#include "stdafx.h"
#include "..\Public\Level_Jugglas.h"
#include "GameInstance.h"
#include "UI_Manager.h"

//Cameras
#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "SideViewCamera.h"
#include "TransitionCamera.h"
#include "CutSceneCamera.h"

#include "Map_Element.h"
#include "Monster.h"

#include "Light.h"
#include "UI_FadeInOut.h"
#include "FireEffect.h"


#include "Trap.h"
#include "Item.h"

#include "BlastWall.h"
#include"CInitLoader.h"

#include"EventTrigger.h"

#include "SavePoint.h"
 vector< CMap_Element::MAP_ELEMENT_DESC> vecMapElementDesc;



CLevel_Jugglas::CLevel_Jugglas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())

{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_Jugglas::Initialize()
{
	CRenderer::FOG_DESC fogDesc{};
	fogDesc.vFogColor = { 0.137f, 0.211f, 0.115f, 1.f };
	fogDesc.vFogColor2 = { 0.814f, 0.814f, 0.814f, 1.f };
	fogDesc.fFogRange = 577.f;
	fogDesc.fFogHeightFalloff = 0.096f;
	fogDesc.fFogGlobalDensity = 0.346f;
	fogDesc.fFogTimeOffset = 0.577f;
	fogDesc.fFogTimeOffset2 = 2.019f;
	fogDesc.fNoiseIntensity = 1.010f;
	fogDesc.fNoiseIntensity2 = 3.125f;
	fogDesc.fNoiseSize = 0.008654f;
	fogDesc.fNoiseSize2 = 0.003365f;

	fogDesc.fFogBlendFactor = 0.4190f;
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	//m_pGameInstance->Set_FogOption({ 0.272f, 0.252f, 0.367f, 1.f }, 400.f, 0.08f, 0.005f, 10.f, 1.f, 0.5f);

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Trigger()))
			return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	Load_LevelData(TEXT("../Bin/MapData/Stage_Jugglas.bin"));
	Load_LevelData(TEXT("../Bin/MapData/Stage_Jugglas_Traps.bin")); //Traps

	Load_Data_Effects();

	m_pUI_Manager->Render_UIGroup(true, "HUD_State");
	m_pUI_Manager->Render_UIGroup(true, "HUD_WeaponSlot");


	m_iCamSize =  m_pGameInstance->Get_GameObjects_Ref(/*m_pGameInstance->Get_CurrentLevel()*/LEVEL_JUGGLAS, TEXT("Layer_Camera")).size();

	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

	pDesc.isFadeIn = true;
	pDesc.isLevelChange = true;
	pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), TEXT("Prototype_GameObject_RotateGate"))))
		return E_FAIL;	
	
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), TEXT("Prototype_GameObject_TreasureChest"))))
		return E_FAIL;

	_vector vEye = { -150.f, 100.f, 100.f, 1.f };
	_vector vFocus = { -150.f, 0.f, 0.f, 1.f };
	m_pGameInstance->Set_ShadowEyeFocus(vEye, vFocus, 0.5f);

	////비동기 저장
	//auto futures = m_pGameInstance->AddWork([this]() {
	//	//초기값 몬스터 저장
	//	vector<_tagMonsterInit_Property> vecMonsterInitProperty;
	//	list<CGameObject*> pList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_JUGGLAS, L"Layer_Monster");
	//	m_pvecMonsterInitProperty.resize(pList.size());
	//	_uint iIndex = 0;
	//	for (auto& pMonster : pList)
	//	{
	//		m_pvecMonsterInitProperty[iIndex].vPos = dynamic_cast<CMonster*>(pMonster)->Get_InitPos();
	//		//wcscpy_s(vecMonsterInitProperty[iIndex].strName, MAX_PATH, pMonster->Get_ProtoTypeTag().c_str());
	//		iIndex++;

	//	}
	//	wstring wstrlevelName = Get_CurLevelName(m_pGameInstance->Get_CurrentLevel());
	//	wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrlevelName + L".dat";
	//	//const char* Test = Client::LevelNames[m_pGameInstance->Get_CurrentLevel()];
	//	Engine::Save_Data(wstrFilePath.c_str(), false, m_pvecMonsterInitProperty.size(), m_pvecMonsterInitProperty.data());
	//});




	// UI Manaver로 UI Level 생성하기
	CUI_Manager::GetInstance()->Create_LevelUI();
	CUI_Manager::GetInstance()->SetVisitLevel(LEVEL_JUGGLAS);




	CInitLoader<LEVEL, const wchar_t*>* initLoader = new CInitLoader<LEVEL, const wchar_t*>(&initLoader);
	initLoader->Save_Start(LEVEL_JUGGLAS, L"Layer_Monster");
	initLoader->Save_BlastWallStart(LEVEL_JUGGLAS, L"Layer_BlastWall");


	CInitLoader<LEVEL, const wchar_t*>* initTriggerLoader = new CInitLoader<LEVEL, const wchar_t*>(&initTriggerLoader);
	initTriggerLoader->Save_TriggerStart(LEVEL_JUGGLAS, L"Layer_Trigger");


	



	return S_OK;
}

void CLevel_Jugglas::Tick(_float fTimeDelta)
{
	m_pUI_Manager->Tick(fTimeDelta);


//#ifdef _DEBUG

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

	//if (m_pGameInstance->Key_Down(DIK_4))
	//{
	//	CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

	//	//pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	//	//pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	//	pTCDesc.fFovy = XMConvertToRadians(60.f);
	//	pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	//	pTCDesc.fNear = 0.1f;
	//	pTCDesc.fFar = 3000.f;

	//	pTCDesc.fSpeedPerSec = 40.f;
	//	pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

	//	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
	//	{
	//		MSG_BOX("FAILED");
	//	}
	//	return;
	//}

	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
#endif
}

void CLevel_Jugglas::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

HRESULT CLevel_Jugglas::Ready_Lights()
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

	//Juggulus Ambient Light
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(-1000.f, -1000.f, -1000.f, 1.f);
	LightDesc.fRange = 20.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f , 1.f);
	LightDesc.vAmbient = _float4(169.f / 255.f, 212.f / 255.f, 255.f / 255.f, 1.f);
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

HRESULT CLevel_Jugglas::Ready_Layer_Camera(const wstring & strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
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
	 pTPCDesc.pPlayerTrans = dynamic_cast<CTransform*>( m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_ThirdPersonCamera"), &pTPCDesc)))
		 return E_FAIL;

	 CCutSceneCamera::CUTSCENECAMERA_DESC pCSCdesc = {};


	 pCSCdesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	 pCSCdesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	 pCSCdesc.fFovy = XMConvertToRadians(60.f);
	 pCSCdesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	 pCSCdesc.fNear = 0.1f;
	 pCSCdesc.fFar = 3000.f;

	 pCSCdesc.fSpeedPerSec = 40.f;
	 pCSCdesc.fRotationPerSec = XMConvertToRadians(90.f);

	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_CutSceneCamera"), &pCSCdesc)))
		 return E_FAIL;

	 CSideViewCamera::SIDEVIEWCAMERA_DESC pSVCDesc = {};

	 pSVCDesc.fSensor = 0.1f;

	 pSVCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	 pSVCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	 pSVCDesc.fFovy = XMConvertToRadians(60.f);
	 pSVCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	 pSVCDesc.fNear = 0.1f;
	 pSVCDesc.fFar = 3000.f;

	 pSVCDesc.fSpeedPerSec = 40.f;
	 pSVCDesc.fRotationPerSec = XMConvertToRadians(90.f);
	 pSVCDesc.pPlayerTrans = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_SideViewCamera"), &pSVCDesc)))
		 return E_FAIL;

	 m_pGameInstance->Set_MainCamera(CAM_THIRDPERSON);
	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Particle_Rect"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
	//	return E_FAIL;

	//for (size_t i = 0; i < 60; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Explosion"))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_Layer_Trigger()
{


	_float4x4 WorldMatrix;
	//For . GrassLand
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranslation(-255.159f, 4.27f, 27.176f));
		CMap_Element::MAP_ELEMENT_DESC pDesc{};
	
		pDesc.mWorldMatrix = WorldMatrix;
		pDesc.TriggerType = CEventTrigger::TRIGGER_TYPE::TRIG_SCENE_CHANGE;
	
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
			return E_FAIL;

		// Portal UI
		CUIGroup_Portal::UIGROUP_PORTAL_DESC pUIDesc{};
		pUIDesc.eLevel = LEVEL_STATIC;
		pUIDesc.ePortalLevel = LEVEL_GRASSLAND;
		pUIDesc.isPic = true;
		pUIDesc.iPicNum = 3;
		pUIDesc.fAngle = 110.f; // 100
		pUIDesc.fDistance = 7.f;
		pUIDesc.vPos = XMVectorSet(-256.922f, 25.179f, -185.385f, 1.f); // -256.922f, 22.179f, -185.385f
		pUIDesc.fScale = _float2(5.5f, 9.f);
		CUI_Manager::GetInstance()->Create_PortalUI(&pUIDesc);
	}




	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC		LandObjDesc{};
	
	LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjDesc)))
		return E_FAIL;

	CLandObject::LANDOBJ_DESC landObjDesc;
	landObjDesc.mWorldMatrix._41 = -200.f;
	landObjDesc.mWorldMatrix._42 = -25.f;
	landObjDesc.mWorldMatrix._43 = 0.f;
	landObjDesc.mWorldMatrix._44 = 1.f;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Platform"), TEXT("Prototype_GameObject_FallPlatform"), &landObjDesc)))
		return E_FAIL;

	_float3 fPosArray[] = {
	_float3(-73.8f, 6.7f, -7.2f),
	_float3(-107.2f, 3.3f, -15.9f),
	_float3(-201.5f, 3.5f, -15.4f),
	};

	_uint arraySize = sizeof(fPosArray) / sizeof(_float3);

	//CItem::ITEM_DESC desc;
	//for (_uint i = 0; i < arraySize; i++)
	//{
	//	desc.vPosition = fPosArray[i];
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &desc)))
	//		return E_FAIL;
	//}

			 -63.043f, -0.08f, -1.715f 		;	
			 -73.063f, -10.458f, -2.202f 	;
			 -83.620f, 0.144f, -2.700f 		;
			 -102.285f, 0.098f, -2.700f 	;
			 -133.776f, -5.287f, -1.712f 	;
			 -140.611f, -5.343f, -2.091f 	;
			 -144.392f, -5.325f, -1.826f 	;
			 -167.43f, 0.348f, -2.02f 		;
			 -171.177f, 0.446f, -1.988f 	;
	

	//나중에 바이너리로 채워서 만들기
	_float Average = -0.5f;
	_matrix fWorldMatrixArray[] = {
		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) * 
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) * 
		XMMatrixTranslation(-63.1f  + Average , -0.06f, -1.715f) } ,				//1번째 Balst Wall

		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-73.063f + Average, -10.458f, -2.202f) } ,		//2번째 Balst Wall

		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-83.620f + Average, 0.144f, -2.700f) } ,		//3번째 Balst Wall

		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-102.285f + Average, 0.098f, -2.700f) } ,		//4번째 Balst Wall

			{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-133.776f + Average, 0.07f, -1.712f) } ,		//5번째 Balst Wall

			{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-140.611f + Average, 0.043f, -2.091f) } ,		//6번째 Balst Wall

		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f))*
		XMMatrixTranslation(-144.392f + Average, -5.325f, -1.826f) } ,		//7번째 Balst Wall

	

		{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		XMMatrixTranslation(-167.43f + Average, 0.348f, -2.02f) } ,		//8번째 Balst Wall

		//{ XMMatrixScaling(0.2f, 0.2f, 0.2f) *
		//XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)) *
		//XMMatrixTranslation(-171.177f + Average, 0.446f, -1.988f) } ,		//9번째 Balst Wall


	};

	arraySize = sizeof(fWorldMatrixArray) / sizeof(_matrix);

	CBlastWall::CBlastWall_DESC BlastWallDesc;
	for (_uint i = 0; i < arraySize; i++)
	{
		XMStoreFloat4x4(&BlastWallDesc.fWorldMatrix, fWorldMatrixArray[i]);
		//BlastWallDesc.mWorldMatrix = fWorldMatrix[i];
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_BlastWall"), TEXT("Prototype_GameObject_BlastWall"), &BlastWallDesc)))
			return E_FAIL;
	}
	





	CSavePoint::_tagSavePoint_Desc savePointDesc;
	savePointDesc.vPosition = _float3(-236, 4.25f, 8.204f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_SavePoint"), TEXT("Prototype_GameObject_SavePoint"), &savePointDesc)))
		return E_FAIL;



	//Npc
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Choron"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_Layer_Player(const wstring & strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
	pLandObjDesc->mWorldMatrix._41 = -8.3f;
	pLandObjDesc->mWorldMatrix._42 = 3.5f;
	pLandObjDesc->mWorldMatrix._43 = -2.4f;
	pLandObjDesc->mWorldMatrix._44 = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Player"), pLandObjDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Jugglas::Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Monster"), pLandObjDesc)))
	//		return E_FAIL;
	//}0

	// 테스트 위해 임의로 생성
	/*CMonster::MST_DESC* pDesc = static_cast<CMonster::MST_DESC*>(pLandObjDesc);

	pDesc->eLevel = LEVEL_JUGGLAS;*/

	// Prototype_GameObject_Boss_Juggulus   Prototype_GameObject_Ghost    Prototype_GameObject_Legionnaire_Gun

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Legionnaire_Gun"), pLandObjDesc)))
	//	return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Ghost"), pLandObjDesc)))
	//	return E_FAIL;
	//

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Homonculus"), pLandObjDesc)))
	//	return E_FAIL;



	////for (size_t i = 0; i < 5; i++)
	//{

		//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, strLayerTag, TEXT("Prototype_GameObject_Mantari"), pLandObjDesc)))
		//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Jugglas::Load_LevelData(const _tchar* pFilePath)
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
	_double dStartTime = 0;

	// 모델 종류별로 월드 매트릭스를 저장할 맵
	map<wstring, vector<_float4x4*>> modelMatrices;

	_uint iTriggerCount = 0;


	
	

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
			
			if (iTriggerCount++ < 2)
			{
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_NonDeleteTrigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
					return E_FAIL;

				
				
				

			}
			

		}
		else if (strcmp(szName, "Prototype_GameObject_Trap") == 0)
		{
			ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
			ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &dStartTime, sizeof(_double), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CTrap::TRAP_DESC pTrapDesc{};

			MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

			pTrapDesc.wstrLayer = wszLayer;
			pTrapDesc.wstrModelName = wszModelName;
			pTrapDesc.mWorldMatrix = WorldMatrix;
			pTrapDesc.TriggerType = iTriggerType;
			pTrapDesc.dStartTimeOffset = dStartTime;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Trap"), TEXT("Prototype_GameObject_Trap"), &pTrapDesc)))
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

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_TreasureChest"), TEXT("Prototype_GameObject_TreasureChest"), &pChestDesc)))
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
			else
			{
				//for (int i = 0; i < 40; i++)
				if (wstring(wszName) == TEXT("Prototype_GameObject_BossDeco"))
				{
					CMap_Element::MAP_ELEMENT_DESC pDesc{};

					pDesc.mWorldMatrix = WorldMatrix;
					pDesc.wstrModelName = wszModelName;
					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_BossDeco"), wszName, &pDesc)))
						return E_FAIL;
				}
				else
				{
					// 다른 객체들은 개별적으로 생성
					CMap_Element::MAP_ELEMENT_DESC pDesc{};


					pDesc.mWorldMatrix = WorldMatrix;
					pDesc.wstrModelName = wszModelName;
					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, wszLayer, wszName, &pDesc)))
						return E_FAIL;
				}


			}
		}



	}

	CloseHandle(hFile);

	// Passive_Element 객체들을 인스턴싱하여 생성
	for (const auto& pair : modelMatrices)
	{
		CMap_Element::MAP_ELEMENT_DESC pDesc{};
		pDesc.WorldMats = pair.second;
		pDesc.iInstanceCount = pair.second.size();
		pDesc.wstrModelName = pair.first;

		pDesc.mWorldMatrix = WorldMatrix;
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"),
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

HRESULT CLevel_Jugglas::Load_Data_Effects()
{
	const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_Juggulas_Effects.bin";
	HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
		return E_FAIL;

	DWORD dwByte(0);
	_uint iEffectCount = 0;

	// 이펙트 개수 읽기
	ReadFile(hFile, &iEffectCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iEffectCount; ++i)
	{
		char szName[MAX_PATH] = "";
		char szLayer[MAX_PATH] = "";
		_float4x4 WorldMatrix;

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		// char 배열을 wstring으로 변환
		wstring wsName, wsLayer;
		int nNameLen = MultiByteToWideChar(CP_ACP, 0, szName, -1, NULL, 0);
		int nLayerLen = MultiByteToWideChar(CP_ACP, 0, szLayer, -1, NULL, 0);
		wsName.resize(nNameLen);
		wsLayer.resize(nLayerLen);
		MultiByteToWideChar(CP_ACP, 0, szName, -1, &wsName[0], nNameLen);
		MultiByteToWideChar(CP_ACP, 0, szLayer, -1, &wsLayer[0], nLayerLen);

		// 이펙트 생성 및 설정
		if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Fire_Effect") == 0)
		{
			CFireEffect::FIREEFFECTDESC FireDesc{};
			FireDesc.vStartPos = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };
			FireDesc.vStartScale = { 1.f, 1.f }; // 스케일은 필요에 따라 조정
			FireDesc.mWorldMatrix = WorldMatrix;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_ACKBAR, wsLayer.c_str(), wsName.c_str(), &FireDesc)))
				return E_FAIL;
		}

	}

	CloseHandle(hFile);

#ifdef _DEBUG
	//MSG_BOX("Effects Data Load");
#endif

	return S_OK;
}

void CLevel_Jugglas::Load_Lights()
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

	wstring LightsDataPath = L"../Bin/MapData/LightsData/Juggulas_Lights.dat";

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

CLevel_Jugglas * CLevel_Jugglas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Jugglas*		pInstance = new CLevel_Jugglas(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Jugglas");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Jugglas::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Manager);
	

}

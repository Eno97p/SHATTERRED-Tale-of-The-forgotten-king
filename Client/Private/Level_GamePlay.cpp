#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "CutSceneCamera.h"
#include "SideViewCamera.h"

#include "Map_Element.h"
#include "Monster.h"

#include "Light.h"
#include "UI_FadeInOut.h"
#include "Level_Loading.h"

#include "Trap.h"
#include "EventTrigger.h"
#include"Item.h"

#include "FireEffect.h"
//#include "Grass.h"
#include "Tree.h"
#include "Decal.h"
#include "SavePoint.h"
#include "CHoverBoard.h"

// test용
#include "Npc_Valnir.h"
#include "NPC_Choron.h"
#include "NPC_Yaak.h"

#include "EffectManager.h"
#include"CInitLoader.h"

#include "UIGroup_Portal.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())

{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	CRenderer::FOG_DESC fogDesc{};
	fogDesc.vFogColor = { 0.095f, 0.130f, 0.255f, 1.f };
	fogDesc.vFogColor2 = { 0.299f, 0.299f, 0.299f, 1.f };
	fogDesc.fFogRange = 576.9f;
	fogDesc.fFogHeightFalloff = 3.341f;
	fogDesc.fFogGlobalDensity = 0.913f;
	fogDesc.fFogTimeOffset = 2.019f;
	fogDesc.fFogTimeOffset2 = 2.308f;
	fogDesc.fNoiseIntensity = 0.0f;
	fogDesc.fNoiseIntensity2 = 0.00f;
	fogDesc.fNoiseSize = 0.030769f;
	fogDesc.fNoiseSize2 = 0.020192f;
	fogDesc.fFogBlendFactor = 0.553f;
	m_pGameInstance->Set_FogOption(fogDesc);
	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	
	//CGameObject::GAMEOBJECT_DESC gameobjDesc;
	//gameobjDesc.mWorldMatrix._41 = 155.f;
	//gameobjDesc.mWorldMatrix._42 = 522.f;
	//gameobjDesc.mWorldMatrix._43 = 113.f;
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_Decal"), &gameobjDesc)))
	//	return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

/*#ifdef _DEBUG
#else
#endif*/ // _DEBUG
	Load_LevelData(TEXT("../Bin/MapData/Stage_Tutorial.bin"));

	Load_Data_Decals();
	//Load_Data_Effects();

	m_pUI_Manager->Render_UIGroup(true, "HUD_State");
	m_pUI_Manager->Render_UIGroup(true, "HUD_WeaponSlot");

#ifdef _DEBUG
	m_iCamSize = m_pGameInstance->Get_GameObjects_Ref(/*m_pGameInstance->Get_CurrentLevel()*/LEVEL_GAMEPLAY, TEXT("Layer_Camera")).size();
#endif


	_vector vEye = { 140.f, 600.f, 97.f, 1.f };
	_vector vFocus = { 151.f, 521.f, 97.f, 1.f };
	m_pGameInstance->Set_ShadowEyeFocus(vEye, vFocus, 0.3f);





	// UI Manaver로 UI Level 생성하기
	CUI_Manager::GetInstance()->Create_LevelUI();
	


	CInitLoader<LEVEL, const wchar_t*>* initLoader = new CInitLoader<LEVEL, const wchar_t*>(&initLoader);
	initLoader->Save_Start(LEVEL_GAMEPLAY, L"Layer_Monster");

	CInitLoader<LEVEL, const wchar_t*>* initTriggerLoader = new CInitLoader<LEVEL, const wchar_t*>(&initTriggerLoader);
	initTriggerLoader->Save_TriggerStart(LEVEL_GAMEPLAY, L"Layer_Trigger");

	Set_Volume();


	// Portal
	CUIGroup_Portal::UIGROUP_PORTAL_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.ePortalLevel = LEVEL_GRASSLAND;
	pDesc.isPic = true;
	pDesc.iPicNum = 3;
	pDesc.fAngle = 190.f;
	pDesc.fDistance = 15.f;
	pDesc.vPos = XMVectorSet(250.f, 523.f, 97.f, 1.f);
	pDesc.fScale = _float2(3.7f, 6.f);
	CUI_Manager::GetInstance()->Create_PortalUI(&pDesc);

	m_pGameInstance->StopAll();
	m_pGameInstance->Disable_Echo();
	//m_pGameInstance->PlayBGM(TEXT("BGM_Gameplay.mp3"), 0.1f);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	m_pUI_Manager->Tick(fTimeDelta);


	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		CLevel* level = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ANDRASARENA);
		m_pGameInstance->Scene_Change(LEVEL_LOADING, level);

		CUI_Manager::GetInstance()->Delete_PortalUI(); // 디버그용

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

	/*list<CGameObject*> temp = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Trigger");
	if (!temp.empty())
	{
		if()


	}*/



	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		CLevel* level = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ACKBAR);
   		m_pGameInstance->Scene_Change(LEVEL_LOADING, level);
		
		CUI_Manager::GetInstance()->Delete_PortalUI(); // 디버그용
		
	}

	/*list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	if (!objs.empty())
	{
		if (dynamic_cast<CUI*>(objs.back())->Get_isSceneChange())
		{
			if ((m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ACKBAR))))
			{
				MSG_BOX("Failed to Open Level JUGGLAS");
				return;
			}
		}
	}*/

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pGameInstance->Set_MainCamera(CAM_CUTSCENE);
		dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_MainCamera())->Play_CutScene();
	}


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
#endif // _DEBUG

//#endif
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Ready_Lights()
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


	//Mantari Ambient Light
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(-1000.f, -1000.f, -1000.f, 1.f);
	LightDesc.fRange = 15.f;
	LightDesc.vDiffuse = _float4(138.f / 255.f, 138.f / 255.f, 138.f / 255.f, 1.f);
	LightDesc.vAmbient = _float4(205.f / 255.f, 221.f / 255.f, 140.f / 255.f, 1.f);
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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
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
	 pTPCDesc.pPlayerTrans = dynamic_cast<CTransform*>( m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ThirdPersonCamera"), &pTPCDesc)))
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

	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_CutSceneCamera"), &pCSCdesc)))
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
	pSVCDesc.pPlayerTrans = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_SideViewCamera"), &pSVCDesc)))
		return E_FAIL;

	m_pGameInstance->Set_MainCamera(CAM_THIRDPERSON);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Rect"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
	//	return E_FAIL;

	//for (size_t i = 0; i < 60; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Explosion"))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	//for (int i = 0; i < 40; i++)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
			return E_FAIL;
	}


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	CLandObject::LANDOBJ_DESC landObjDesc;
	landObjDesc.mWorldMatrix._41 = 140.f;
	landObjDesc.mWorldMatrix._42 = 450.f;
	landObjDesc.mWorldMatrix._43 = 100.f;
	landObjDesc.mWorldMatrix._44 = 1.f;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_FallPlatform"), &landObjDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;




	//landObjDesc.mWorldMatrix._41 = 75.f;
	//landObjDesc.mWorldMatrix._42 = 523.f;
	//landObjDesc.mWorldMatrix._43 = 98.f;
	//landObjDesc.mWorldMatrix._44 = 1.f;


	//CHoverboard::HoverboardInfo hoverboardInfo;
	//hoverboardInfo.vPosition = _float3(75.f, 553.f, 98.f);
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Vehicle"), TEXT("Prototype_GameObject_HoverBoard"), &hoverboardInfo)))
	//	return E_FAIL;



	CSavePoint::_tagSavePoint_Desc savePointDesc;
	savePointDesc.vPosition = _float3(66.899f, 521.f, 97.272f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_SavePoint"), TEXT("Prototype_GameObject_SavePoint"),&savePointDesc)))
		return E_FAIL;
	
	//savePointDesc.vPosition = _float3(93.f, 521.f, 98.f);
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_SavePoint"), TEXT("Prototype_GameObject_SavePoint"), &savePointDesc)))
	//	return E_FAIL;




	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag)
{
	CLandObject::LANDOBJ_DESC landObjDesc;
	landObjDesc.mWorldMatrix._41 = 75.f;
	landObjDesc.mWorldMatrix._42 = 523.f;
	landObjDesc.mWorldMatrix._43 = 98.f;
	landObjDesc.mWorldMatrix._44 = 1.f;

	

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &landObjDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), pLandObjDesc)))
	//		return E_FAIL;
	//}0

	// 테스트 위해 임의로 생성
	/*CMonster::MST_DESC* pDesc = static_cast<CMonster::MST_DESC*>(pLandObjDesc);

	pDesc->eLevel = LEVEL_GAMEPLAY;*/

	// Prototype_GameObject_Boss_Juggulus   Prototype_GameObject_Ghost    Prototype_GameObject_Legionnaire_Gun

	//pLandObjDesc->mWorldMatrix._41 = 160.f;
	//pLandObjDesc->mWorldMatrix._42 = 528.f;
	//pLandObjDesc->mWorldMatrix._43 = 98.f;
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Juggulus"), pLandObjDesc)))
	//	return E_FAIL;
	
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Legionnaire_Gun"), pLandObjDesc)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Ghost"), pLandObjDesc)))
	//	return E_FAIL;
	

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Homonculus"), pLandObjDesc)))
	//	return E_FAIL;
	

	CLandObject::LANDOBJ_DESC landObjDesc;
	landObjDesc.mWorldMatrix._41 = 165.712f;
	landObjDesc.mWorldMatrix._42 = 528.f;
	landObjDesc.mWorldMatrix._43 = 97.312f;
	landObjDesc.mWorldMatrix._44 = 1.f;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Mantari"), &landObjDesc)))
		return E_FAIL;

	_float4 GrassPos = { landObjDesc.mWorldMatrix._41,landObjDesc.mWorldMatrix._42 - 5.f, landObjDesc.mWorldMatrix._43, 1.f };
	EFFECTMGR->Generate_Particle(40, GrassPos);

	//// Npc Valnir
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Valnir"), &landObjDesc)))
	//	return E_FAIL;

	//// Npc Choron
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Choron"), &landObjDesc)))
	//	return E_FAIL;

	//// Npc Yaak
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Npc"), TEXT("Prototype_GameObject_Npc_Yaak"), &landObjDesc)))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_LevelData(const _tchar* pFilePath)
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

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
				return E_FAIL;
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

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trap"), TEXT("Prototype_GameObject_Trap"), &pTrapDesc)))
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
				{
					// 다른 객체들은 개별적으로 생성
					CMap_Element::MAP_ELEMENT_DESC pDesc{};


					pDesc.mWorldMatrix = WorldMatrix;
					pDesc.wstrModelName = wszModelName;
					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wszLayer, wszName, &pDesc)))
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
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"),
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

HRESULT CLevel_GamePlay::Load_Data_Effects()
{
	const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_Plain_Effects.bin";
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

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wsLayer.c_str(), wsName.c_str(), &FireDesc)))
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

		//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wsLayer.c_str(), wsName.c_str(), &GrassDesc)))
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

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Vegetation"), TEXT("Prototype_GameObject_Tree"), &TreeDesc)))
			return E_FAIL;
	}

	for (auto& group : treeGroups)
	{
		for (auto& pMatrix : group.second)
		{
			delete pMatrix;
		}
	}

#ifdef _DEBUG
	//MSG_BOX("Effect Data Load");
#endif

	return S_OK;
}


HRESULT CLevel_GamePlay::Load_Data_Decals()
{
	const wchar_t* wszFileName = L"../Bin/MapData/DecalsData/Stage_Tutorial_Decals.bin";
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

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_Decal"), &pDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);


#ifdef _DEBUG
	//MSG_BOX("Decal Data Loaded");
#endif
	return S_OK;
}

void CLevel_GamePlay::Load_Lights()
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

	wstring LightsDataPath = L"../Bin/MapData/LightsData/Tutorial_Lights.dat";

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

HRESULT CLevel_GamePlay::ReLoad_Monster(const _tchar* pFilePath)
{
	//CLandObject::LANDOBJ_DESC landObjDesc;
	//landObjDesc.mWorldMatrix._41 = 167.f;
	//landObjDesc.mWorldMatrix._42 = 528.f;
	//landObjDesc.mWorldMatrix._43 = 98.f;
	//landObjDesc.mWorldMatrix._44 = 1.f;
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Mantari"), &landObjDesc)))
	//	return E_FAIL;

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

	// 생성된 객체 로드
	while (true)
	{
		ZeroMemory(wszName, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszLayer, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszModelName, sizeof(_tchar) * MAX_PATH);

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

		MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
		MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
		MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

		if (0 == dwByte)
			break;


			if (wstring(wszLayer) == TEXT("Layer_Monster"))
			{
				CLandObject::LANDOBJ_DESC pDesc{};
				pDesc.mWorldMatrix = WorldMatrix;

				if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), wszLayer, wszName, &pDesc)))
					return E_FAIL;
			}
		}

	return S_OK;
}

void CLevel_GamePlay::Set_Volume()
{
	m_pGameInstance->SetChannelVolume(SOUND_MONSTER05, 0.5f);
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Manager);
	

}

#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Default_Camera.h"
#include "ThirdPersonCamera.h"
#include "SideViewCamera.h"

#include "Imgui_Manager.h"
#include "ToolObj_Manager.h"
#include "Terrain.h"

#include "NaviMgr.h"

#include "Light.h"
#include "Light_Manager.h"

#include "VIBuffer_Instance_Point.h"

#include "Grass.h"
#include "Tree.h"

#include "FireEffect.h"
#include "TransitionCamera.h"
#include "Trap.h"
#include "Decal.h"
#include "BackGround_Card.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{pDevice, pContext}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    CRenderer::FOG_DESC fogDesc{};
    fogDesc.vFogColor = { 196.f / 255.f, 233.f / 255.f, 255.f / 255.f, 1.f };
    fogDesc.vFogColor2 = { 94.f / 255.f, 160.f / 255.f, 255.f / 255.f, 1.f };
    fogDesc.fFogRange = 7788.5;
    fogDesc.fFogHeightFalloff = 0.0f;
    fogDesc.fFogGlobalDensity = 1.0f;
    fogDesc.fFogTimeOffset = 1.154f;
    fogDesc.fFogTimeOffset2 = 3.462f;
    fogDesc.fNoiseIntensity = 1.731f;
    fogDesc.fNoiseIntensity2 = 1.923f;
    fogDesc.fNoiseSize = 0.000481f;
    fogDesc.fNoiseSize2 = 0.000481f;
    fogDesc.fFogBlendFactor = 0.284f;
    m_pGameInstance->Set_FogOption(fogDesc);


    if (FAILED(Ready_Light()))
        return E_FAIL;


    
    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if(FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
        return E_FAIL;

    if (FAILED(Ready_LandObjects()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_EnvEffects(TEXT("Layer_EnvEffects"))))
        return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
    //// Imgui의 Save 버튼이 눌린 경우 Save_Data( ) 함수 호출
    if (CImgui_Manager::GetInstance()->Get_IsSave())
    {
        Save_Data();
       // CNaviMgr::GetInstance()->Save_NaviData(); // Navi Save( ) 함수 호출
    }     



    if (CImgui_Manager::GetInstance()->Get_IsLoad())
        Load_Data();

    if (CImgui_Manager::GetInstance()->Get_IsPhysXSave())
    {
        Save_Data_PhysX();
    }

    if (CImgui_Manager::GetInstance()->Get_IsPhysXLoad())
    {
        Load_Data_PhysX();
    }

    if (CImgui_Manager::GetInstance()->Get_IsEffectsSave())
    {
        Save_Data_Effects();
    }

    if (CImgui_Manager::GetInstance()->Get_IsEffectsLoad())
    {
        Load_Data_Effects();
    }

    if (CImgui_Manager::GetInstance()->Get_IsDecalSave())
    {
        Save_Data_Decals();
    }

    if (CImgui_Manager::GetInstance()->Get_IsDecalLoad())
    {
        Load_Data_Decals();
    }

    if (CImgui_Manager::GetInstance()->Get_IsDecalSave())
    {
        Save_Data_Decals();
    }

    if (CImgui_Manager::GetInstance()->Get_IsDecalLoad())
    {
        Load_Data_Decals();
    }


    if (m_pGameInstance->Key_Down(DIK_1))
    {
        m_pGameInstance->Set_MainCamera(1);
    }
    if (m_pGameInstance->Key_Down(DIK_2))
    {
        m_pGameInstance->Set_MainCamera(2);
    }

    if (m_pGameInstance->Key_Down(DIK_4))
    {
        CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

        //pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
        //pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

        pTCDesc.fFovy = XMConvertToRadians(60.f);
        pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
        pTCDesc.fNear = 0.1f;
        pTCDesc.fFar = 3000.f;

        pTCDesc.fSpeedPerSec = 40.f;
        pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

        if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
        {
            MSG_BOX("FAILED");
        }
        return;
    }


#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Level : GamePlay"));
#endif
}

HRESULT CLevel_GamePlay::Ready_Light()
{
    LIGHT_DESC			LightDesc{};

    //LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    //LightDesc.vDirection = _float4{ -0.6f, -1.f, 0.8f, 0.f };
    //LightDesc.vDiffuse = _float4(0.95f, 0.97f, 0.97f, 1.f);
    ////LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    //LightDesc.vAmbient = _float4(0.93f * 0.8f, 0.98f * 0.8f, 0.97f * 0.8f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);


    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    //LightDesc.fRange = 20.f;
    //LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    ////LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
    //LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    ////LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);

    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(100.f, 50.f, 100.f, 1.f);
    //LightDesc.fRange = 10.f;
    //LightDesc.vDiffuse = _float4(0.8f, 1.f, 0.8f, 1.f);
    //LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);


    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(259.85f, 53.45f, 186.49f, 1.0f);
    //LightDesc.fRange = 20.f;
    //LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
    //LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);

    CImgui_Manager::GetInstance()->Load_Lights();

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const wstring& strLayerTag)
{
    if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain")))) // , &pDesc
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;


    CBackGround_Card::CARD_DESC desc{};
    desc.mWorldMatrix = {};

    _matrix vMat = { 1427.475f, 0.f, -2009.57, 0.f,
    0.f, 1000.f, 0.f, 0.f,
    815.468f, 0.f, 579.258f, 0.f,
    -3342.f, 695.f, -1722.f, 1.f };

    XMStoreFloat4x4(&desc.mWorldMatrix, vMat);
    desc.iTexNum = 0;
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &desc)))
        return E_FAIL;

    ZeroMemory(&desc, sizeof(desc));
    vMat = { -1977.f, 0.f, -3478.f, 0.f,
   0.f, 2000.f, 0.f, 0.f,
   1739.f, 0.f, -988.f, 0.f,
  -3304.f, 1157.f, 2678.f, 1.f };
    XMStoreFloat4x4(&desc.mWorldMatrix, vMat);
    desc.iTexNum = 1;
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &desc)))
        return E_FAIL;

    desc.mWorldMatrix = {};
     vMat = { 1143.863f, 0.f, 2773.897f, 0.f,
        0.f, 1500.f, 0.f, 0.f,
        -1386.976f, 0.f, 571.943f, 0.f,
        3609.f, 783.378f, -706.441f, 1.f };
    XMStoreFloat4x4(&desc.mWorldMatrix, vMat);
    desc.iTexNum = 2;
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &desc)))
        return E_FAIL;

    ZeroMemory(&desc, sizeof(desc));
    vMat = { 3495.291f, 0.f, -188.991f, 0.f,
        0.f, 2000.f, 0.f, 0.f,
        161.997f, 0.f, 2996.f, 0.f,
        1349.754f, 1881.764f, 4170.497f, 1.f };
    XMStoreFloat4x4(&desc.mWorldMatrix, vMat);
    desc.iTexNum = 0;
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &desc)))
        return E_FAIL;

    //CGrass::GRASS_DESC desc{};
    //if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Grass"), &desc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CDefault_Camera::DEFAULTCAM_DESC pDesc = {};

    pDesc.fSensor = 0.1f;

    pDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    pDesc.fFovy = XMConvertToRadians(60.f);
    pDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pDesc.fNear = 0.1f;
    pDesc.fFar = 3000.f;

    pDesc.fSpeedPerSec = 40.f;
    pDesc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_DefaultCamera"), &pDesc)))
        return E_FAIL;

    //pDesc.fSensor = 0.1f;

    //pDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    //pDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    //pDesc.fFovy = XMConvertToRadians(60.f);
    //pDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    //pDesc.fNear = 0.1f;
    //pDesc.fFar = 3000.f;

    //pDesc.fSpeedPerSec = 40.f;
    //pDesc.fRotationPerSec = XMConvertToRadians(90.f);

    //if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_DefaultCamera"), &pDesc)))
    //    return E_FAIL;


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




    CSideViewCamera::SIDEVIEWCAMERA_DESC pSVDesc = {};

    pSVDesc.fSensor = 0.1f;

    pSVDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pSVDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    pSVDesc.fFovy = XMConvertToRadians(60.f);
    pSVDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pSVDesc.fNear = 0.1f;
    pSVDesc.fFar = 3000.f;

    pSVDesc.fSpeedPerSec = 40.f;
    pSVDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pSVDesc.pPlayerTrans = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
    if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_SideViewCamera"), &pSVDesc)))
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


    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
 /*   if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
        return E_FAIL;*/




    //if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(0, 0, XMVectorSet(0.f,0.f,0.f, 1.f))))
    //    return E_FAIL;



    //if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(0, 0, XMVectorSet(5.f, 3.f, 5.f, 1.f))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_EnvEffects(const wstring& strLayerTag)
{
   


    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), pLandObjDesc)))
        return E_FAIL;


    return S_OK;
}


HRESULT CLevel_GamePlay::Ready_LandObjects()
{
    CLandObject::LANDOBJ_DESC		LandObjDesc{};

    LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(CGameInstance::GetInstance()->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
    LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(CGameInstance::GetInstance()->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjDesc)))
        return E_FAIL;

    //if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Save_Data()
{
    const wchar_t* wszFileName[MAX_PATH] = { Setting_FileName() };
    HANDLE	hFile = CreateFile(*wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    _char   szName[MAX_PATH] = "";
    _char   szLayer[MAX_PATH] = "";
    _char   szModelName[MAX_PATH] = "";
    _float4x4 WorldMatrix;
    CModel::MODELTYPE eModelType;
    //_uint   iVerticesX = { 0 };
    //_uint   iVerticesZ = { 0 };
    DWORD   dwByte(0);
    _uint iTriggerType;
    _double dStartTime = 0;

    // 생성된 Tool Obj들 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        strcpy_s(szName, iter->Get_Name());
        XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
       
        //이펙트 추가할 때마다 여기추가
        //이펙트 추가할 때마다 여기추가
        //이펙트 추가할 때마다 여기추가
    if (strcmp(szName, "Prototype_GameObject_Fire_Effect") == 0) //이펙트 추가할 때마다 여기추가
    {
        continue;
    }

        WriteFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr); // sizeof(_char) * MAX_PATH

        if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
        {
            iTriggerType =iter->Get_TriggerType();
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
        }
        else if (strcmp(szName, "Prototype_GameObject_Trap") == 0)
        {
            strcpy_s(szModelName, iter->Get_ModelName());
            eModelType = iter->Get_ModelType();
            strcpy_s(szLayer, iter->Get_Layer());
            iTriggerType = iter->Get_TriggerType();
            dStartTime = dynamic_cast<CTrap*>(iter)->Get_StartTime();

            WriteFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
            WriteFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &dStartTime, sizeof(_double), &dwByte, nullptr);
        }
        else if (strcmp(szName, "Prototype_GameObject_TreasureChest") == 0)
        {
            strcpy_s(szModelName, iter->Get_ModelName());
            eModelType = iter->Get_ModelType();
            strcpy_s(szLayer, iter->Get_Layer());
            iTriggerType = iter->Get_TriggerType();

            WriteFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
            WriteFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
        }
        else
        {
            strcpy_s(szModelName, iter->Get_ModelName());
            eModelType = iter->Get_ModelType();
            strcpy_s(szLayer, iter->Get_Layer());

            WriteFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
        }
      
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontSave();

    MSG_BOX("Data Save");

    return S_OK;
}


HRESULT CLevel_GamePlay::Save_Data_PhysX()
{
    const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_GrassLand_PhysX.bin";
    HANDLE hFile = CreateFile(wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
        return E_FAIL;

    DWORD dwByte(0);
    map<std::string, bool> processedModels;

    // 먼저 모든 고유한 모델을 처리하여 map의 크기를 결정
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (nullptr != iter->Get_ModelName() && strcmp(iter->Get_Layer(), "Layer_Monster") != 0)
        {
            string modelName = iter->Get_ModelName();
            if (processedModels.find(modelName) == processedModels.end())
            {
                processedModels[modelName] = true;
            }
        }
    }

    // map의 크기(고유한 모델의 수)를 파일에 쓰기
    size_t mapSize = processedModels.size();
    WriteFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

    // map을 초기화하고 다시 채우면서 모델 정보 저장
    processedModels.clear();

    // 생성된 Tool Obj들 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (nullptr != iter->Get_ModelName() && strcmp(iter->Get_Layer(), "Layer_Monster") != 0)
        {
            string modelName = iter->Get_ModelName();
            string modelPath = iter->Get_ModelPath();
            if (processedModels.find(modelName) == processedModels.end())
            {
                processedModels[modelName] = true;
                // "Model_"을 "PhysX_"로 대체하는 부분은 그대로 유지
                size_t pos = modelName.find("Model_");
                if (pos != string::npos)
                {
                    modelName.replace(pos, 6, "PhysX_");
                }
                char szModelName[MAX_PATH] = "";
                strcpy_s(szModelName, modelName.c_str());
                WriteFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
                WriteFile(hFile, modelPath.c_str(), sizeof(char) * MAX_PATH, &dwByte, nullptr);
            }
        }
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontPhysXSave();
    MSG_BOX("Data Save");
    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data_PhysX()
{
    const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_GrassLand_PhysX.bin";
    HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    DWORD dwByte(0);

    // 먼저 map의 크기(고유한 모델의 수)를 읽습니다.
    size_t mapSize = 0;
    ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);
    if (0 == dwByte)
    {
        CloseHandle(hFile);
        return S_OK; // 파일이 비어있는 경우
    }

    // 각 모델에 대한 정보를 읽습니다.
    for (size_t i = 0; i < mapSize; ++i)
    {
        char szModelName[MAX_PATH] = "";
        char szModelPath[MAX_PATH] = "";

        ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
        if (0 == dwByte)
            break;

        ReadFile(hFile, szModelPath, sizeof(char) * MAX_PATH, &dwByte, nullptr);
        if (0 == dwByte)
            break;

        // 여기서 읽어들인 모델 이름과 경로를 사용하여 필요한 작업을 수행합니다.
        // 예: PhysX 콜라이더 생성, 모델 로드 등

        // 예시:
        // Create_PhysXCollider(szModelName, szModelPath);

        // 디버그 출력 (필요시 사용)
        // OutputDebugStringA(("Loaded Model: " + std::string(szModelName) + "\n").c_str());
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontPhysXLoad();

    MSG_BOX("PhysX Data Loaded");
    return S_OK;
}

HRESULT CLevel_GamePlay::Save_Data_Effects()
{
    const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_GrassLand_Effects.bin";
    HANDLE hFile = CreateFile(wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
        return E_FAIL;

    DWORD dwByte(0);
    _uint iEffectCount = 0;

    vector<_float4x4> grassWorldMats;


    // 먼저 이펙트 개수를 세고 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (strcmp(iter->Get_Name(), "Prototype_GameObject_Fire_Effect") == 0 || 
            strcmp(iter->Get_Name(), "Prototype_GameObject_Grass") == 0 ||
            strcmp(iter->Get_Name(), "Prototype_GameObject_Tree") == 0) //이펙트 추가할 때마다 여기에 조건 추가
        {
            iEffectCount++;
        }
    }
    WriteFile(hFile, &iEffectCount, sizeof(_uint), &dwByte, nullptr);

    // 이펙트 정보 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (strcmp(iter->Get_Name(), "Prototype_GameObject_Fire_Effect") == 0) //이펙트 추가할 때마다 여기에 조건 추가
        {
            char szName[MAX_PATH] = "";
            char szModelName[MAX_PATH] = "";
            char szLayer[MAX_PATH] = "";
            _float4x4 WorldMatrix;

            strcpy_s(szName, iter->Get_Name());
            strcpy_s(szLayer, iter->Get_Layer());
            XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());

            WriteFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
        }
        if (strcmp(iter->Get_Name(), "Prototype_GameObject_Grass") == 0)
        {
            char szName[MAX_PATH] = "";
            char szModelName[MAX_PATH] = "";
            char szLayer[MAX_PATH] = "";
            _float4x4 WorldMatrix;
            _float3 TopCol, BotCol;
            strcpy_s(szName, iter->Get_Name());
            strcpy_s(szModelName, iter->Get_ModelName());
            strcpy_s(szLayer, iter->Get_Layer());
            XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
            vector<_float4x4*> vtxMatrices = static_cast<CGrass*>(iter)->Get_VtxMatrices();
            TopCol = static_cast<CGrass*>(iter)->Get_TopCol();
            BotCol = static_cast<CGrass*>(iter)->Get_BotCol();
            WriteFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &TopCol, sizeof(_float3), &dwByte, nullptr);
            WriteFile(hFile, &BotCol, sizeof(_float3), &dwByte, nullptr);
            // 정점 매트릭스 개수 저장
            _uint iVtxMatrixCount = vtxMatrices.size();
            WriteFile(hFile, &iVtxMatrixCount, sizeof(_uint), &dwByte, nullptr);
            // 정점 매트릭스 저장
            for (_uint i = 0; i < iVtxMatrixCount; ++i)
            {
                WriteFile(hFile, vtxMatrices[i], sizeof(_float4x4), &dwByte, nullptr);
            }
            // 메모리 해제
            for (auto& mat : vtxMatrices)
            {
                delete mat;
            }
        }
        else if (strcmp(iter->Get_Name(), "Prototype_GameObject_Tree") == 0)
        {

            char szName[MAX_PATH] = "";
            char szModelName[MAX_PATH] = "";
            char szLayer[MAX_PATH] = "";
            _float4x4 WorldMatrix;
            _float3 LeafCol;
            _bool isBloom;

            strcpy_s(szName, iter->Get_Name());
            strcpy_s(szModelName, iter->Get_ModelName());
            strcpy_s(szLayer, iter->Get_Layer());
            XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
            LeafCol = static_cast<CTree*>(iter)->Get_LeafCol();
            isBloom = static_cast<CTree*>(iter)->Get_Bloom();

            WriteFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &LeafCol, sizeof(_float3), &dwByte, nullptr);
            WriteFile(hFile, &isBloom, sizeof(_bool), &dwByte, nullptr);

        }
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontEffectsSave();
    MSG_BOX("Data Save");
    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data_Effects()
{
    const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_GrassLand_Effects.bin";
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
        char szModelName[MAX_PATH] = "";
        char szLayer[MAX_PATH] = "";
        _float4x4 WorldMatrix;

        ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
        ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
        ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
        ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

        // char 배열을 wstring으로 변환
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

        // 이펙트 생성 및 설정
        if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Fire_Effect") == 0)
        {
            CFireEffect::FIREEFFECTDESC FireDesc{};
            FireDesc.vStartPos = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };
            FireDesc.vStartScale = { 1.f, 1.f }; // 스케일은 필요에 따라 조정
            FireDesc.mWorldMatrix = WorldMatrix;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wsLayer.c_str(), wsName.c_str(), &FireDesc)))
                return E_FAIL;
        }
        else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Grass") == 0)
        {
            CGrass::GRASS_DESC GrassDesc{};
            GrassDesc.mWorldMatrix = WorldMatrix;

            // 추가 정보 읽기
            _float3 TopCol, BotCol;
            ReadFile(hFile, &TopCol, sizeof(_float3), &dwByte, nullptr);
            ReadFile(hFile, &BotCol, sizeof(_float3), &dwByte, nullptr);

            GrassDesc.vTopCol = TopCol;
            GrassDesc.vBotCol = BotCol;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wsLayer.c_str(), wsName.c_str(), &GrassDesc)))
                return E_FAIL;
        }
        else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Tree") == 0)
        {
            CTree::TREE_DESC TreeDesc{};
            TreeDesc.mWorldMatrix = WorldMatrix;
            strcpy_s(TreeDesc.szModelName, szModelName);

            // 추가 정보 읽기
            _float3 LeafCol;
            _bool isBloom;
            ReadFile(hFile, &LeafCol, sizeof(_float3), &dwByte, nullptr);
            ReadFile(hFile, &isBloom, sizeof(_bool), &dwByte, nullptr);

            TreeDesc.vLeafCol = LeafCol;
            TreeDesc.isBloom = isBloom;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, wsLayer.c_str(), wsName.c_str(), &TreeDesc)))
                return E_FAIL;
        }
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontEffectsLoad();

    MSG_BOX("Data Load");
    return S_OK;
}

HRESULT CLevel_GamePlay::Save_Data_Decals()
{
    const wchar_t* wszFileName = L"../Bin/MapData/DecalsData/Stage_GrassLand_Decals.bin";
    HANDLE hFile = CreateFile(wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
        return E_FAIL;

    DWORD dwByte(0);
    _uint iDecalCount = 0;

    // 먼저 이펙트 개수를 세고 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (strcmp(iter->Get_Name(), "Prototype_GameObject_Decal") == 0) //이펙트 추가할 때마다 여기에 조건 추가
        {
            iDecalCount++;
        }
    }
    WriteFile(hFile, &iDecalCount, sizeof(_uint), &dwByte, nullptr);

    // 데칼 정보 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (strcmp(iter->Get_Name(), "Prototype_GameObject_Decal") == 0) //이펙트 추가할 때마다 여기에 조건 추가
        {
            char szName[MAX_PATH] = "";
            char szLayer[MAX_PATH] = "";
            _float4x4 WorldMatrix;
            _uint     decalIdx = 0;

            strcpy_s(szName, "Prototype_GameObject_Decal");
            strcpy_s(szLayer, iter->Get_Layer());
            XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
            decalIdx = dynamic_cast<CDecal*>(iter)->Get_DecalIdx();

            WriteFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &decalIdx, sizeof(_uint), &dwByte, nullptr);
        }
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontDecalSave();
    MSG_BOX("Data Save");

    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data_Decals()
{
    const wchar_t* wszFileName = L"../Bin/MapData/DecalsData/Stage_GrassLand_Decals.bin";
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

        strcpy_s(pDesc.szObjName, szName);
        strcpy_s(pDesc.szLayer, szLayer);
        pDesc.mWorldMatrix = WorldMatrix;
        pDesc.iDecalIdx = decalIdx;

        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_Decal"), &pDesc)))
            return E_FAIL;
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontDecalLoad();
    MSG_BOX("Decal Data Loaded");
    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data()
{
    HANDLE	hFile = CreateFile(Setting_FileName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    _char   szName[MAX_PATH] = "";
    _char   szLayer[MAX_PATH] = "";
    _char   szModelName[MAX_PATH] = "";
    _float4x4 WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    CModel::MODELTYPE eModelType = CModel::TYPE_END;
    _uint   iVerticesX = { 0 };
    _uint   iVerticesZ = { 0 };
    DWORD   dwByte(0);

    _uint   iTriggerType;
    _double dStartTime = 0;

    //ReadFile(hFile, &iVerticesX, sizeof(_uint), &dwByte, nullptr);
    //ReadFile(hFile, &iVerticesZ, sizeof(_uint), &dwByte, nullptr);

    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object_InLayer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"))->back());

    // pDesc 넣어줘야함
   /* CTerrain::TERRAIN_DESC pDesc{};
    pDesc.iVerticesX = iVerticesX;
    pDesc.iVerticesZ = iVerticesZ;*/

    //pTerrain->Setting_NewTerrain(&pDesc);
    //pTerrain->Setting_LoadTerrain(&pDesc);
    
    while (true)
    {
        ReadFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

        if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
        {
            ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);


            if (0 == dwByte)
                break;

            CToolObj::TOOLOBJ_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
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

            CTrap::TRAP_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
            strcpy_s(pDesc.szLayer, szLayer);
            strcpy_s(pDesc.szModelName, szModelName);
            pDesc.mWorldMatrix = WorldMatrix;
            pDesc.eModelType = eModelType;
            pDesc.TriggerType = iTriggerType;
            pDesc.dStartTimeOffset = dStartTime;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trap"), TEXT("Prototype_GameObject_Trap"), &pDesc)))
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

            CToolObj::TOOLOBJ_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
            strcpy_s(pDesc.szLayer, szLayer);
            strcpy_s(pDesc.szModelName, szModelName);
            pDesc.mWorldMatrix = WorldMatrix;
            pDesc.eModelType = eModelType;
            pDesc.TriggerType = iTriggerType;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_TreasureChest"), TEXT("Prototype_GameObject_TreasureChest"), &pDesc)))
                return E_FAIL;
        }
        else 
        {
            ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

            if (0 == dwByte)
                break;

            CToolObj::TOOLOBJ_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
            strcpy_s(pDesc.szLayer, szLayer);
            strcpy_s(pDesc.szModelName, szModelName);
            pDesc.mWorldMatrix = WorldMatrix;
            pDesc.eModelType = eModelType;
            if (strcmp(szName, "Prototype_GameObject_BackGround_Card") == 0)
            {
                continue;
            }
            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_ToolObj"), &pDesc)))
                return E_FAIL;
        }
      
    
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontLoad();
    MSG_BOX("Data Load");

    return S_OK;
}



const _tchar* CLevel_GamePlay::Setting_FileName()
{
    _int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();

    switch (iStageIdx)
    {
    case STAGE_HOME:
        return  L"../Bin/MapData/Stage_Ackbar.dat";
    case STAGE_ONE:
        return L"../Bin/MapData/Stage_Tutorial.dat";
    case STAGE_TWO:
        return L"../Bin/MapData/Stage_AndrasArena.dat";
    case STAGE_THREE:
        return L"../Bin/MapData/Stage_Jugglas.dat";
    case STAGE_BOSS:
        return L"../Bin/MapData/test.dat";
    default:
        MSG_BOX("Setting File Name is Failed");
        return L"\0";
    }
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CLevel_GamePlay");
        return nullptr;
    }

    return pInstance;
}

void CLevel_GamePlay::Free()
{
    CToolObj_Manager::DestroyInstance();
    __super::Free();
}

#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Default_Camera.h"

#include "Imgui_Manager.h"
#include "ToolObj_Manager.h"
#include "Terrain.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{pDevice, pContext}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(Ready_Light()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    /*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;*/

    if(FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
        return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
    // Imgui의 Save 버튼이 눌린 경우 Save_Data( ) 함수 호출
    //if (CImgui_Manager::GetInstance()->Get_IsSave())
    //{
    //    Save_Data();
    //    CNaviMgr::GetInstance()->Save_NaviData(); // Navi Save( ) 함수 호출
    //}     

    //if (CImgui_Manager::GetInstance()->Get_IsLoad())
    //    Load_Data();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Level : GamePlay"));
#endif
}

HRESULT CLevel_GamePlay::Ready_Light()
{
    LIGHT_DESC      LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 1.f);
    LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
    LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    m_pGameInstance->Add_Light(LightDesc);

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const wstring& strLayerTag)
{
    if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain")))) // , &pDesc
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CDefault_Camera::DEFAULTCAM_DESC pDesc = {};

    pDesc.fSensor = 0.1f;

    pDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    pDesc.fFovy = XMConvertToRadians(60.f);
    //pDesc.fFov = 
    pDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pDesc.fNear = 0.1f;
    pDesc.fFar = 3000.f;

    pDesc.fSpeedPerSec = 40.f;
    pDesc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_DefaultCamera"), &pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
        return E_FAIL;

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
    _uint   iVerticesX = { 0 };
    _uint   iVerticesZ = { 0 };
    DWORD   dwByte(0);

    // Terrain 정보 저장
    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object_InLayer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"))->back());
    // 현재 object list의 size가 0으로 나옴

    //iVerticesX = pTerrain->Get_VerticesX();
    //iVerticesZ = pTerrain->Get_VerticesZ();

    WriteFile(hFile, &iVerticesX, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, &iVerticesZ, sizeof(_uint), &dwByte, nullptr);

    // 생성된 Tool Obj들 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        strcpy_s(szName, iter->Get_Name());
        strcpy_s(szLayer, iter->Get_Layer());
        strcpy_s(szModelName, iter->Get_ModelName());
        XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
        eModelType = iter->Get_ModelType();

        WriteFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr); // sizeof(_char) * MAX_PATH
        WriteFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        WriteFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
        WriteFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontSave();

    MSG_BOX("Data Save");

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

    ReadFile(hFile, &iVerticesX, sizeof(_uint), &dwByte, nullptr);
    ReadFile(hFile, &iVerticesZ, sizeof(_uint), &dwByte, nullptr);

    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object_InLayer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"))->back());

    // pDesc 넣어줘야함
    CTerrain::TERRAIN_DESC pDesc{};
    pDesc.iVerticesX = iVerticesX;
    pDesc.iVerticesZ = iVerticesZ;

    //pTerrain->Setting_NewTerrain(&pDesc);
    //pTerrain->Setting_LoadTerrain(&pDesc);
    
    while (true)
    {
        ReadFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
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

        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_ToolObj"), &pDesc)))
            return E_FAIL;
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontLoad();
    MSG_BOX("Data Load");

    return S_OK;
}

const _tchar* CLevel_GamePlay::Setting_FileName()
{
    //_int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();

    _int iFileIdx = CImgui_Manager::GetInstance()->Get_FileIdx();

    switch (iFileIdx)
    {
    case MODEL_PLAYAER:
        return  L"../Bin/Resources/Anim/Octopus.dat";
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
    __super::Free();

    CToolObj_Manager::DestroyInstance();
}

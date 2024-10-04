#pragma once
#include "..\Default\framework.h"
#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "..\Public\imgui_impl_win32.h"
#include "..\Public\imgui_impl_dx11.h"
#include <Windows.h>
#include <string.h>
#include "..\ImGuizmo\ImGuizmo.h"
#include "..\ImGuizmo\ImSequencer.h"

#include "ToolObj_Manager.h"
#include "PipeLine.h"
#include "NaviMgr.h"

#include "Light.h"
#include "Terrain.h"
#include "Renderer.h"

#include "ThirdPersonCamera.h"
#include "Cloud.h"
#include "Lagoon.h"

#include <DirectXTex.h>
#include <vector>
#include <list>
#include <d3d11.h>
#include <windows.h>

IMPLEMENT_SINGLETON(CImgui_Manager)

static ImGuizmo::OPERATION mGizmoOperation(ImGuizmo::TRANSLATE);

CImgui_Manager::CImgui_Manager() : m_bShowWindow(false), m_bShowAnotherWindow(false)
{
}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{


    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    m_iStageIdx = 0;
    m_iObjIdx = 0;

    m_pDecalTexs = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Decals_Textures/Decal%d.png"), 58);

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static XMFLOAT4 objColor;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
    {
        g_bIsHovered = true;

    }
    else
    {
        g_bIsHovered = false;
    }

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (m_bShowWindow) ImGui::ShowDemoWindow(&m_bShowWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float fTerrainX = 0.f;
        static float fTerrainZ = 0.f;
        static int counter = 0;

        ImGui::Begin("~Shattered~ MapTool");

        /*    ImGui::Spacing();
            ImGui::Separator();*/

        ImGui::TextColored({ 0.5f, 1.f, 1.f, 1.f }, "%.1f FPS", 1000.0f / io.Framerate, io.Framerate);
        _float4 pMousePos = { 0.f, 0.f, 0.f, 1.f };
        m_pGameInstance->Get_PickPos(&pMousePos);
        ImGui::Spacing();
        ImGui::TextColored({ 1.0f, 0.84f, 0.32f, 1.f }, "Mouse Pos");
        ImGui::Text("x: %.1f, y: %.1f, z: %.1f, w: %.1f", pMousePos.x, pMousePos.y, pMousePos.z, pMousePos.w);

        ImGui::Spacing();
        ImGui::TextColored({ 1.0f, 0.84f, 0.32f, 1.f }, "Pick Pos (RB to Update)");

        if (m_pGameInstance->Mouse_Down(DIM_RB))
        {
            m_ClickedMousePos = pMousePos;
        }

        ImGui::Text("x: %.1f, y: %.1f, z: %.1f, w: %.1f", m_ClickedMousePos.x, m_ClickedMousePos.y, m_ClickedMousePos.z, m_ClickedMousePos.w);

        ImGui::Separator();
        ImGui::ColorEdit4("Global ColorPicker", &m_GlobalColor.x);
        ImGui::Separator();   
        
        ImGui::Separator();
        ImGui::Separator();
        const char* skybox_items[] = { "SkyBox 0", "SkyBox 1", "SkyBox 2", "SkyBox 3", "SkyBox 4", "SkyBox 5", "SkyBox 6" };
        ImGui::Combo("SkyBox", &m_iSkyBoxIdx, skybox_items, IM_ARRAYSIZE(skybox_items));
        ImGui::Separator();
        ImGui::Separator();

        // Stage 선택(List Box)
        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Scene");
        const char* Stages[] = { "Ackbar", "Tutorial", "AndrasArena", "RasSamrahCastle", "Boss Stage" };
        static int Stage_current = 0;
        ImGui::ListBox("###Stage", &Stage_current, Stages, IM_ARRAYSIZE(Stages)); // item_current 변수에 선택 값 저장
        m_iStageIdx = Stage_current;

        if (ImGui::Button("Save Objects"))
            m_IsSave = true;
        ImGui::SameLine();
        if (ImGui::Button("Load Obejects"))
            m_IsLoad = true;
        ImGui::SameLine();
        ImGui::Separator();

        if (ImGui::Button("Save PhysX"))
            m_IsPhysXSave = true;
        ImGui::SameLine();

        if (ImGui::Button("Load PhysX"))
            m_IsPhysXLoad = true;

        ImGui::SameLine();

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Save Effects"))
            m_IsEffectsSave = true;
        ImGui::SameLine();

        if (ImGui::Button("Load Effects"))
            m_IsEffectsLoad = true;
        ImGui::SameLine();

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Save Decal"))
            m_IsDecalSave = true;
        ImGui::SameLine();

        if (ImGui::Button("Load Decal"))
            m_IsDecalLoad = true;
        ImGui::SameLine();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Add Objects");

        ImGui::Checkbox("ADD OBJECT", &m_bAddToolObj); // Navigation 모드 활성화 여부



        if (m_bAddToolObj && !ImGui::IsAnyItemActive() && !g_bIsHovered)
        {
            if (m_pGameInstance->Mouse_Down(DIM_LB))
            {
                //_vector vPickingPos = Check_Picking();
                _bool	isPick = { false };
                _float4 pPickPos = { 0.f, 0.f, 0.f, 1.f };
                if (m_pGameInstance->Get_PickPos(&pPickPos))
                {
                    Add_ToolObj(XMLoadFloat4(&pPickPos), XMLoadFloat4(&objColor)); // Obj 생성
                }


            }
        }

        // ImGui::Checkbox("Navigation Mode On", &m_IsNaviMode); // Navigation 모드 활성화 여부
        ImGui::SameLine();
        /*      if (ImGui::Button("Navigation Clear"))
                  m_IsNaviClear = true;*/

        ImGui::Spacing();


        // Layer 선택(Combo)
        ImGui::Checkbox("ANIM MODEL", &m_bIsAnimModel);
        ImGui::Checkbox("DECO OBJECT", &m_bIsDecoObject);

        const char* Layers[] = { "Monster", "Passive Element", "Active Element", "Trigger", "EnvEffects", "Traps", "Decals", "Vegetation"};
        static int Layer_current = 0;
        ImGui::Combo("Layer", &Layer_current, Layers, IM_ARRAYSIZE(Layers));
        m_iLayerIdx = Layer_current;



        Setting_ObjListBox(m_iLayerIdx); // ListBox 구성

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Editor");
        ImGui::Checkbox("Object Edit", &m_bShowAnotherWindow);
        ImGui::Spacing();
        ImGui::Checkbox("Light Edit", &m_bLightWindow);
        ImGui::Spacing();
        ImGui::Checkbox("Shadow Edit", &m_bShadowWindow);
        ImGui::Spacing();


        ImGui::Separator();
        ImGui::Checkbox("Battle Camera Edit", &m_bBattleCameraWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Cinematic Camera Edit", &m_bCameraWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Terrain Edit", &m_bTerrainWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Decal Edit", &m_bShowDecalTextureWindow);
        ImGui::Spacing();


        ImGui::Separator();
        ImGui::Checkbox("Global Wind Edit", &m_bGlobalWindWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Fog Edit", &m_bFogWindow);
        ImGui::Spacing(); 
        
        
        ImGui::Separator();
        ImGui::Checkbox("Cloud Edit", &m_bCloudWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Water Edit", &m_bWaterWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("HBAO Edit", &m_bHBAOWindow);
        ImGui::Spacing();
        ImGui::End();
    }

    if (m_bShowAnotherWindow)
    {
        m_bLightWindow = false;
        ImGui::Begin("Object Edit", &m_bShowAnotherWindow);
        //ImGui::SetWindowSize("Object State", ImVec2(350, 300));

        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Object Transform");

        Setting_CreateObj_ListBox();

        const _float4x4* worldMatrix = CToolObj_Manager::GetInstance()->Get_Obj_WorldMatrix(m_iCreateObjIdx);

        if (nullptr != worldMatrix)
        {
            EditTransform((_float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), // cameraView
                (_float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ),
                (_float*)worldMatrix,
                true);
        }
        ImGui::ColorEdit4("Obj Diffuse", &objColor.x);

        ImGui::Spacing();
        if (ImGui::Button("Delete Obj")) // 선택 객체 삭제
            Delete_Obj();
        ImGui::SameLine();
        if (ImGui::Button("Close"))
            m_bShowAnotherWindow = false;
        ImGui::End();
    }






    if (m_bLightWindow)
    {
        m_bShowAnotherWindow = false;
        Light_Editor();
    }

    if (m_bShadowWindow)
    {
        Shadow_Editor();
    }

    if (m_bCameraWindow)
    {
        Camera_Editor();
    }
    
    if (m_bBattleCameraWindow)
    {
        Battle_Camera_Editor();
    }

    if (m_bTerrainWindow)
    {
        Terrain_Editor();
    }

    if (m_bGlobalWindWindow)
    {
        GlobalWind_Editor();
    }

    if (m_bFogWindow) 
    {
        Fog_Editor();
    }

    if (m_bCloudWindow)
    {
        Cloud_Editor();
    }

    if (m_bWaterWindow)
    {
        Water_Editor();
    } 
    
    if (m_bHBAOWindow)
    {
        HBAO_Editor();
    }

    if (m_bShowDecalTextureWindow)
    {
        ImGui::Begin("Decal Texture Preview", &m_bShowDecalTextureWindow);

        if (m_iLayerIdx == LAYER_DECAL && m_iObjIdx >= 0 && m_iObjIdx < 58)
        {
            ImGui::Text("Selected Decal: Decal %d", m_iObjIdx);
            ImGui::Separator();

            if (ImGui::ImageButton(DirectXTextureToImTextureID(m_iObjIdx), ImVec2(300.f, 300.f), ImVec2(0.f, 0.f)))
            {
            }
        }
        else
        {
            ImGui::Text("Please select a decal from the list.");
        }

        ImGui::End();
    }

    ImGui::EndFrame();

}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    // editTransformDecomposition
    if (true)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mGizmoOperation == ImGuizmo::TRANSLATE))
            mGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mGizmoOperation == ImGuizmo::ROTATE))
            mGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mGizmoOperation == ImGuizmo::SCALE))
            mGizmoOperation = ImGuizmo::SCALE;
        /*if (ImGui::RadioButton("Universal", mGizmoOperation == ImGuizmo::UNIVERSAL))
            mGizmoOperation = ImGuizmo::UNIVERSAL;*/
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
            useSnap = !useSnap;
        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (mGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    if (useWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
        ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        //ImGuiWindow* window = ImGui::GetCurrentWindow();
        //gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
    }
    else
    {
        // 이거만 신경쓰기 얼마만큼 기즈모 화면값
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    }
    //격자
    //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
    // 큐브 그려주는애
    //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
    // 객체 잡고 동그라미좌표계 그려주는애
    // 객체 잡고 동그라미좌표계 그려주는애
    // matrix : 객체월드행ㄹ려
    ImGuizmo::Manipulate(cameraView, cameraProjection, mGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    // Gizmo 조작 후 업데이트된 행렬 표시
    ImGui::Text("Updated World Matrix:");
    for (int i = 0; i < 4; i++)
    {
        ImGui::Text("%.3f %.3f %.3f %.3f",
            matrix[i * 4 + 0], matrix[i * 4 + 1],
            matrix[i * 4 + 2], matrix[i * 4 + 3]);
    }

    // 화면 상단 카메라 돌아가는애
    //ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    if (useWindow)
    {
        ImGui::End();
        // 이거해보기 터지면
        ImGui::PopStyleColor(1);
    }
}

void CImgui_Manager::Setting_ObjListBox(_int iLayerIdx)
{
    // 배치할 Obj 선택(List Box)
    //ImGui::Text("Select Obj");

    static int item_current = 0;

    switch (iLayerIdx)
    {
    case LAYER_MONSTER:
    {
        const char* items_Monster[] = { "test", "MANTARI", "Legionnaire_Gun", "Ghost", "Homonculus", "Boss Juggulus", 
           "Chaudron", "SkeletalChain", "Boss Statue", "Boss Malkhel",  "Boss Andras", "Yaak"

        };
        ImGui::ListBox("###Obj", &item_current, items_Monster, IM_ARRAYSIZE(items_Monster)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_PASSIVE_ELEMENT:
    {
        const char* items_MapElement[] = { "BasicCube", "BasicDonut",  "BasicGround", "TronesT03","AndrasTEST","AndrasArenaRocks","Grasses_TT",
            "RasSamrahCastle",
            "RasSamrahCastle2",
            "RasSamrahCastle3",
            "RasSamrahCastle4",
            "RasSamrahCastle5",
            
            //TUTORIAL
            "TutorialMap", "TutorialDecoStructure", "TutorialDecoCube", "TutorialDecoMaze",

            //STAGE 1
            "WellArea", "MergedHouses23", "MergedHouses65", "AckbarCastle",
            "AqueducTower", "BigStairSides", "WoodStair", "_WoodPlatform", "WoodPlank", "RichStairs_Rambarde", "BigRocks", "BigRocks4",
            "Rock1",    "Rock2",    "Rock3",    "Rock4",


            //STAGE 1 DECOS
            "BoxA", "BoxB",
            "YantraStatue",
            "BannerPole1", "BannerPole2", "BannerPole3", "BannerPole4","BannerPole5",

            "AckbarWell", "Remparts_CrenauxA", "Remparts_CrenauxB", "Remparts_HourdA","Remparts_WallMotifA",
            "Remparts_WallRenfortA", "RichStairs_Rambarde",

            "Light_Brasero", "Light_BraseroSmall",
            "Light_Candle", "Light_CandleGroup",
            "Light_Crystal",
            "Light_TorchA", "Light_TorchB",
            "CharetteNew", "CharetteBroke",
            "Facade1", "Facade2",
            "Facade3", "Facade4",
            "Facade5", "Facade6",
            "Facade7", "Facade8",
            "Facade9", "Facade10",
            "TreeC",
            "RuinsPillar", "AckbarHouseLarge",
            "AckbarHouseSmall",  "AckbarHouseSwamp",
            "AckbarHouseRoofLarge",
            "AckbarHouseRoofMid", "AckbarHouseRoofSmall",
            "AqueducTowerRoof",
            "UnderRoofFilling", 
            "RotateGate",
            "Fragile Rock",
            "SkyDome test",

            "Cathedral",
            "Espadon_Station",
            "GrassLand_Ruins",
            "Tower_Ruins",
            "Bastion",
            "MetalGrid2",
            "Hoverboard_Track",
            "Hoverboard_Track_Pillar",
            "GrassLand_Arch",
            "Meteore",
            "Mountain Card",
            "Mountain BackGround",
            "AndrasArena_Deco",
            "wp1",
            "wp2",
            "wp3",
            "wp4",
            "wp5",
            "wp6",
            "wp7",
            "wp8",
            "wp9",
            "wp10",
            "wp11",
            "wp12",
            "WarpGate"


        };
        ImGui::ListBox("###Obj", &item_current, items_MapElement, IM_ARRAYSIZE(items_MapElement)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_ACTIVE_ELEMENT:
    {
        const char* items_MapObj[] = { "Grass", "TutorialMap Bridge", "Well", "FakeWall_Donut", "FakeWall_Box",
                                        "EventTrigger_Box", "EventTrigger_Sphere",
                                        "Elevator", "Treasure Chest", "Epic Chest", "Legendary Chest","Cloud" ,"BG Card" ,"Lagoon", "Moon"};
        ImGui::ListBox("###Obj", &item_current, items_MapObj, IM_ARRAYSIZE(items_MapObj)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_TRIGGER:
    {
        const char* items_Trigger[] = { "Tutorial_BOSS_ENCOUNTER",  "Jugglas_Room2_Spawn",  "Jugglas_Room3_Spawn", "Viewchange 3 to Side", "Viewchange Side to 3",
            "Ascend Elevator", "Descend Elevator", "Scene Change",  "Viewchange 3 to Boss Side",
           "scene change ackbar ", "scene change jugglas", "scene change andras",
            "CUTSCENE GRASSLAND", "CUTSCENE malkhel", "cutscene jugglus", "cutscene andras",
            "Racing Start", "Racing Win"

        };
        ImGui::ListBox("###Obj", &item_current, items_Trigger, IM_ARRAYSIZE(items_Trigger)); // item_current 변수에 선택 값 저장
        break;
    }
        break;
    case LAYER_ENVEFFECTS:
    {
        const char* items_EnvEffects[] = { "Fire", "Dust" };
        ImGui::ListBox("###Obj", &item_current, items_EnvEffects, IM_ARRAYSIZE(items_EnvEffects)); // item_current 변수에 선택 값 저장
        break;
    }
    break;
    case LAYER_TRAP:
    {
        const char* items_Traps[] = { "Hachoir", "SmashingPillar" };
        ImGui::ListBox("###Obj", &item_current, items_Traps, IM_ARRAYSIZE(items_Traps)); // item_current 변수에 선택 값 저장

        ImGui::SliderFloat("X##TimeOffset", &m_fTrapTimeOffset, 0.0f, 1.0f);


        break;
    }
    break;
    case LAYER_DECAL:
    {
        const char* items_Decals[58];
        for (int i = 0; i < 58; ++i)
        {
            static char buffer[32][58]; // 문자열을 저장할 정적 배열
            snprintf(buffer[i], sizeof(buffer[i]), "Decal %d", i);
            items_Decals[i] = buffer[i];
        }

        ImGui::ListBox("###Obj", &item_current, items_Decals, IM_ARRAYSIZE(items_Decals));

        // LAYER_DECAL이 선택되었을 때 텍스처 선택 창 표시
        m_bShowDecalTextureWindow = true;

        break;
    }
    break;
    case LAYER_VEGETATION:
    {
        const char* items_Vege[] = { "Grass", "BasicTree", "BirchTree", "BloomTree" ,
            "Bush"
        
        , "CherryTree"  , "GhostTree" , "PineTree", "WillowTree", "TreeC"
        
        };
        ImGui::ListBox("###Obj", &item_current, items_Vege, IM_ARRAYSIZE(items_Vege)); // item_current 변수에 선택 값 저장
        static float TopCol[3] = { 0.0f, 1.0f, 0.0f};
        static float BotCol[3] = { 0.0f, 0.0f, 0.0f};

        static float LeafCol[3] = { 0.0f, 0.0f, 0.0f };

        static _int count = 10000;

        static float BillboardFactor = 0.f;
        static float ElasticityFactor = 0.f;
        static float PlaneOffset = 0.15f;
        static float PlaneVertOffset = 0.15f;
        static float LODDistance1 = 300.0f;
        static float LODDistance2 = 500.0f;
        static int LODPlaneCount1 = 5;
        static int LODPlaneCount2 = 3;
        static int LODPlaneCount3 = 1;
        static float GrassFrequency = 1.5f;
        static float GrassAmplitude = 0.05f;

        if (item_current == 0)
        {
            ImGui::ColorEdit3("Top Color", TopCol);
            ImGui::ColorEdit3("Bot Color", BotCol);

            m_TopCol = { TopCol[0], TopCol[1], TopCol[2] };
            m_BotCol = { BotCol[0], BotCol[1], BotCol[2] };

    

            if (item_current == 0)
            {
                ImGui::SliderInt("Grass Plane Count", &count, 1000, 100000);
                m_iGrassCount = count;

                ImGui::SliderFloat("Billboard Factor", &BillboardFactor, 0.f, 1.f, "%.3f");
                m_fBillboardFactor = BillboardFactor;

                ImGui::SliderFloat("Elasticity Factor", &ElasticityFactor, 0.f, 1.f, "%.3f");
                m_fElasticityFactor = ElasticityFactor;


                ImGui::SliderFloat("Plane Offset", &PlaneOffset, 0.f, 5.f, "%.3f");
                m_fPlaneOffset = PlaneOffset;

                ImGui::SliderFloat("Plane VErt Offset", &PlaneVertOffset, 0.f, 5.f, "%.3f");
                m_fPlaneOffset = PlaneVertOffset;

                ImGui::SliderFloat("LOD Distance 1", &LODDistance1, 100.f, 1000.f, "%.1f");
                ImGui::SliderFloat("LOD Distance 2", &LODDistance2, LODDistance1, 2000.f, "%.1f");
                m_fLODDistance1 = LODDistance1;
                m_fLODDistance2 = LODDistance2;

                ImGui::SliderInt("LOD 1 Plane Count", &LODPlaneCount1, 1, 30);
                ImGui::SliderInt("LOD 2 Plane Count", &LODPlaneCount2, 1, 30);
                ImGui::SliderInt("LOD 3 Plane Count", &LODPlaneCount3, 1, 30);
                m_iLODPlaneCount1 = LODPlaneCount1;
                m_iLODPlaneCount2 = LODPlaneCount2;
                m_iLODPlaneCount3 = LODPlaneCount3;

                ImGui::SliderFloat("Grass Frequency", &GrassFrequency, 0.1f, 5.f, "%.2f");
                ImGui::SliderFloat("Grass Amplitude", &GrassAmplitude, 0.f, 0.2f, "%.3f");
                m_fGrassFrequency = GrassFrequency;
                m_fGrassAmplitude = GrassAmplitude;
            }

        }
        else
        {
            ImGui::ColorEdit3("Leaf Color", LeafCol);
            m_LeafCol = { LeafCol[0], LeafCol[1], LeafCol[2] };
        }

        ImGui::Checkbox("Bloom", &m_bTreeBloom);

    }
    break;
    default:
        break;
    }

    m_iObjIdx = item_current;
}

void CImgui_Manager::Add_ToolObj(_vector vPosition, _vector vColor)
{
    /*CMonster::MONSTER_DESC pDesc = {};

    pDesc.vPosition = vPosition;*/

    // Calculator의 Picking_OnTerrain( ) 호출 후 반환 값이 -1, -1, -1이 아니면 해당 위치에 모델 생성
    /*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &pDesc)))
        return E_FAIL;*/

    if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(m_iLayerIdx, m_iObjIdx, vPosition, vColor)))
    {
        MSG_BOX("ToolObj 생성 실패");
        return;

    }

    return;
}

void CImgui_Manager::Light_Editor()
{
    ImGui::Begin("Light Editing", &m_bLightWindow);
    ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Input LightDesc : ");
    static LIGHT_DESC desc{};
    static int lightIdx = 0;
    static float diffuseCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static float ambientCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static float specularCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    static XMFLOAT4 direction{ 0.f, 0.f, 0.f, 1.f };
    static XMFLOAT4 position{ 0.f, 0.f, 0.f, 1.f };
    static float range = 15.0f;
    static float innerAngle = 30.0f;
    static float outerAngle = 80.0f;

    // 빛의 종류 선택
    const char* lightTypes[] = { "Directional", "Point", "Spotlight" };
    static int selectedLightType = 0;

    // LightIdx 선택
    if (ImGui::InputInt("Light Index", &lightIdx))
    {
        LIGHT_DESC newDesc{};
        newDesc = *m_pGameInstance->Get_LightDesc(lightIdx);

        // newDesc의 변수들로 static 변수 초기화
        selectedLightType = static_cast<int>(newDesc.eType);
        direction = newDesc.vDirection;
        position = newDesc.vPosition;
        range = newDesc.fRange;
        innerAngle = newDesc.innerAngle;
        outerAngle = newDesc.outerAngle;
        memcpy(diffuseCol, &newDesc.vDiffuse, sizeof(float) * 4);
        memcpy(ambientCol, &newDesc.vAmbient, sizeof(float) * 4);
        memcpy(specularCol, &newDesc.vSpecular, sizeof(float) * 4);
    }


    ImGui::Combo("Light Type", &selectedLightType, lightTypes, IM_ARRAYSIZE(lightTypes));

    ImGui::Spacing();

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Light On / Off");

    if (ImGui::Button("Light On"))
        m_pGameInstance->LightOn(lightIdx);
    ImGui::SameLine();

    // ImGui::Spacing();
    if (ImGui::Button("Light Off"))
        m_pGameInstance->LightOff(lightIdx);

    // Gizmo 세팅
    float cameraView[16];
    float cameraProjection[16];
    memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
    memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

    // 빛의 위치와 방향을 나타내는 행렬 생성
    XMFLOAT4X4 lightMatrix;
    XMStoreFloat4x4(&lightMatrix, XMMatrixIdentity());

    switch (selectedLightType)
    {
    case 0: // Directional
    {
        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Direction");
        ImGui::SliderFloat("X##DirectionX", &direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y##DirectionY", &direction.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z##DirectionZ", &direction.z, -1.0f, 1.0f);

        // 방향 벡터 정규화
        XMVECTOR directionVector = XMVector3Normalize(XMLoadFloat4(&direction));
        XMStoreFloat4(&direction, directionVector);
    }
    break;
    case 1: // Point
    {
        XMStoreFloat4x4(&lightMatrix, XMMatrixTranslation(position.x, position.y, position.z));

        if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
        {
            // 조작된 행렬에서 위치 추출
            XMStoreFloat4(&position, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
        }

        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Position");
        ImGui::InputFloat3("##PositionInput", (float*)&position, "%.2f");
        ImGui::InputFloat("Range", &range);
    }
    break;
    case 2: // Spotlight
    {
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&position));
        XMStoreFloat4x4(&lightMatrix, translationMatrix);

        if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
        {
            // 조작된 행렬에서 위치 추출
            XMStoreFloat4(&position, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
        }

        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Position");
        ImGui::InputFloat3("##PositionInput", (float*)&position, "%.2f");
        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Direction");
        ImGui::SliderFloat("X##DirectionX", &direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y##DirectionY", &direction.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z##DirectionZ", &direction.z, -1.0f, 1.0f);

        // 방향 벡터 정규화
        XMVECTOR directionVector = XMVector3Normalize(XMLoadFloat4(&direction));
        XMStoreFloat4(&direction, directionVector);

        ImGui::InputFloat("Range", &range);
        ImGui::InputFloat("Inner Angle", &innerAngle);
        ImGui::InputFloat("Outer Angle", &outerAngle);
    }
    break;
    }

    // 빛 색 조정
    ImGui::ColorEdit4("Light Diffuse", diffuseCol);
    ImGui::ColorEdit4("Light Ambient", ambientCol);
    ImGui::ColorEdit4("Light Specular", specularCol);

    ImGui::Separator();
    ImGui::Spacing();

    static bool autoUpdateLight = false;
    ImGui::Checkbox("Auto Update Light", &autoUpdateLight);

    ImGui::Spacing();

    if (ImGui::Button("Set LightDesc") || autoUpdateLight)
    {
        desc.eType = static_cast<LIGHT_DESC::TYPE>(selectedLightType);
        desc.vDiffuse = _float4{ diffuseCol[0], diffuseCol[1], diffuseCol[2], diffuseCol[3] };
        desc.vAmbient = _float4{ ambientCol[0], ambientCol[1], ambientCol[2], ambientCol[3] };
        desc.vSpecular = _float4{ specularCol[0], specularCol[1], specularCol[2], specularCol[3] };

        switch (selectedLightType)
        {
        case 0: // Directional
            desc.vDirection = direction;
            break;
        case 1: // Point
            desc.vPosition = position;
            desc.fRange = range;
            break;
        case 2: // Spotlight
            desc.vDirection = direction;
            desc.vPosition = position;
            desc.fRange = range;
            desc.innerAngle = XMConvertToRadians(innerAngle);
            desc.outerAngle = XMConvertToRadians(outerAngle);
            break;
        }

        m_pGameInstance->Edit_Light(lightIdx, &desc);
    }

    ImGui::Spacing();

    if (ImGui::Button("Add Light"))
    {
        desc.eType = static_cast<LIGHT_DESC::TYPE>(selectedLightType);
        desc.vDiffuse = _float4{ diffuseCol[0], diffuseCol[1], diffuseCol[2], diffuseCol[3] };
        desc.vAmbient = _float4{ ambientCol[0], ambientCol[1], ambientCol[2], ambientCol[3] };
        desc.vSpecular = _float4{ specularCol[0], specularCol[1], specularCol[2], specularCol[3] };

        switch (selectedLightType)
        {
        case 0: // Directional
            desc.vDirection = direction;
            break;
        case 1: // Point
            desc.vPosition = position;
            desc.fRange = range;
            break;
        case 2: // Spotlight
            desc.vDirection = direction;
            desc.vPosition = position;
            desc.fRange = range;
            desc.innerAngle = XMConvertToRadians(innerAngle);
            desc.outerAngle = XMConvertToRadians(outerAngle);
            break;
        }

        m_pGameInstance->Add_Light(desc);
    }

    ImGui::Spacing();


    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Save Lights"))
        Save_Lights();

    ImGui::SameLine();
    if (ImGui::Button("Load Lights"))
        Load_Lights();

    ImGui::SameLine();
    if (ImGui::Button("Clear Lights"))
    {
        m_pGameInstance->LightOff_All();
        lightIdx = 0;
    }




    ImGui::Spacing();

    if (ImGui::Button("Close"))
        m_bLightWindow = false;

    ImGui::End();
}

ImTextureID CImgui_Manager::DirectXTextureToImTextureID(_uint iIdx)
{
    vector<ID3D11ShaderResourceView*> SRV = m_pDecalTexs->Get_Textures();
    return reinterpret_cast<ImTextureID>(SRV[iIdx]);
}

void CImgui_Manager::Save_Lights()
{
    int iStageIdx = Get_StageIdx();
    switch (iStageIdx)
    {
    case STAGE_HOME:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage_Lights.dat";
        break;
    case STAGE_ONE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage1_Lights.dat";
        break;
    case STAGE_TWO:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage2_Lights.dat";
        break;
    case STAGE_THREE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage3_Lights.dat";
        break;
    case STAGE_BOSS:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage4_Lights.dat";
        break;
    default:
        MSG_BOX("Setting File Name is Failed");
        return;
    }

    HANDLE hFile = CreateFile(m_LightsDataPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
    {
        MSG_BOX("hFile is nullptr");
        return;
    }

    DWORD dwByte(0);

    list<CLight*> lights = m_pGameInstance->Get_Lights();

    _uint iLightCount = lights.size();
    WriteFile(hFile, &iLightCount, sizeof(_uint), &dwByte, nullptr);

    for (auto& iter : lights)
    {
        if (!iter->Get_LightOn()) continue;

        LIGHT_DESC desc{};
        desc = *iter->Get_LightDesc();

        WriteFile(hFile, &desc.eType, sizeof(LIGHT_DESC::TYPE), &dwByte, nullptr);
        WriteFile(hFile, &desc.vDiffuse, sizeof(XMFLOAT4), &dwByte, nullptr);
        WriteFile(hFile, &desc.vAmbient, sizeof(XMFLOAT4), &dwByte, nullptr);
        WriteFile(hFile, &desc.vSpecular, sizeof(XMFLOAT4), &dwByte, nullptr);

        switch (desc.eType)
        {
        case LIGHT_DESC::TYPE_DIRECTIONAL:
            WriteFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_POINT:
            WriteFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_SPOTLIGHT:
            WriteFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &desc.innerAngle, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &desc.outerAngle, sizeof(float), &dwByte, nullptr);
            break;
        }
    }

    CloseHandle(hFile);
    MSG_BOX("Lights Data Save");

    return;
}

void CImgui_Manager::Load_Lights()
{
    list<CLight*> lights = m_pGameInstance->Get_Lights();
    for (auto& iter : lights)
    {
        iter->LightOff();
    }

    int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();
    switch (iStageIdx)
    {
    case STAGE_HOME:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Ackbar_Lights.dat";
        break;
    case STAGE_ONE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Tutorial_Lights.dat";
        break;
    case STAGE_TWO:
        m_LightsDataPath = L"../Bin/MapData/LightsData/AndrasArena_Lights.dat";
        break;
    case STAGE_THREE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Juggulas_Lights.dat";
        break;
    case STAGE_BOSS:
        m_LightsDataPath = L"../Bin/MapData/LightsData/GrassLand_Lights.dat";
        break;
    default:
        MSG_BOX("Setting File Name is Failed");
        return;
    }

    HANDLE hFile = CreateFile(m_LightsDataPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
    MSG_BOX("Lights Data Load");

    return;
}

void CImgui_Manager::Shadow_Editor()
{
    ImGui::Begin("Shadow Settings", &m_bShadowWindow);
    m_bShowAnotherWindow = false;
    m_bLightWindow = false;

    static float ShadowEye[4] = { 0.f, 10.f, -10.f, 1.f };
    static float ShadowFocus[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static int selectedPosition = 0; // 0 for Eye, 1 for Focus
    static bool autoUpdateShadow = false;
    static float shadowThreshold = 0.5f; // 새로운 float 변수 추가

    const char* positionTypes[] = { "Eye", "Focus" };
    ImGui::Combo("Select Position", &selectedPosition, positionTypes, IM_ARRAYSIZE(positionTypes));

    // Gizmo 세팅
    float cameraView[16];
    float cameraProjection[16];
    memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
    memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

    // Shadow Eye/Focus 위치를 나타내는 행렬 생성
    XMFLOAT4X4 shadowMatrix;
    XMStoreFloat4x4(&shadowMatrix, XMMatrixTranslation(
        selectedPosition == 0 ? ShadowEye[0] : ShadowFocus[0],
        selectedPosition == 0 ? ShadowEye[1] : ShadowFocus[1],
        selectedPosition == 0 ? ShadowEye[2] : ShadowFocus[2]
    ));

    bool positionChanged = false;

    if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&shadowMatrix, NULL, NULL))
    {
        // 조작된 행렬에서 위치 추출
        XMFLOAT4 newPosition;
        XMStoreFloat4(&newPosition, XMLoadFloat4((XMFLOAT4*)&shadowMatrix._41));

        if (selectedPosition == 0)
        {
            ShadowEye[0] = newPosition.x;
            ShadowEye[1] = newPosition.y;
            ShadowEye[2] = newPosition.z;
        }
        else
        {
            ShadowFocus[0] = newPosition.x;
            ShadowFocus[1] = newPosition.y;
            ShadowFocus[2] = newPosition.z;
        }
        positionChanged = true;
    }

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Threshold");

    ImGui::SliderFloat("##ShadowParam", &shadowThreshold, 0.0f, 1.1f, "%.2f");
   
    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Eye");
    if (ImGui::InputFloat3("##EyeInput", ShadowEye, "%.2f"))
        positionChanged = true;

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Focus");
    if (ImGui::InputFloat3("##FocusInput", ShadowFocus, "%.2f"))
        positionChanged = true;

    ImGui::Checkbox("Auto Update Shadow", &autoUpdateShadow);

    if (ImGui::Button("Set Shadow") || (autoUpdateShadow && positionChanged))
    {
        _vector vEye = XMLoadFloat4((XMFLOAT4*)ShadowEye);
        _vector vFocus = XMLoadFloat4((XMFLOAT4*)ShadowFocus);
        m_pGameInstance->Set_ShadowEyeFocus(vEye, vFocus, shadowThreshold);
    }

    ImGui::End();
}

void CImgui_Manager::Camera_Editor()
{
    ImGui::Begin("Camera Settings", &m_bCameraWindow);

    CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[3]);
    if (pCutSceneCamera)
    {
        // CutScene 선택
        static int selectedCutScene = 0;
        static vector<string> cutSceneNames;
        static vector<const char*> cutSceneNamesCStr;

        if (cutSceneNames.size() != pCutSceneCamera->Get_CutSceneCount())
        {
            cutSceneNames.clear();
            cutSceneNamesCStr.clear();
            for (_uint i = 0; i < pCutSceneCamera->Get_CutSceneCount(); ++i)
            {
                cutSceneNames.push_back("CutScene " + to_string(i));
            }
            for (const auto& name : cutSceneNames)
            {
                cutSceneNamesCStr.push_back(name.c_str());
            }
        }

        ImGui::Combo("Select CutScene", &selectedCutScene, cutSceneNamesCStr.data(), cutSceneNamesCStr.size());

        // CutScene 컨트롤
        if (ImGui::Button("Add New CutScene"))
        {
            vector<CCamera::CameraKeyFrame> newCutScene;
            pCutSceneCamera->Add_CutScene(newCutScene);
            selectedCutScene = pCutSceneCamera->Get_CutSceneCount() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Edit CutScene"))
        {
            m_vCameraKeyFrames = pCutSceneCamera->Get_CutScene(selectedCutScene);
            m_bEditingCutScene = true;
            m_iEditingCutSceneIdx = selectedCutScene;
        }
        ImGui::SameLine();
        if (ImGui::Button("Play CutScene"))
        {
            pCutSceneCamera->Set_CutSceneIdx(selectedCutScene);
            pCutSceneCamera->Play_CutScene();
            m_pGameInstance->Set_MainCamera(3);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop CutScene"))
        {
            pCutSceneCamera->Stop_CutScene();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pop CutScene"))
        {
            pCutSceneCamera->Pop_CutScene(selectedCutScene);
        }

        // 현재 재생 중인 컷신 정보
        ImGui::Text("Current CutScene: %d", pCutSceneCamera->Get_CutSceneIdx());
        ImGui::Text("Animation Progress: %.2f%%", pCutSceneCamera->Get_AnimationProgress() * 100.0f);
        // 컷씬 카메라 데이터 세이브 버튼 추가
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "CutScene Camera Data");

        if (ImGui::Button("Save CutScene Data"))
        {
            Save_CameraKeyFrames();
        }

        ImGui::SameLine();

        if (ImGui::Button("Load CutScene Data"))
        {
            Load_CameraKeyFrames();
        }
        // 컷신 편집 모드
        if (m_bEditingCutScene)
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), "Editing CutScene %d", m_iEditingCutSceneIdx);

            // KeyFrame 목록
            static int selectedKeyFrame = 0;
            static vector<string> keyFrameNames;
            static vector<const char*> keyFrameNamesCStr;

            // 키프레임 이름 목록 업데이트
            if (keyFrameNames.size() != m_vCameraKeyFrames.size())
            {
                keyFrameNames.clear();
                keyFrameNamesCStr.clear();
                for (size_t i = 0; i < m_vCameraKeyFrames.size(); ++i)
                {
                    keyFrameNames.push_back("KeyFrame " + to_string(i));
                }
                for (const auto& name : keyFrameNames)
                {
                    keyFrameNamesCStr.push_back(name.c_str());
                }
            }

            ImGui::ListBox("KeyFrames", &selectedKeyFrame, keyFrameNamesCStr.data(), keyFrameNamesCStr.size());

            // KeyFrame 저장 버튼
            if (ImGui::Button("Save New KeyFrame"))
            {
                CCamera::CameraKeyFrame newKeyFrame;
                newKeyFrame.fTime = m_fKeyFrameTime;
                m_fKeyFrameTime += 0.5f;
                newKeyFrame.matWorld = *m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
                newKeyFrame.fFovy = XMConvertToRadians(60.f);
                newKeyFrame.fNear = 0.1f;
                newKeyFrame.fFar = 3000.f;

                m_vCameraKeyFrames.push_back(newKeyFrame);
                selectedKeyFrame = m_vCameraKeyFrames.size() - 1;
            }

            ImGui::SameLine();

            // KeyFrame 삭제 버튼
            if (ImGui::Button("Delete KeyFrame") && !m_vCameraKeyFrames.empty())
            {
                m_vCameraKeyFrames.erase(m_vCameraKeyFrames.begin() + selectedKeyFrame);
                if (selectedKeyFrame >= m_vCameraKeyFrames.size())
                {
                    selectedKeyFrame = m_vCameraKeyFrames.size() - 1;
                }
            }

            // KeyFrame 편집
            if (selectedKeyFrame >= 0 && selectedKeyFrame < m_vCameraKeyFrames.size())
            {
                CCamera::CameraKeyFrame& keyFrame = m_vCameraKeyFrames[selectedKeyFrame];

                ImGui::SliderFloat("Time", &keyFrame.fTime, 0.0f, 20.0f);
                float fovDegrees = XMConvertToDegrees(keyFrame.fFovy);
                if (ImGui::SliderFloat("FOV", &fovDegrees, 1.0f, 179.0f))
                {
                    keyFrame.fFovy = XMConvertToRadians(fovDegrees);
                }

                // Speed Changes
                if (ImGui::TreeNode("Speed Changes"))
                {
                    for (size_t j = 0; j < keyFrame.speedChanges.size(); ++j)
                    {
                        ImGui::PushID(static_cast<int>(j));
                        ImGui::SliderFloat("Timing", &get<0>(keyFrame.speedChanges[j]), 0.0f, 1.0f);
                        ImGui::SliderFloat("Speed", &get<1>(keyFrame.speedChanges[j]), 0.1f, 10.0f);
                        ImGui::SliderFloat("Smooth Offset", &get<2>(keyFrame.speedChanges[j]), 0.0f, 1.0f);
                        ImGui::PopID();
                    }
                    if (ImGui::Button("Add Speed Change"))
                    {
                        keyFrame.speedChanges.emplace_back(0.0f, 1.0f, 0.3f);
                    }
                    ImGui::TreePop();
                }

                // Speed Change Graph
                if (!keyFrame.speedChanges.empty())
                {
                    ImGui::Spacing();
                    ImGui::Text("Speed Change Graph:");
                    ImVec2 graphSize(250, 120);
                    ImVec2 graphPos = ImGui::GetCursorScreenPos();
                    graphPos.x += 30;

                    // 그래프 배경 및 격자 그리기
                    ImGui::GetWindowDrawList()->AddRectFilled(
                        graphPos,
                        ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y),
                        IM_COL32(30, 30, 30, 255));

                    // 수평 격자선 및 y축 레이블
                    for (int i = 0; i <= 5; i++) {
                        float y = graphPos.y + (i * graphSize.y / 5);
                        ImGui::GetWindowDrawList()->AddLine(
                            ImVec2(graphPos.x, y),
                            ImVec2(graphPos.x + graphSize.x, y),
                            IM_COL32(100, 100, 100, 100));
                        char label[16];
                        snprintf(label, sizeof(label), "%.f", (5 - i) * 2.0f);
                        ImGui::GetWindowDrawList()->AddText(
                            ImVec2(graphPos.x + graphSize.x + 10, y - 7),
                            IM_COL32(200, 200, 200, 150),
                            label);
                    }

                    // 수직 격자선 및 x축 레이블
                    for (int i = 0; i <= 10; i++) {
                        float x = graphPos.x + (i * graphSize.x / 10);
                        ImGui::GetWindowDrawList()->AddLine(
                            ImVec2(x, graphPos.y),
                            ImVec2(x, graphPos.y + graphSize.y),
                            IM_COL32(100, 100, 100, 100));
                        char label[16];
                        snprintf(label, sizeof(label), "%d", i);
                        ImGui::GetWindowDrawList()->AddText(
                            ImVec2(x - 10, graphPos.y + graphSize.y + 5),
                            IM_COL32(200, 200, 200, 150),
                            label);
                    }

                    // 속도 그래프 그리기
                    std::vector<ImVec2> points;
                    for (int i = 0; i <= 100; i++) {
                        float t = static_cast<float>(i) / 100.0f;
                        float speedMultiplier = 1.0f;
                        float prevSpeedMultiplier = 1.0f;

                        for (const auto& [changeTime, changeSpeed, smoothOffset] : keyFrame.speedChanges)
                        {
                            if (t < changeTime - smoothOffset)
                            {
                                speedMultiplier = prevSpeedMultiplier;
                                break;
                            }
                            else if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
                            {
                                float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
                                speedMultiplier = prevSpeedMultiplier + (changeSpeed - prevSpeedMultiplier) * lerpT * lerpT * (3.0f - 2.0f * lerpT);
                                break;
                            }
                            else
                            {
                                prevSpeedMultiplier = changeSpeed;
                            }
                        }

                        points.push_back(ImVec2(
                            graphPos.x + t * graphSize.x,
                            graphPos.y + graphSize.y - (speedMultiplier / 10.0f) * graphSize.y
                        ));
                    }

                    // 선 그리기
                    for (size_t i = 1; i < points.size(); i++) {
                        ImGui::GetWindowDrawList()->AddLine(points[i - 1], points[i], IM_COL32(0, 255, 0, 255), 2.0f);
                    }

                    // 속도 변화 지점 표시
                    for (const auto& [changeTime, changeSpeed, smoothOffset] : keyFrame.speedChanges)
                    {
                        float x = graphPos.x + changeTime * graphSize.x;
                        float y = graphPos.y + graphSize.y - (changeSpeed / 10.0f) * graphSize.y;
                        ImGui::GetWindowDrawList()->AddCircleFilled(
                            ImVec2(x, y), 4.0f, IM_COL32(255, 0, 0, 255));
                    }

                    ImGui::Dummy(graphSize);
                }
            }

            if (ImGui::Button("Save Changes"))
            {
                pCutSceneCamera->Set_CutScene(m_iEditingCutSceneIdx, m_vCameraKeyFrames);
                m_bEditingCutScene = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel Editing"))
            {
                m_bEditingCutScene = false;
            }
        }
    }
    else
    {
        ImGui::Text("CutSceneCamera not found.");
    }

    ImGui::End();
}

void CImgui_Manager::Save_CameraKeyFrames()
{
    wstring filePath = L"../Bin/CutSceneData/CutSceneKeyFrames.dat";

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed to create file for camera keyframes.");
        return;
    }

    DWORD dwByte = 0;

    CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[3]);
    if (!pCutSceneCamera)
    {
        CloseHandle(hFile);
        MSG_BOX("CutSceneCamera not found.");
        return;
    }

    // 컷씬 개수 저장
    _uint iCutSceneCount = pCutSceneCamera->Get_CutSceneCount();
    WriteFile(hFile, &iCutSceneCount, sizeof(_uint), &dwByte, nullptr);

    // 각 컷씬의 키프레임 저장
    for (_uint i = 0; i < iCutSceneCount; ++i)
    {
        const vector<CCamera::CameraKeyFrame>& cutScene = pCutSceneCamera->Get_CutScene(i);

        // 키프레임 개수 저장
        _uint iKeyFrameCount = cutScene.size();
        WriteFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

        // 각 키프레임 정보 저장
        for (const auto& keyFrame : cutScene)
        {
            WriteFile(hFile, &keyFrame.fTime, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &keyFrame.matWorld, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &keyFrame.fFovy, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &keyFrame.fNear, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &keyFrame.fFar, sizeof(float), &dwByte, nullptr);

            // 속도 변화 정보 저장
            _uint iSpeedChangeCount = keyFrame.speedChanges.size();
            WriteFile(hFile, &iSpeedChangeCount, sizeof(_uint), &dwByte, nullptr);
            for (const auto& speedChange : keyFrame.speedChanges)
            {
                WriteFile(hFile, &get<0>(speedChange), sizeof(float), &dwByte, nullptr);
                WriteFile(hFile, &get<1>(speedChange), sizeof(float), &dwByte, nullptr);
                WriteFile(hFile, &get<2>(speedChange), sizeof(float), &dwByte, nullptr);
            }
        }
    }

    CloseHandle(hFile);
    MSG_BOX("Camera KeyFrames Data Saved");
}

void CImgui_Manager::Load_CameraKeyFrames()
{
    wstring filePath = L"../Bin/CutSceneData/CutSceneKeyFrames.dat";

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed to open file for camera keyframes.");
        return;
    }

    DWORD dwByte = 0;

    CCutSceneCamera* pCutSceneCamera = dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[3]);
    if (!pCutSceneCamera)
    {
        CloseHandle(hFile);
        MSG_BOX("CutSceneCamera not found.");
        return;
    }

    // 컷씬 개수 로드
    _uint iCutSceneCount = 0;
    ReadFile(hFile, &iCutSceneCount, sizeof(_uint), &dwByte, nullptr);

    // 기존 컷씬 클리어
   // pCutSceneCamera->Clear_CutScenes();

    // 각 컷씬의 키프레임 로드
    for (_uint i = 0; i < iCutSceneCount; ++i)
    {
        vector<CCamera::CameraKeyFrame> cutScene;

        // 키프레임 개수 로드
        _uint iKeyFrameCount = 0;
        ReadFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

        // 각 키프레임 정보 로드
        for (_uint j = 0; j < iKeyFrameCount; ++j)
        {
            CCamera::CameraKeyFrame keyFrame;
            ReadFile(hFile, &keyFrame.fTime, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.matWorld, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFovy, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fNear, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFar, sizeof(float), &dwByte, nullptr);

            // 속도 변화 정보 로드
            _uint iSpeedChangeCount = 0;
            ReadFile(hFile, &iSpeedChangeCount, sizeof(_uint), &dwByte, nullptr);
            for (_uint k = 0; k < iSpeedChangeCount; ++k)
            {
                float time, speed, smoothOffset;
                ReadFile(hFile, &time, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &speed, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &smoothOffset, sizeof(float), &dwByte, nullptr);
                keyFrame.speedChanges.emplace_back(time, speed, smoothOffset);
            }

            cutScene.push_back(keyFrame);
        }

        pCutSceneCamera->Add_CutScene(cutScene);
    }

    CloseHandle(hFile);
    MSG_BOX("Camera KeyFrames Data Loaded");
}

void CImgui_Manager::Battle_Camera_Editor()
{
    if (m_pGameInstance->Get_MainCameraIdx() == 1) // 메인 카메라가 ThirdPersonCamera일 때
    {
        CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_MainCamera());
        if (pThirdPersonCamera)
        {
            ImGui::Begin("Battle Camera Editor");

            // 셰이크 설정
            static float fShakeDuration = 0.5f;
            static float fShakePosAmount = 0.1f;
            static float fShakeRotAmount = 0.1f;
            static float fShakeFrequency = 25.f;

            // Zoom 설정
            static float fTargetFov = 45.0f;
            static float fMaxZoomTime = 0.5f;
            static float fRecoverTime = 1.0f;

            static bool autoUpdate = false;
            bool settingsChanged = false;

            // 셰이크 설정
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shake Settings");
            settingsChanged |= ImGui::SliderFloat("Shake Duration", &fShakeDuration, 0.1f, 2.0f);
            settingsChanged |= ImGui::SliderFloat("Position Amount", &fShakePosAmount, 0.01f, 1.0f);
            settingsChanged |= ImGui::SliderFloat("Rotation Amount", &fShakeRotAmount, 0.01f, 1.0f);
            settingsChanged |= ImGui::SliderFloat("Shake Frequency", &fShakeFrequency, 1.0f, 100.0f);

            if (ImGui::Button("Test Shake"))
            {
                pThirdPersonCamera->Shake_Camera(fShakeDuration, fShakePosAmount, fShakeRotAmount, fShakeFrequency);
            }

            // 현재 셰이크 상태 표시
            ImGui::Text("Current Shake State:");
            ImGui::Text("Is Shaking: %s", pThirdPersonCamera->Get_isShaking() ? "Yes" : "No");
            ImGui::Text("Shake Timer: %.2f / %.2f", pThirdPersonCamera->Get_ShakeTimer(), pThirdPersonCamera->Get_ShakeDuration());

            // Zoom 설정
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Zoom Settings");
            settingsChanged |= ImGui::SliderFloat("Target FOV", &fTargetFov, 10.0f, 120.0f);
            settingsChanged |= ImGui::SliderFloat("Max Zoom Time", &fMaxZoomTime, 0.1f, 2.0f);
            settingsChanged |= ImGui::SliderFloat("Recover Time", &fRecoverTime, 0.1f, 2.0f);

       
            if (ImGui::Button("Test Zoom"))
            {
                pThirdPersonCamera->Zoom(fTargetFov, fMaxZoomTime, fRecoverTime);
            }

            ImGui::Checkbox("Auto Update", &autoUpdate);

            if (ImGui::Button("Apply Settings") || (autoUpdate && settingsChanged))
            {
                // 여기서는 즉시 적용하지 않고, Test 버튼을 누를 때 적용됩니다.
            }

            // 현재 Zoom 상태 표시
           /* ImGui::Text("Current Zoom State:");
            ImGui::Text("Is Zooming: %s", pThirdPersonCamera->Get_isZooming() ? "Yes" : "No");
            ImGui::Text("Current FOV: %.2f", pThirdPersonCamera->Get_CurrentFOV());*/

            // Tilt 설정
            static float fTiltAngle = 30.0f;
            static float fTiltDuration = 0.5f;
            static float fTiltRecoveryDuration = 0.5f;

            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Tilt Settings");
            settingsChanged |= ImGui::SliderFloat("Tilt Angle", &fTiltAngle, -45.0f, 45.0f);
            settingsChanged |= ImGui::SliderFloat("Tilt Duration", &fTiltDuration, 0.1f, 2.0f);
            settingsChanged |= ImGui::SliderFloat("Tilt Recovery Duration", &fTiltRecoveryDuration, 0.1f, 2.0f);

            if (ImGui::Button("Test Tilt"))
            {
                pThirdPersonCamera->StartTilt(fTiltAngle, fTiltDuration, fTiltRecoveryDuration);
            }

            // 현재 Tilt 상태 표시
  /*          ImGui::Text("Current Tilt State:");
            ImGui::Text("Is Tilting: %s", pThirdPersonCamera->Get_isTilting() ? "Yes" : "No");
            ImGui::Text("Tilt Timer: %.2f / %.2f", pThirdPersonCamera->Get_TiltTimer(),
                pThirdPersonCamera->Get_TiltDuration() + pThirdPersonCamera->Get_TiltRecoveryDuration());
            ImGui::Text("Current Roll Angle: %.2f", pThirdPersonCamera->Get_CurrentRollAngle());*/


            ImGui::End();
        }
    }
}
void CImgui_Manager::Terrain_Editor()
{
    static char heightMapPath[256] = "../Bin/Resources/Textures/Terrain/heightmap.r16";
    static float maxHeight = 100.0f;
    static float brushSize = 10.0f;
    static float brushStrength = 0.1f;
    static int brushMode = 0;
    static float flattenHeight = 50.0f;
    static bool bShowBrush = false;

    ImGui::Begin("Terrain Editor");

    ImGui::Separator();

    // 눈 지형 설정
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Snow Ground Settings");

    static float snowGroundHeight = 500.0f;
    static float snowGroundHeightOffset = 50.0f;

    if (ImGui::SliderFloat("Snow Ground Height", &snowGroundHeight, 0.0f, 1000.0f))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            pTerrain->Set_SnowGroundHeight(snowGroundHeight);
        }
    }

    if (ImGui::SliderFloat("Snow Ground Height Offset", &snowGroundHeightOffset, 0.0f, 200.0f))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            pTerrain->Set_SnowGroundHeightOffset(snowGroundHeightOffset);
        }
    }


    // 높이맵 섹션
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Height Map");
    ImGui::InputText("Height Map Path", heightMapPath, IM_ARRAYSIZE(heightMapPath));

    // Load 버튼
    if (ImGui::Button("Load Height Map"))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            std::wstring wHeightMapPath;
            wHeightMapPath.assign(heightMapPath, heightMapPath + strlen(heightMapPath));
            pTerrain->LoadHeightMap(wHeightMapPath);
        }
    }

    // Save 버튼
    ImGui::SameLine();
    if (ImGui::Button("Save Height Map"))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            std::wstring wHeightMapPath;
            wHeightMapPath.assign(heightMapPath, heightMapPath + strlen(heightMapPath));
            if (SUCCEEDED(pTerrain->SaveHeightMapToR16(wHeightMapPath)))
            {
                ImGui::OpenPopup("Save Successful");
            }
            else
            {
                ImGui::OpenPopup("Save Failed");
            }
        }
    }

    // 성공/실패 팝업
    if (ImGui::BeginPopupModal("Save Successful", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Height map saved successfully.");
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Failed to save height map.");
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SliderFloat("Max Height", &maxHeight, 10.0f, 500.0f);
    ImGui::SliderFloat("Brush Size", &brushSize, 1.0f, 100.0f);
    ImGui::SliderFloat("Brush Strength", &brushStrength, 0.01f, 1.0f);

    ImGui::Separator();

    // 브러쉬 모드 선택
    const char* brushModes[] = { "Raise", "Lower", "Flatten" };
    ImGui::Combo("Brush Mode", &brushMode, brushModes, IM_ARRAYSIZE(brushModes));

    // 평탄화 높이 설정 (Flatten 모드에서 사용)
    if (brushMode == 2) // Flatten 모드
    {
        ImGui::SliderFloat("Flatten Height", &flattenHeight, 0.0f, maxHeight);
    }

    ImGui::Separator();

    // 브러쉬 섹션
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Brush");
    ImGui::Checkbox("Show Brush", &bShowBrush);

    if (bShowBrush && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered())
    {
        _bool isPick = false;
        _float4 pPickPos = { 0.f, 0.f, 0.f, 1.f };
        if (m_pGameInstance->Get_PickPos(&pPickPos))
        {
            CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
            if (pTerrain)
            {
                switch (brushMode)
                {
                case 0: // Raise
                    pTerrain->Brush_Picking(XMLoadFloat4(&pPickPos), brushSize, brushStrength, maxHeight);
                    break;
                case 1: // Lower
                    pTerrain->Brush_Picking(XMLoadFloat4(&pPickPos), brushSize, -brushStrength, maxHeight);
                    break;
                case 2: // Flatten
                    pTerrain->Brush_Flatten(XMLoadFloat4(&pPickPos), brushSize, brushStrength, flattenHeight);
                    break;
                }
            }
        }
    }

    ImGui::End();
}

void CImgui_Manager::GlobalWind_Editor()
{
    static float windStrength = 1.0f;
    static float windFrequency = 1.0f;
    static bool enableWind = true;

    ImGui::Begin("Global Wind Editor");

    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Wind Direction");
    ImGui::SliderFloat("X", &m_GlobalWindDir.x, -1.0f, 1.0f);
    ImGui::SliderFloat("Y", &m_GlobalWindDir.y, -1.0f, 1.0f);
    ImGui::SliderFloat("Z", &m_GlobalWindDir.z, -1.0f, 1.0f);

    if (ImGui::Button("Normalize Direction"))
    {
        float length = sqrt(m_GlobalWindDir.x * m_GlobalWindDir.x +
            m_GlobalWindDir.y * m_GlobalWindDir.y +
            m_GlobalWindDir.z * m_GlobalWindDir.z);
        if (length > 0.0001f)
        {
            m_GlobalWindDir.x /= length;
            m_GlobalWindDir.y /= length;
            m_GlobalWindDir.z /= length;
        }
    }

    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Wind Properties");
    ImGui::SliderFloat("Wind Strength", &m_fGlobalWindStrength, 0.0f, 30.0f);
    ImGui::SliderFloat("Wind Frequency", &m_fGlobalWindFrequency, 0.1f, 5.0f);
    ImGui::Checkbox("Enable Wind", &enableWind);

    ImGui::Separator();

    if (ImGui::Button("Apply Wind Settings"))
    {
        // 여기에서 바람 설정을 적용하는 로직을 구현합니다.
        // 예를 들어, 셰이더에 바람 정보를 전달하거나 관련 객체들을 업데이트합니다.
        //ApplyWindSettings(m_GlobalWindDir, windStrength, windFrequency, enableWind);
    }

    ImGui::End();
}

void CImgui_Manager::Fog_Editor()
{
    ImGui::Begin("Fog Settings", &m_bFogWindow);

    static CRenderer::FOG_DESC fogDesc = {};
    static bool autoUpdateFog = false;

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Color 1");
    if (ImGui::ColorEdit4("##FogColor1", (float*)&fogDesc.vFogColor))
    {
        if (autoUpdateFog)
            m_pGameInstance->Set_FogOption(fogDesc);
    }

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Color 2");
    if (ImGui::ColorEdit4("##FogColor2", (float*)&fogDesc.vFogColor2))
    {
        if (autoUpdateFog)
            m_pGameInstance->Set_FogOption(fogDesc);
    }

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Range");
    if (ImGui::SliderFloat("##FogRange", &fogDesc.fFogRange, 0.0f, 30000.0f, "%.1f"))
    {
        if (autoUpdateFog)
            m_pGameInstance->Set_FogOption(fogDesc);
    }

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Height Falloff");
    ImGui::SliderFloat("##HeightFalloff", &fogDesc.fFogHeightFalloff, 0.0f, 30.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Global Density");
    ImGui::SliderFloat("##GlobalDensity", &fogDesc.fFogGlobalDensity, 0.0f, 1.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Time Offset 1");
    ImGui::SliderFloat("##FogTimeOffset1", &fogDesc.fFogTimeOffset, 0.0f, 30.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Time Offset 2");
    ImGui::SliderFloat("##FogTimeOffset2", &fogDesc.fFogTimeOffset2, 0.0f, 30.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Intensity 1");
    ImGui::SliderFloat("##NoiseIntensity1", &fogDesc.fNoiseIntensity, 0.0f, 10.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Intensity 2");
    ImGui::SliderFloat("##NoiseIntensity2", &fogDesc.fNoiseIntensity2, 0.0f, 10.0f, "%.3f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Size 1");
    ImGui::SliderFloat("##NoiseSize1", &fogDesc.fNoiseSize, 0.0f, 0.1f, "%.6f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Size 2");
    ImGui::SliderFloat("##NoiseSize2", &fogDesc.fNoiseSize2, 0.0f, 0.1f, "%.6f");

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Fog Blend Factor");
    ImGui::SliderFloat("##FogBlendFactor", &fogDesc.fFogBlendFactor, 0.0f, 1.0f, "%.3f");

    ImGui::Checkbox("Auto Update Fog", &autoUpdateFog);

    if (ImGui::Button("Apply Fog Settings") || autoUpdateFog)
    {
        m_pGameInstance->Set_FogOption(fogDesc);
    }

    if (ImGui::Button("Close"))
        m_bFogWindow = false;

    ImGui::End();
}

void CImgui_Manager::Cloud_Editor()
{
    list<CGameObject*> clouds = m_pGameInstance->Get_GameObjects_Ref(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Clouds"));
    if (!clouds.empty())
    {
        CCloud* cloud = dynamic_cast<CCloud*>(clouds.front());
        if (cloud)
        {
            ImGui::Begin("Cloud Editor");
            // ShaderPath 설정
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shader Settings");
            static int currentShaderPath = 0;
            const char* shaderPaths[] = { "For Tool", "Texture", "Volumetric"}; // 예시 경로들
            if (ImGui::Combo("Shader Path", &currentShaderPath, shaderPaths, IM_ARRAYSIZE(shaderPaths)))
            {
                cloud->Set_ShaderPath((_uint)currentShaderPath);
            }

            // Volumetric 설정 (새로 추가된 부분)
            if (currentShaderPath == 2) // Volumetric shader 선택 시
            {
                ImGui::Separator();
                ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Volumetric Cloud Settings");

                // 구름 볼륨 크기 조절
                static float cloudVolumeSize = 100.0f; // 초기값 설정
                if (ImGui::SliderFloat("Cloud Volume Size", &cloudVolumeSize, 10.0f, 500.0f))
                {
                    cloud->Set_CloudVolumeSize(cloudVolumeSize);
                }

                // 구름 볼륨 오프셋 조절
                static float cloudVolumeOffset[3] = { 0.0f, 50.0f, 0.0f }; // 초기값 설정
                if (ImGui::SliderFloat3("Cloud Volume Offset", cloudVolumeOffset, -100.0f, 100.0f))
                {
                    cloud->Set_CloudVolumeOffset(XMFLOAT3(cloudVolumeOffset[0], cloudVolumeOffset[1], cloudVolumeOffset[2]));
                }
            }

            // 기존 구름 설정
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Cloud Settings");
            ImGui::SliderFloat("Cloud Density", &cloud->m_fCloudDensity, 0.000001f, 5.0f, "%.6f");
            ImGui::SliderFloat("Cloud Scale", &cloud->m_fCloudScale, 0.000001f, 0.01f, "%.6f");
            ImGui::SliderFloat("Cloud Speed", &cloud->m_fCloudSpeed, 0.01f, 1.0f, "%.6f");
            ImGui::SliderFloat("Cloud Height", &cloud->m_fCloudHeight, 50.0f, 200.0f);
            ImGui::ColorEdit3("Cloud Color", (float*)&cloud->m_vCloudColor);

            // Noise 설정
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Settings");

            // Perlin Noise 설정
            ImGui::Text("Perlin Noise");
            ImGui::SliderInt("Perlin Octaves", &cloud->m_iPerlinOctaves, 1, 10);
            ImGui::SliderFloat("Perlin Frequency", &cloud->m_fPerlinFrequency, 0.1f, 20.0f);
            ImGui::SliderFloat("Perlin Persistence", &cloud->m_fPerlinPersistence, 0.1f, 1.0f);
            ImGui::SliderFloat("Perlin Lacunarity", &cloud->m_fPerlinLacunarity, 1.0f, 4.0f);

            // Worley Noise 설정
            ImGui::Text("Worley Noise");
            ImGui::SliderFloat("Worley Frequency", &cloud->m_fWorleyFrequency, 0.1f, 20.0f);
            ImGui::SliderFloat("Worley Jitter", &cloud->m_fWorleyJitter, 0.0f, 1.0f);

            // Noise 혼합 설정
            ImGui::Text("Noise Mixing");
            ImGui::SliderFloat("Perlin-Worley Mix", &cloud->m_fPerlinWorleyMix, 0.0f, 1.0f);
            ImGui::SliderFloat("Noise Remap Lower", &cloud->m_fNoiseRemapLower, 0.0f, 1.0f);
            ImGui::SliderFloat("Noise Remap Upper", &cloud->m_fNoiseRemapUpper, 0.0f, 1.0f);

            // 적응형 샘플링 설정
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Sampling Settings");
            ImGui::SliderFloat("Coarse Step Size", &cloud->m_fCoarseStepSize, 0.1f, 10.0f);
            ImGui::SliderFloat("Fine Step Size", &cloud->m_fFineStepSize, 0.01f, 1.f);
            ImGui::SliderInt("Max Coarse Steps", &cloud->m_iMaxCoarseSteps, 10, 400);
            ImGui::SliderInt("Max Fine Steps", &cloud->m_iMaxFineSteps, 10, 400);
            ImGui::SliderFloat("Density Threshold", &cloud->m_fDensityThreshold, 0.001f, 0.1f);
            ImGui::SliderFloat("Alpha Threshold", &cloud->m_fAlphaThreshold, 0.9f, 0.999f);

            //리플렉션 최적화
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Reflection Optimization");
            ImGui::SliderFloat("Reflection Quality", &cloud->m_fReflectionQuality, 0.1f, 3.0f);
            ImGui::SliderFloat("Reflection Opacity", &cloud->m_fReflectionOpacity, 0.0f, 1.0f);
            ImGui::SliderFloat("Reflection Density Scale", &cloud->m_fReflectionDensityScale, 0.1f, 3.0f);

            // 반사 색상 (하늘색) 조절
            if (ImGui::ColorEdit4("Base Sky Color", (float*)&cloud->m_vBaseSkyColor))
            {
            }

            // 조명 설정
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Light Settings");

            // Gizmo 세팅
            float cameraView[16];
            float cameraProjection[16];
            memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
            memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

            // 조명 위치를 나타내는 행렬 생성
            XMFLOAT4X4 lightMatrix;
            XMStoreFloat4x4(&lightMatrix, XMMatrixTranslation(
                cloud->m_vLightPosition.x,
                cloud->m_vLightPosition.y,
                cloud->m_vLightPosition.z
            ));

            // Gizmo 조작
            if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
            {
                // 조작된 행렬에서 위치 추출
                XMFLOAT4 newPosition;
                XMStoreFloat4(&newPosition, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
                cloud->m_vLightPosition = XMFLOAT4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
            }

            // 조명 위치 수동 입력
            ImGui::InputFloat3("Light Position", (float*)&cloud->m_vLightPosition);

            // 조명 범위 설정
            ImGui::SliderFloat("Light Range", &cloud->m_fLightRange, 300.0f, 3000.0f);

            // 조명 색상 설정
            ImGui::ColorEdit3("Light Color", (float*)&cloud->m_vLightDiffuse);

            // 태양 방향 설정 (정규화된 방향 벡터)
            static float sunDirection[3] = { -1.0f, -1.0f, -1.0f };
            if (ImGui::SliderFloat3("Sun Direction", sunDirection, -1.0f, 1.0f))
            {
                // 정규화
                XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3((XMFLOAT3*)sunDirection));
                XMStoreFloat4(&cloud->m_vLightDir, vDir);
            }


            // 노이즈 텍스처 생성 및 저장 버튼
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Noise Texture Generation");

            static int textureSize[3] = { 128, 128, 128 }; // 기본 텍스처 크기
            ImGui::InputInt3("Texture Size (Width, Height, Depth)", textureSize);

            static char fileName[256] = "NoiseTexture.dds";
            ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));
            swprintf_s(m_filePath, MAX_PATH, L"../Bin/Resources/Textures/Noise/Cloud/%S", fileName);

            if (ImGui::Button("Generate and Save 3D Noise Texture"))
            {
                HRESULT hr = Create3DNoiseTexture(textureSize[0], textureSize[1], textureSize[2], DXGI_FORMAT_R8G8B8A8_UNORM);
                if (SUCCEEDED(hr))
                {
                    // 파일 경로 생성

                    //hr = Save3DTextureToDDS(m_pNoiseTextureView, filePath);
                    if (SUCCEEDED(hr))
                        MessageBox(NULL, L"3D Noise Texture saved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                    else
                        MessageBox(NULL, L"Failed to save 3D Noise Texture!", L"Error", MB_OK | MB_ICONERROR);
                }
                else
                {
                    MessageBox(NULL, L"Failed to generate 3D Noise Texture!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            ImGui::End();

        }
    }
}

HRESULT CImgui_Manager::Create3DNoiseTexture(UINT width, UINT height, UINT depth, DXGI_FORMAT format)
{
    // 텍스처 생성
    D3D11_TEXTURE3D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    std::vector<BYTE> noiseData(width * height * depth * 4);

    std::list<CGameObject*> clouds = m_pGameInstance->Get_GameObjects_Ref(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Clouds"));
    if (clouds.empty())
    {
        MessageBox(NULL, L"No cloud object found!", L"Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }
    CCloud* cloud = dynamic_cast<CCloud*>(clouds.front());
    if (!cloud)
    {
        MessageBox(NULL, L"Failed to cast to CCloud object!", L"Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    GenerateNoiseData(noiseData.data(), width, height, depth);

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = noiseData.data();
    initData.SysMemPitch = width * 4;
    initData.SysMemSlicePitch = width * height * 4;

    ID3D11Texture3D* pNoiseTexture = nullptr;
    HRESULT hr = m_pDevice->CreateTexture3D(&desc, &initData, &pNoiseTexture);
    if (FAILED(hr))
    {
        MessageBox(NULL, L"Failed to create 3D texture!", L"Error", MB_OK | MB_ICONERROR);
        return hr;
    }

    // 셰이더 리소스 뷰 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    srvDesc.Texture3D.MipLevels = 1;

    hr = m_pDevice->CreateShaderResourceView(pNoiseTexture, &srvDesc, &m_pNoiseTextureView);
    if (FAILED(hr))
    {
        Safe_Release(pNoiseTexture);
        MessageBox(NULL, L"Failed to create shader resource view!", L"Error", MB_OK | MB_ICONERROR);
        return hr;
    }

    // 텍스처 생성 후 DDS 파일로 저장
    if (SUCCEEDED(hr))
    {
        // DirectXTex의 ScratchImage로 텍스처 데이터 캡처
        DirectX::ScratchImage scratchImage;
        hr = DirectX::CaptureTexture(m_pDevice, m_pContext, pNoiseTexture, scratchImage);
        if (SUCCEEDED(hr))
        {
            // DDS 파일로 저장
            hr = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, m_filePath);
            if (FAILED(hr))
            {
                MessageBox(NULL, L"Failed to save 3D texture to DDS file!", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        else
        {
            MessageBox(NULL, L"Failed to capture texture data!", L"Error", MB_OK | MB_ICONERROR);
        }
    }

    Safe_Release(pNoiseTexture);
    return S_OK;
}

void CImgui_Manager::GenerateNoiseData(BYTE* data, UINT width, UINT height, UINT depth)
{
    std::list<CGameObject*> clouds = m_pGameInstance->Get_GameObjects_Ref(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Clouds"));
    CCloud* cloud = dynamic_cast<CCloud*>(clouds.front());
    float perlinFreq = cloud->m_fPerlinFrequency;
    int perlinOctaves = cloud->m_iPerlinOctaves;
    float worleyFreq = cloud->m_fWorleyFrequency;
    float perlinPersistence = cloud->m_fPerlinPersistence;
    float perlinLacunarity = cloud->m_fPerlinLacunarity;
    float worleyJitter = cloud->m_fWorleyJitter;

    auto hash33 = [](XMFLOAT3 p) -> XMFLOAT3 {
        XMUINT3 q;
        q.x = (UINT)(p.x) * 1597334673U;
        q.y = (UINT)(p.y) * 3812015801U;
        q.z = (UINT)(p.z) * 2798796415U;

        q.x = (q.x ^ q.y ^ q.z) * 1597334673U;
        q.y = (q.y ^ q.z ^ q.x) * 3812015801U;
        q.z = (q.z ^ q.x ^ q.y) * 2798796415U;

        const float invMaxUint = 1.0f / 4294967295.0f;
        return XMFLOAT3(
            -1.0f + 2.0f * (float)q.x * invMaxUint,
            -1.0f + 2.0f * (float)q.y * invMaxUint,
            -1.0f + 2.0f * (float)q.z * invMaxUint
        );
        };

    auto gradientNoise = [&](XMFLOAT3 x, float freq) -> float {
        XMFLOAT3 p = XMFLOAT3(std::floor(x.x), std::floor(x.y), std::floor(x.z));
        XMFLOAT3 w = XMFLOAT3(x.x - p.x, x.y - p.y, x.z - p.z);

        auto smoothstep = [](XMFLOAT3 t) -> XMFLOAT3 {
            return XMFLOAT3(
                t.x * t.x * t.x * (t.x * (t.x * 6.0f - 15.0f) + 10.0f),
                t.y * t.y * t.y * (t.y * (t.y * 6.0f - 15.0f) + 10.0f),
                t.z * t.z * t.z * (t.z * (t.z * 6.0f - 15.0f) + 10.0f)
            );
            };

        XMFLOAT3 u = smoothstep(w);

        auto fmod = [](XMFLOAT3 a, float b) -> XMFLOAT3 {
            return XMFLOAT3(fmodf(a.x, b), fmodf(a.y, b), fmodf(a.z, b));
            };

        XMFLOAT3 ga = hash33(fmod(XMFLOAT3(p.x + 0.0f, p.y + 0.0f, p.z + 0.0f), freq));
        XMFLOAT3 gb = hash33(fmod(XMFLOAT3(p.x + 1.0f, p.y + 0.0f, p.z + 0.0f), freq));
        XMFLOAT3 gc = hash33(fmod(XMFLOAT3(p.x + 0.0f, p.y + 1.0f, p.z + 0.0f), freq));
        XMFLOAT3 gd = hash33(fmod(XMFLOAT3(p.x + 1.0f, p.y + 1.0f, p.z + 0.0f), freq));
        XMFLOAT3 ge = hash33(fmod(XMFLOAT3(p.x + 0.0f, p.y + 0.0f, p.z + 1.0f), freq));
        XMFLOAT3 gf = hash33(fmod(XMFLOAT3(p.x + 1.0f, p.y + 0.0f, p.z + 1.0f), freq));
        XMFLOAT3 gg = hash33(fmod(XMFLOAT3(p.x + 0.0f, p.y + 1.0f, p.z + 1.0f), freq));
        XMFLOAT3 gh = hash33(fmod(XMFLOAT3(p.x + 1.0f, p.y + 1.0f, p.z + 1.0f), freq));

        float va = Dot(ga, XMFLOAT3(w.x - 0.0f, w.y - 0.0f, w.z - 0.0f));
        float vb = Dot(gb, XMFLOAT3(w.x - 1.0f, w.y - 0.0f, w.z - 0.0f));
        float vc = Dot(gc, XMFLOAT3(w.x - 0.0f, w.y - 1.0f, w.z - 0.0f));
        float vd = Dot(gd, XMFLOAT3(w.x - 1.0f, w.y - 1.0f, w.z - 0.0f));
        float ve = Dot(ge, XMFLOAT3(w.x - 0.0f, w.y - 0.0f, w.z - 1.0f));
        float vf = Dot(gf, XMFLOAT3(w.x - 1.0f, w.y - 0.0f, w.z - 1.0f));
        float vg = Dot(gg, XMFLOAT3(w.x - 0.0f, w.y - 1.0f, w.z - 1.0f));
        float vh = Dot(gh, XMFLOAT3(w.x - 1.0f, w.y - 1.0f, w.z - 1.0f));

        return va +
            u.x * (vb - va) +
            u.y * (vc - va) +
            u.z * (ve - va) +
            u.x * u.y * (va - vb - vc + vd) +
            u.y * u.z * (va - vc - ve + vg) +
            u.z * u.x * (va - vb - ve + vf) +
            u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
        };

    auto worleyNoise = [&](XMFLOAT3 uv, float freq) -> float {
        XMFLOAT3 id = Floor(uv);
        XMFLOAT3 p = Frac(uv);

        float minDist = 10000.0f;
        for (float x = -1.0f; x <= 1.0f; ++x)
        {
            for (float y = -1.0f; y <= 1.0f; ++y)
            {
                for (float z = -1.0f; z <= 1.0f; ++z)
                {
                    XMFLOAT3 offset(x, y, z);
                    XMFLOAT3 h = hash33(XMFLOAT3(fmodf(id.x + offset.x, freq), fmodf(id.y + offset.y, freq), fmodf(id.z + offset.z, freq)));
                    h = Add(Multiply(Subtract(h, XMFLOAT3(0.5f, 0.5f, 0.5f)), worleyJitter), XMFLOAT3(0.5f, 0.5f, 0.5f));
                    h = Add(h, offset);
                    XMFLOAT3 d = Subtract(p, h);
                    minDist = min(minDist, Dot(d, d));
                }
            }
        }

        return 1.0f - minDist;
        };

    auto perlinfbm = [&](XMFLOAT3 p, float freq, int octaves) -> float {
        float G = perlinPersistence;
        float amp = 1.0f;
        float noise = 0.0f;
        for (int i = 0; i < octaves; ++i)
        {
            noise += amp * gradientNoise(Multiply(p, freq), freq);
            freq *= perlinLacunarity;
            amp *= G;
        }
        return noise;
        };

    for (UINT z = 0; z < depth; ++z)
    {
        for (UINT y = 0; y < height; ++y)
        {
            for (UINT x = 0; x < width; ++x)
            {
                XMFLOAT3 position = {
                    (float)x / width,
                    (float)y / height,
                    (float)z / depth
                };

                float perlin = perlinfbm(position, perlinFreq, perlinOctaves);
                float worley = worleyNoise(Multiply(position, worleyFreq), worleyFreq);
                float detailNoise = gradientNoise(Multiply(position, worleyFreq * 4), worleyFreq * 4);

                // 노이즈 값을 0-1 범위로 정규화
                perlin = (perlin + 1.0f) * 0.5f;
                detailNoise = (detailNoise + 1.0f) * 0.5f;

                // 노이즈 값을 0-255 범위로 변환하여 저장
                UINT index = (z * width * height + y * width + x) * 4;
                data[index] = (BYTE)(saturate(perlin) * 255.0f);      // R: Perlin noise
                data[index + 1] = (BYTE)(saturate(worley) * 255.0f);  // G: Worley noise
                data[index + 2] = (BYTE)(saturate(detailNoise) * 255.0f); // B: Detail noise
                data[index + 3] = 255;                                // A: Fully opaque
            }
        }
    }
}

void CImgui_Manager::Setting_CreateObj_ListBox()
{
  
  
    {
        static int item_current_idx = 0;
        if (ImGui::BeginListBox("##Obj", ImVec2(300, 200)))
        {
            for (int n = 0; n < m_vecCreateObj.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(m_vecCreateObj[n], is_selected))
                {
                    item_current_idx = n;
                    m_iCreateObjIdx = item_current_idx;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }
}


void CImgui_Manager::Water_Editor()
{
    list<CGameObject*> waters = m_pGameInstance->Get_GameObjects_Ref(MYMapTool::LEVEL_GAMEPLAY, TEXT("Layer_Lagoon"));
    if (!waters.empty())
    {
        CLagoon* lagoon = dynamic_cast<CLagoon*>(waters.front());
        if (lagoon)
        {
            ImGui::Begin("Water Editor");

            // 조명 설정
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Light Settings");

            // Gizmo 세팅
            float cameraView[16];
            float cameraProjection[16];
            memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
            memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

            // 조명 위치를 나타내는 행렬 생성
            XMFLOAT4X4 lightMatrix;
            XMStoreFloat4x4(&lightMatrix, XMMatrixTranslation(
                lagoon->m_vLightPosition.x,
                lagoon->m_vLightPosition.y,
                lagoon->m_vLightPosition.z
            ));

            // Gizmo 조작
            if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
            {
                // 조작된 행렬에서 위치 추출
                XMFLOAT4 newPosition;
                XMStoreFloat4(&newPosition, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
                lagoon->m_vLightPosition = XMFLOAT4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
            }

            // 조명 위치 수동 입력
            ImGui::InputFloat3("Light Position", (float*)&lagoon->m_vLightPosition);

            if (ImGui::SliderFloat("Light Range", &lagoon->m_fLightRange, 100.f, 1000.f))
            {
                // Range가 변경됨
            }

            // 조명 방향 설정 (정규화된 방향 벡터)
            static float lightDirection[3] = { -1.0f, -1.0f, -1.0f };
            if (ImGui::SliderFloat3("Light Direction", lightDirection, -1.0f, 1.0f))
            {
                // 정규화
                XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3((XMFLOAT3*)lightDirection));
                XMStoreFloat4(&lagoon->m_vLightDir, vDir);
            }

            // 조명 색상 설정
            ImGui::ColorEdit3("Light Diffuse Color", (float*)&lagoon->m_vLightDiffuse);
            ImGui::ColorEdit3("Light Ambient Color", (float*)&lagoon->m_vLightAmbient);
            ImGui::ColorEdit3("Light Specular Color", (float*)&lagoon->m_vLightSpecular);
            ImGui::ColorEdit3("Mtrl Specular Color", (float*)&lagoon->m_vMtrlSpecular);

            ImGui::Separator();
            if (ImGui::SliderFloat("Bloom Threshold", &lagoon->m_fBloomThreshold, 0.01f, 1.f))
            {
            }
            if (ImGui::SliderFloat("Bloom Intensity", &lagoon->m_fBloomIntensity, 0.1f, 10.f, "%.6f"))
            {
            }

            // 물 표면 설정
            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Water Surface Settings");

            // FlowSpeed 조절
            ImGui::SliderFloat("Flow Speed", &lagoon->m_fFlowSpeed, 0.01f, 1.0f);

            if (ImGui::SliderFloat("Normal Strength 0", &lagoon->m_fNormalStrength0, 0.0f, 1.0f))
            {
                // 값이 직접 lagoon 객체의 멤버 변수에 저장됩니다.
            }
            if (ImGui::SliderFloat("Normal Strength 1", &lagoon->m_fNormalStrength1, 0.0f, 1.0f))
            {
                // 값이 직접 lagoon 객체의 멤버 변수에 저장됩니다.
            }
            if (ImGui::SliderFloat("Normal Strength 2", &lagoon->m_fNormalStrength2, 0.0f, 1.0f))
            {
                // 값이 직접 lagoon 객체의 멤버 변수에 저장됩니다.
            }
            // 프레넬 효과 강도 조절
            if (ImGui::SliderFloat("Fresnel Strength", &lagoon->m_fFresnelStrength, 0.0f, 1.0f))
            {
                // 값이 직접 lagoon 객체의 멤버 변수에 저장됩니다.
            }

            if (ImGui::SliderFloat("Roughness", &lagoon->m_fRoughness, 0.0f, 1.0f))
            {
            }

            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Alpha Settings");

            if (ImGui::SliderFloat("Water Alpha", &lagoon->m_fWaterAlpha, 0.0f, 1.0f))
            {
                // 알파값이 변경됨
            }

            if (ImGui::SliderFloat("Water Depth", &lagoon->m_fWaterDepth, 0.1f, 10.0f))
            {
                // 물 깊이가 변경됨
                // Caustic 강도와 알파값을 자동으로 조절
                lagoon->m_fCausticStrength = std::max(0.0f, 1.0f - lagoon->m_fWaterDepth / 5.0f);
                lagoon->m_fWaterAlpha = std::min(1.0f, lagoon->m_fWaterAlpha + lagoon->m_fWaterDepth / 10.0f);
            }

            // Caustic 강도는 읽기 전용으로 표시 (자동 조절되므로)

            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Foam Settings");

            // Foam Wave Frequency
            if (ImGui::SliderFloat("Foam Wave Frequency", &lagoon->m_fFoamWaveFrequency, 0.1f, 5.0f))
            {
                // 값이 변경됨
            }

            // Foam Wave Amplitude
            if (ImGui::SliderFloat("Foam Wave Amplitude", &lagoon->m_fFoamWaveAmplitude, 0.01f, 0.2f, "%.3f"))
            {
                // 값이 변경됨
            }

            // Foam Mask Scale
            if (ImGui::SliderFloat("Foam Mask Scale", &lagoon->m_fFoamMaskScale, 0.0f, 1.1f, "%.6f"))
            {
                // 값이 변경됨
            }

            // Foam Mask Speed
            if (ImGui::SliderFloat("Foam Mask Speed", &lagoon->m_fFoamMaskSpeed, 0.001f, 0.1f, "%.3f"))
            {
                // 값이 변경됨
            }

            // Foam Blend Strength
            if (ImGui::SliderFloat("Foam Blend Strength", &lagoon->m_fFoamBlendStrength, 0.0f, 1.0f))
            {
                // 값이 변경됨
            }

            // Foam Fresnel Strength
            if (ImGui::SliderFloat("Foam Fresnel Strength", &lagoon->m_fFoamFresnelStrength, 0.0f, 1.0f))
            {
                // 값이 변경됨
            }

            ImGui::Separator();
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Reflection Wave Settings");

            if (ImGui::SliderFloat("Caustic Strength", &lagoon->m_fCausticStrength, 0.0f, 1.0f))
            {
                // 값이 직접 lagoon 객체의 멤버 변수에 저장됩니다.
            }
            ImGui::Text("Caustic Strength: %.2f", lagoon->m_fCausticStrength);
       

            // 파동 강도 조절
            static float waveStrength = 0.02f; // 초기값, CLagoon에서 가져오는 것이 좋습니다
            static float waveFrequency = 5.0f; // 초기값, CLagoon에서 가져오는 것이 좋습니다
            static float waveTimeOffset = 1.0f; // 초기값, CLagoon에서 가져오는 것이 좋습니다
            static float fresnelPower = 5.0f; // 초기값, CLagoon에서 가져오는 것이 좋습니다
            static _uint CausticIdx = 0; // 초기값, CLagoon에서 가져오는 것이 좋습니다

            ImGui::SliderFloat("Wave Strength", &waveStrength, 0.0f, 30.f);
            // 파동 주파수 조절
            ImGui::SliderFloat("Wave Frequency", &waveFrequency, 0.0f, 0.1f, "%.6f");
            ImGui::SliderFloat("Wave Time OFfset", &waveTimeOffset, 1.f, 100.f, "%.3f");
            ImGui::SliderFloat("Fresnel Power", &fresnelPower, 0.1f, 30.0f);
 
            static int causticIdx = 0;

            if (ImGui::InputInt("Caustic Texture Index", &causticIdx))
            {
                // 입력 값을 0-6 범위로 제한
                causticIdx = max(0, min(causticIdx, 5));
                // lagoon 객체의 멤버 변수를 즉시 업데이트
                CausticIdx = causticIdx;
            }


            static bool autoUpdateWave = false;
            ImGui::Checkbox("Auto Update Wave", &autoUpdateWave);

            if (ImGui::Button("Wave Update") || autoUpdateWave)
            {
                m_pGameInstance->Set_ReflectionWave(waveStrength, waveFrequency, waveTimeOffset, fresnelPower, CausticIdx);
            }

            ImGui::End();
        }
    }
}

void CImgui_Manager::HBAO_Editor()
{
    ImGui::Begin("HBAO+ Settings", &m_bHBAOWindow);

    static GFSDK_SSAO_Parameters params = {};
    static bool autoUpdate = false;

    ImGui::Text("HBAO+ Params");

    bool paramsChanged = false;

    // Radius
    if (ImGui::SliderFloat("Radius", &params.Radius, 0.1f, 10.0f))
        paramsChanged = true;

    // Bias
    if (ImGui::SliderFloat("Bias", &params.Bias, 0.0f, 0.5f))
        paramsChanged = true;

    // Small Scale AO
    if (ImGui::SliderFloat("Small Scale AO", &params.SmallScaleAO, 0.0f, 2.0f))
        paramsChanged = true;

    // Large Scale AO
    if (ImGui::SliderFloat("Large Scale AO", &params.LargeScaleAO, 0.0f, 2.0f))
        paramsChanged = true;

    // Power Exponent
    if (ImGui::SliderFloat("Power Exponent", &params.PowerExponent, 1.0f, 4.0f))
        paramsChanged = true;

    // Step Count
    const char* stepCounts[] = { "4", "6", "8" };
    int stepCount = static_cast<int>(params.StepCount);
    if (ImGui::Combo("Step Count", &stepCount, stepCounts, IM_ARRAYSIZE(stepCounts)))
    {
        params.StepCount = static_cast<GFSDK_SSAO_StepCount>(stepCount);
        paramsChanged = true;
    }

    // Enable Dual Layer AO
    bool enableDualLayerAO = (params.EnableDualLayerAO != 0);
    if (ImGui::Checkbox("Enable Dual Layer AO", &enableDualLayerAO))
    {
        params.EnableDualLayerAO = enableDualLayerAO ? 1 : 0;
        paramsChanged = true;
    }

    // Blur 설정
    ImGui::Text("Blur Settings");
    bool blurEnable = (params.Blur.Enable != 0);
    if (ImGui::Checkbox("Enable Blur", &blurEnable))
    {
        params.Blur.Enable = blurEnable ? 1 : 0;
        paramsChanged = true;
    }

    if (blurEnable)
    {
        const char* blurRadii[] = { "2", "4", "8" };
        int blurRadius = static_cast<int>(params.Blur.Radius);
        if (ImGui::Combo("Blur Radius", &blurRadius, blurRadii, IM_ARRAYSIZE(blurRadii)))
        {
            params.Blur.Radius = static_cast<GFSDK_SSAO_BlurRadius>(blurRadius);
            paramsChanged = true;
        }

        if (ImGui::SliderFloat("Blur Sharpness", &params.Blur.Sharpness, 0.0f, 32.0f))
            paramsChanged = true;
    }

    // 오토 업데이트 체크박스
    ImGui::Checkbox("Auto Update", &autoUpdate);

    if (ImGui::Button("Apply") || (autoUpdate && paramsChanged))
    {
        m_pGameInstance->Set_HBAOParams(params);
    }

    ImGui::End();
}

void CImgui_Manager::Delete_Obj()
{
    //if (m_IsNaviMode) // Navigation 모드 On 일 때
    //{
    //    m_isNaviDelete = true;
    //}
    //else
    {
        CToolObj_Manager::GetInstance()->Delete_Obj(m_iCreateObjIdx); // Obj 제거

        // 생성 목록에서 이름 제거
        vector<_char*>::iterator iter = m_vecCreateObj.begin();

        for (int i = 0; i < m_iCreateObjIdx; ++i)
            ++iter;

        //이벤트 매니저로 삭제
        //Safe_Delete(*iter);
        //m_pGameInstance->Erase(iter);
        m_vecCreateObj.erase(iter);
    }
}

void CImgui_Manager::Add_vecCreateCellSize()
{
    _char szName[MAX_PATH] = "";
    sprintf(szName, "%d", m_vecCreateCell.size());
    m_vecCreateCell.emplace_back(szName);
}

void CImgui_Manager::Clear_vecCell()
{
    for (auto& iter : m_vecCreateCell)
    {
        Safe_Delete(iter);
    }
    m_vecCreateCell.clear();
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pDecalTexs);
    //for (_int i = 0; i < m_vecCreateObj.size(); ++i)
    //{
    //    Safe_Release(m_vecCreateObj[i]);
    //}

    //for (auto& iter : m_vecCreateObj)
    //{
    //    Safe_Delete(iter);
    //}
    //Safe_Delete_Array(m_vecCreateObj);
    //m_vecCreateObj.clear();

    //for (auto& iter : m_vecCreateCell)
    //{
    //    Safe_Delete(iter);
    //}
    //Safe_Delete_Array(m_vecCreateCell);
    //m_vecCreateCell.clear();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
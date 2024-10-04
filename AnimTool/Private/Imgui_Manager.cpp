#include "..\Default\framework.h"
#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "..\Public\imgui_impl_win32.h"
#include "..\Public\imgui_impl_dx11.h"
#include <Windows.h>
#include <string.h>
#include "ImGuizmo.h"
#include "ImSequencer.h"

#include "ToolObj_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

CImgui_Manager::CImgui_Manager() : m_bShowWindow(false), m_bShowAnotherWindow(false)
{
}

vector<string>* CImgui_Manager::Get_ApplySound(_uint iAnimIdx)
{
    map<_int, vector<string>>::iterator iter = m_mapApplySound.find(iAnimIdx);

    return &(*iter).second;
}

vector<string>* CImgui_Manager::Get_ApplyEffect(_uint iAnimIdx)
{
    map<_int, vector<string>>::iterator iter = m_mapApplyEffect.find(iAnimIdx);

    return &(*iter).second;
}

vector<_int>* CImgui_Manager::Get_EffectBoneIdx(_uint iAnimIdx)
{
    map<_int, vector<_int>>::iterator iter = m_mapBoneIdx.find(iAnimIdx);

    return &(*iter).second;
}

_bool CImgui_Manager::Get_isAble(_uint iAnimIdx)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("Type");
    if ("Able" == (*iterValue).second)
    {
        return true;
    }
    else
    {
        return false;
    }
}

_uint CImgui_Manager::Get_ColliderBoneIdx(_uint iAnimIdx)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("Position");
    
    vector<_char*>::iterator boneName = m_vecBone.begin();

    _int iBoneIdx = { 0 };

    // boneName에서 값 일치하는 것 찾아 Index 반환
    for (size_t i = 0; i < m_vecBone.size(); ++i)
    {
        if (*boneName == (*iterValue).second)
        {
            iBoneIdx = i;
            break;
        }
        else
            ++boneName;
    }

    return iBoneIdx;
}

_float CImgui_Manager::Get_StartKeyframe(_uint iAnimIdx)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("StartKeyFrame");

    return stof((*iterValue).second);
}

_float CImgui_Manager::Get_EndKeyframe(_uint iAnimIdx)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("EndKeyFrame");

    return stof((*iterValue).second);
}

void CImgui_Manager::Clear_vecAnim()
{
    m_iAnimIdx = 0;

    for (auto& pAnimName : m_vecAnim)
        Safe_Delete(pAnimName);

    m_vecAnim.clear();
}

void CImgui_Manager::Clear_vecChannel()
{
    m_iChannelIdx = 0;

    for (auto& pChannelName : m_vecChannel)
        Safe_Delete(pChannelName);

    m_vecChannel.clear();
}

void CImgui_Manager::Clear_vecBone()
{
    m_iBoneIdx = 0;

    for (auto& pBoneName : m_vecBone)
        Safe_Delete(pBoneName);

    m_vecBone.clear();
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    m_iFileIdx = 0;

    Initialize_vecSound();
    Initialize_vecEffect();
    Initialize_PartObj();

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    static _bool isLoop;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls



    if (m_bShowWindow) ImGui::ShowDemoWindow(&m_bShowWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static int counter = 0;

        ImGui::Begin("Anim Tool");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Select Model File");
        // 저장 및 로드
        ImGui::SameLine();
        if (ImGui::Button(" Save "))
        {
            CToolObj_Manager::GetInstance()->Save_Data();
        }
        ImGui::SameLine();
        if (ImGui::Button(" Load "))
        {
            CToolObj_Manager::GetInstance()->Load_Data();
        }
        // Type 선택
        Select_List_File();
        ImGui::SameLine();
        if (ImGui::Button(" Model Load "))
        {
            // 기존 모델 제거
            CToolObj_Manager::GetInstance()->Delete_AnimModel();

            // 모델 로드
            CToolObj_Manager::GetInstance()->Add_AnimModel(m_iFileIdx);

            Clear_vecChannel();
            Initialize_ApplySound();
            Initialize_ApplyEffect();
            Initialize_Collider(); // Clear 해야 하는 거 아닌지
        }

        ImGui::Text("");
        ImGui::Text("Animation List");
        Select_List_Anim();
        ImGui::SameLine();
        if (ImGui::Button(" Select \n Anim "))
        {
            Clear_vecChannel();

            CToolObj_Manager::GetInstance()->Setting_ChannelList();
        }

        if (ImGui::Button(" Play "))
            CToolObj_Manager::GetInstance()->Change_AnimState(0);
        ImGui::SameLine();
        if (ImGui::Button(" Pause "))
            CToolObj_Manager::GetInstance()->Change_AnimState(1);
        ImGui::SameLine();
        ImGui::Text("                   ");

        ImGui::SameLine();
        ImGui::Checkbox(" Loop ", &m_isLoop);

        if (!(CToolObj_Manager::GetInstance()->Get_isObjEmpty()))
        {
            CToolObj_Manager::GetInstance()->Set_Animation(m_iAnimIdx, m_isLoop); // m_CurrentKeyframesIndices를 초기화하는 코드가 들어있는 함수
        }
        ImGui::Text("");
        ImGui::Checkbox("Insert to Bone (Sound/Effect/Collider)", &m_bShowAnotherWindow); // Bone List Popup 출력

        ImGui::Text("");
        ImGui::Text(" Duration "); // Animation Duration
        ImGui::SameLine();
        Slider_AnimDuration();

        // Sound
        ImGui::Text("");
        ImGui::Text("------------------");
        Select_List_Sound();
        ImGui::Text("------------------");

        ImGui::Text("");
        Table_Channel();

        //ImGui::Checkbox("Demo Window", &m_bShowWindow);      // Edit bools storing our window open/close state

        ImGui::Text("");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (m_bShowAnotherWindow)
    {
        ImGui::Begin("Insert to Bone", &m_bShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::SetWindowSize("Object State", ImVec2(350, 300));
        ImGui::Text("Bone List");

        Select_List_Bone();

        ImGui::Checkbox("Isert PartObj", &m_bShowPartObjWindow); // PartObj 부착 기능

        // Effect
        ImGui::Text("");
        ImGui::Text("------------------");
        Select_List_Effect();

        // Collider
        ImGui::Text("");
        ImGui::Text("------------------");
        ImGui::Text("Collider");

        Insert_Collider();

        ImGui::Text("");
        if (ImGui::Button("Close"))
            m_bShowAnotherWindow = false;
        ImGui::End();
    }

    if (m_bShowPartObjWindow)
    {
        ImGui::Begin("Insert PartObj", &m_bShowPartObjWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::SetWindowSize("PartObj", ImVec2(200, 300));

        Select_List_PartObj();

        if (ImGui::Button("Add PartObj"))
        {
            CToolObj_Manager::GetInstance()->Add_PartObj(m_iPartObjIdx, m_iBoneIdx); // Bone Idx를 같이 넣어줌
            Add_PartObject();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete PartObj"))
        {
            CToolObj_Manager::GetInstance()->Delete_PartObj(m_iAddPartObjIdx);
            vector<string>::iterator partObj = m_vecAddPartObj.begin();
            for (size_t i = 0; i < m_iAddPartObjIdx; ++i)
                ++partObj;
            m_vecAddPartObj.erase(partObj);
        }

        // 생성된 PartObj List 필요
        ImGui::Text("");
        Select_List_AddPartObj();

        Setting_PartObj();

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
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        /*if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
            mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
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

        switch (mCurrentGizmoOperation)
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
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    // 화면 상단 카메라 돌아가는애
    //ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    if (useWindow)
    {
        ImGui::End();
        // 이거해보기 터지면
        ImGui::PopStyleColor(1);
    }
}

void CImgui_Manager::Delete_Obj()
{
    //if (m_IsNaviMode) // Navigation 모드 On 일 때
    //{
    //    m_isNaviDelete = true;
    //}
    //else
    //{
    //    CToolObj_Manager::GetInstance()->Delete_Obj(m_iCreateObjIdx); // Obj 제거

    //    // 생성 목록에서 이름 제거
    //    vector<_char*>::iterator iter = m_vecCreateObj.begin();

    //    for (int i = 0; i < m_iCreateObjIdx; ++i)
    //        ++iter;

    //    m_vecCreateObj.erase(iter);
    //}
}

void CImgui_Manager::Select_List_File()
{
    const char* ModelFile[] = { "Wander", "Homomculus", "Job_Mob", "Juggulus", "Malkhel",
                                "Mantari", "Npc_Choron", "Npc_Valnir", "Arrow_Jobmob", "Ghost"};
    static _int iSelectIdex = 0;
    ImGui::ListBox("###ModelFile", &iSelectIdex, ModelFile, IM_ARRAYSIZE(ModelFile));

    m_iFileIdx = iSelectIdex;
}

void CImgui_Manager::Select_List_Anim()
{
    // 선택한 Anim Model 종류에 따라 애니메이션 리스트 갱신
    static int anim_current = 0;

    if (ImGui::BeginListBox("###Anim", ImVec2(370, 100)))
    {
        for (int n = 0; n < m_vecAnim.size(); n++)
        {
            const bool is_selected = (anim_current == n);
            if (ImGui::Selectable(m_vecAnim[n], is_selected))
            {
                anim_current = n;
                m_iAnimIdx = anim_current;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Slider_AnimDuration()
{
    if (!(CToolObj_Manager::GetInstance()->Get_isObjEmpty()))
    {
        // Animation Duration을 Slider로 출력
        m_dMaxDuration = CToolObj_Manager::GetInstance()->Get_Duration(m_iAnimIdx);
        m_dCurrentPos = *(CToolObj_Manager::GetInstance()->Get_CurrentPos(m_iAnimIdx));
    }

    //static float fCurrentPos = static_cast<float>(m_dCurrentPos);
    _float fTemp = static_cast<float>(m_dCurrentPos);
    ImGui::SliderFloat("###Duration", &fTemp, 0, static_cast<float>(m_dMaxDuration));
}

void CImgui_Manager::Table_Channel()
{
    ImGui::Text("Current Key Frame");
    if (ImGui::BeginTable("Channel Keyframe", 4))
    {
        for (int row = 0; row < m_vecChannel.size() * 0.5; row++)
        {
            ImGui::TableNextRow();
            for (int column = 0; column < 4; column++)
            {
                ImGui::TableSetColumnIndex(column);
                
                int i = 0;
                if (0 == column % 2)
                {
                    if(0 == column)
                        i = 0;
                    else
                        i = 1;

                    ImGui::Text(m_vecChannel[row + (i * m_vecChannel.size() * 0.5)]);
                }
                else
                {
                    if (1 == column)
                        i = 0;
                    else
                        i = 1;

                    _uint iCurKeyframe = CToolObj_Manager::GetInstance()->Get_CurKeyFrame(m_iAnimIdx, row + (i * m_vecChannel.size() * 0.5));

                    ImGui::Text("\t %d", iCurKeyframe);
                }
            }
        }
        ImGui::EndTable();
    }
}

void CImgui_Manager::Delete_ChannelVec()
{
    /*for (vector<_char*>::iterator iter = m_vecChannel.begin();
        iter != m_vecChannel.end(); ++iter)
    {
        m_vecChannel.erase(iter);
    }*/
    m_vecChannel.clear();
}

void CImgui_Manager::Select_List_Bone()
{
    // Model의 전체 Bone List 출력
    static int bond_current = 0;

    if (ImGui::BeginListBox("###Bone", ImVec2(360, 100)))
    {
        for (int n = 0; n < m_vecBone.size(); n++)
        {
            const bool is_selected = (bond_current == n);
            if (ImGui::Selectable(m_vecBone[n], is_selected))
            {
                bond_current = n;
                m_iBoneIdx = bond_current;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Select_List_Sound()
{
    // List Box
    ImGui::Text("Select Sound File");

    static int sound_current = 0;

    if (ImGui::BeginListBox("###SoundFile", ImVec2(300, 100)))
    {
        for (int n = 0; n < m_vecSound.size(); n++)
        {
            const bool is_selected = (sound_current == n);
            if (ImGui::Selectable(m_vecSound[n], is_selected))
            {
                sound_current = n;
                m_iSoundIdx = sound_current;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    // Btn
    if (ImGui::Button(" Sound Play "))
    {
        // 추후 사운드 관련 추가
        Play_Sound();
    }

    ImGui::SameLine();
    if (ImGui::Button(" Sound Puase "))
    {
        CGameInstance::GetInstance()->StopAll();
    }
    ImGui::SameLine();
    if (ImGui::Button(" Sound Apply "))
    {
        Add_ApplySound();
    }

    Apply_List_Sound();

    if (ImGui::Button(" Sound Remove "))
    {
        Remove_ApplySound(); // 삭제 코드
    }
}

void CImgui_Manager::Apply_List_Sound()
{
    ImGui::Text("");
    ImGui::Text("Apply Sound");
    // vector<string> 만들어서 사용하는 것으로 변경 필요

    static int iApplySoundIdex = 0;

    if (m_mapApplySound.empty())
        return;

    map<_int, vector<string>>::iterator iter = m_mapApplySound.find(m_iAnimIdx);

    if (ImGui::BeginListBox("###ApplySound", ImVec2(300, 50)))
    {
        for (int n = 0; n < (*iter).second.size(); n++) // m_vecApplySound
        {
            const bool is_selected = (iApplySoundIdex == n);
            if (ImGui::Selectable((*iter).second[n].c_str(), is_selected))
            {
                iApplySoundIdex = n;
                m_iApplySoundIdx = iApplySoundIdex;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    m_iApplySoundIdx = iApplySoundIdex;
}

void CImgui_Manager::Select_List_Effect()
{
    ImGui::Text("Select Effect File");

    static int effect_current = 0;

    if (ImGui::BeginListBox("###EffectFile", ImVec2(200, 100)))
    {
        for (int n = 0; n < m_vecEffect.size(); n++)
        {
            const bool is_selected = (effect_current == n);
            if (ImGui::Selectable(m_vecEffect[n], is_selected))
            {
                effect_current = n;
                m_iEffectIdx = effect_current;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
    ImGui::SameLine();
    if (ImGui::Button(" Effect Apply "))
    {
        Add_ApplyEffect();
    }

    Apply_List_Effect();
    ImGui::SameLine();
    Apply_List_EffectPos();

    if (ImGui::Button(" Effect Remove "))
    {
        Remove_ApplyEffect(); // 삭제 코드
    }
}

void CImgui_Manager::Apply_List_Effect()
{
    ImGui::Text("");
    ImGui::Text("Apply Effect");

    if (m_mapApplyEffect.empty())
        return;

    static int iApplyEffectIdex = 0;

    map<_int, vector<string>>::iterator iter = m_mapApplyEffect.find(m_iAnimIdx);

    if (ImGui::BeginListBox("###ApplyEffect", ImVec2(200, 50)))
    {
        for (int n = 0; n < (*iter).second.size(); n++)
        {
            const bool is_selected = (iApplyEffectIdex == n);
            if (ImGui::Selectable((*iter).second[n].c_str(), is_selected))
            {
                iApplyEffectIdex = n;
                m_iApplyEffectIdx = iApplyEffectIdex;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Apply_List_EffectPos()
{
    static int iEffectBoneIdex = 0; // 얜 안 쓰것는디

    if (m_mapBoneIdx.empty())
        return;

    map<_int, vector<_int>>::iterator vecBoneIdx = m_mapBoneIdx.find(m_iAnimIdx);

    vector<_char*>::iterator boneName = m_vecBone.begin();

    if (ImGui::BeginListBox("###ApplyEffectPos", ImVec2(200, 50)))
    {
        for (int n = 0; n < (*vecBoneIdx).second.size(); n++)
        {
            const bool is_selected = (iEffectBoneIdex == n);

            if (ImGui::Selectable(boneName[(*vecBoneIdx).second[n]], is_selected))   
            {
                iEffectBoneIdex = n;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Initialize_vecSound()
{
    m_vecSound.emplace_back("Octopus_Bullet.mp3");
    m_vecSound.emplace_back("Quest.mp3");
}

void CImgui_Manager::Initialize_ApplySound()
{
    for (size_t i = 0; i < m_vecAnim.size(); ++i)
    {
        vector<string> vecApplySound;
        m_mapApplySound.emplace(i, vecApplySound);
    }
}

void CImgui_Manager::Initialize_vecEffect()
{
    m_vecEffect.emplace_back("TestEffect_1");
    m_vecEffect.emplace_back("TestEffect_2");
}

void CImgui_Manager::Initialize_ApplyEffect()
{
    for (size_t i = 0; i < m_vecAnim.size(); ++i)
    {
        vector<string> vecApplyEffect;
        m_mapApplyEffect.emplace(i, vecApplyEffect);

        vector<_int> vecBoneIdx;
        m_mapBoneIdx.emplace(i, vecBoneIdx);
    }
}

void CImgui_Manager::Add_ApplyEffect()
{
    vector<const _char*>::iterator iter = m_vecEffect.begin();

    for (size_t i = 0; i < m_iEffectIdx; ++i)
        ++iter;

    map < _int, vector<string>>::iterator applyeffect = m_mapApplyEffect.find(m_iAnimIdx);
    (*applyeffect).second.emplace_back(*iter);

    // 현재 선택되어 있는 Bone Index를 넣어주기
    map<_int, vector<_int>>::iterator boneIdx = m_mapBoneIdx.find(m_iAnimIdx);
    int iBoneIdx = m_iBoneIdx;
    (*boneIdx).second.emplace_back(iBoneIdx);
}

void CImgui_Manager::Remove_ApplyEffect()
{
    //m_iApplyEffectIdx 에 해당하는 값 지우기
    map<_int, vector<string>>::iterator applyeffect = m_mapApplyEffect.find(m_iAnimIdx);

    vector<string>::iterator iterApply = (*applyeffect).second.begin();

    for (size_t i = 0; i < m_iApplyEffectIdx; ++i)
        ++iterApply;

    (*applyeffect).second.erase(iterApply);

    // BoneIdx도 지워주기
    map<_int, vector<_int>>::iterator effectBoneIdx = m_mapBoneIdx.find(m_iAnimIdx);

    vector<_int>::iterator iterBone = (*effectBoneIdx).second.begin();

    for (size_t i = 0; i < m_iApplyEffectIdx; ++i)
        ++iterBone;

    (*effectBoneIdx).second.erase(iterBone);
}

void CImgui_Manager::Insert_Collider()
{
    // Type Radio Btn
    static int collider_type = 0;
    ImGui::RadioButton("None", &collider_type, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Able", &collider_type, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Disable", &collider_type, 2);
    ImGui::SameLine();

    // Apply Btn
    if (ImGui::Button(" Collider Apply "))
    {
        Apply_Collider(collider_type);
    }

    // keyframe Range
    ImGui::Text("Start Keyframe ");
    ImGui::SameLine();
    ImGui::SliderFloat("###StartKeyframe", &m_fStartKeyframe, 0, static_cast<float>(m_dMaxDuration));
    ImGui::Text("End Keyframe   ");
    ImGui::SameLine();
    ImGui::SliderFloat("###EndKeyframe", &m_fEndKeyframe, 0, static_cast<float>(m_dMaxDuration));

    // Apply Result (Text)
    ImGui::Text("Collider Apply Result");

    if (m_vecAnim.empty())
        return;

    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(m_iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("StartKeyFrame");

    iterValue = (*iter).second.find("Position");
    const _char* pPosition = (*iterValue).second.c_str(); // iter
    
    iterValue = (*iter).second.find("Type");
    const _char* pType = (*iterValue).second.c_str();
    
    iterValue = (*iter).second.find("StartKeyFrame");
    const _char* pStartKeyframe = (*iterValue).second.c_str();
    
    iterValue = (*iter).second.find("EndKeyFrame");
    const _char* pEndKeyframe = (*iterValue).second.c_str();

    ImGui::BulletText("Position(Bone) : ");
    ImGui::SameLine();
    ImGui::Text(pPosition);
    ImGui::BulletText("Type : ");
    ImGui::SameLine();
    ImGui::Text(pType);
    ImGui::BulletText("StartKeyFrame : ");
    ImGui::SameLine();
    ImGui::Text(pStartKeyframe);
    ImGui::BulletText("EndKeyFrame : ");
    ImGui::SameLine();
    ImGui::Text(pEndKeyframe);
}

void CImgui_Manager::Apply_Collider(_uint iType)
{
    switch (iType)
    {
    case 0: // None
        Clear_Collider();
        break;
    case 1: // Able
    case 2: // Disable
        Setting_Collider(iType);
        break;
    default:
        break;
    }
}

void CImgui_Manager::Initialize_Collider()
{
    for (size_t i = 0; i < m_vecAnim.size(); ++i)
    {
        map<string, string> mapColliderData;

        mapColliderData.emplace("Position", "");
        mapColliderData.emplace("Type", "");
        mapColliderData.emplace("StartKeyFrame", "");
        mapColliderData.emplace("EndKeyFrame", "");

        m_mapColliderMap.emplace(i, mapColliderData);
    }
}

void CImgui_Manager::Clear_Collider()
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.begin();

    for (size_t i = 0; i < m_iAnimIdx; ++i) // 현재 m_iAnimIdx에 해당하는 Collider 정보를 제거
        ++iter;

    for (auto& pColliderData : (*iter).second)
    {
        pColliderData.second = "";
    }
}

void CImgui_Manager::Setting_Collider(_uint iType)
{
    if (m_mapBoneIdx.empty())
        return;

    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.begin();

    for (size_t i = 0; i < m_iAnimIdx; ++i)
        ++iter;

    // StartKeyframe이 EndKeyframe보다 크면 저장 안 하는 예외 처리
    if (m_fStartKeyframe >= m_fEndKeyframe)
    {
        for (auto& pColliderData : (*iter).second)
        {
            pColliderData.second = "";
        }
        return;
    }

    // Bone Index
    vector<_char*>::iterator boneName = m_vecBone.begin();

    map<string, string>::iterator iterValue = (*iter).second.find("Position");
    (*iterValue).second = boneName[m_iBoneIdx];
    m_iCollideBoneIdx = m_iBoneIdx;

    iterValue = (*iter).second.find("Type");
    // Collider 활성화 여부 + Type
    if (1 == iType)
    {
        m_isCollderAble = true;
        (*iterValue).second = "Able";
    }
    else
    {
        m_isCollderAble = false;
        (*iterValue).second = "Disable";
    }

    // StartKeyframe
    iterValue = (*iter).second.find("StartKeyFrame");
    (*iterValue).second = to_string(m_fStartKeyframe);

    // EndKeyFrame
    iterValue = (*iter).second.find("EndKeyFrame");
    (*iterValue).second = to_string(m_fEndKeyframe);
}

void CImgui_Manager::Initialize_PartObj()
{
    m_vecPartObj.emplace_back("Gun");
}

void CImgui_Manager::Select_List_PartObj()
{
    static int partObj_current = 0;

    if (ImGui::BeginListBox("###PartObjList", ImVec2(300, 50)))
    {
        for (int n = 0; n < m_vecPartObj.size(); n++)
        {
            const bool is_selected = (partObj_current == n);
            if (ImGui::Selectable(m_vecPartObj[n].c_str(), is_selected))
            {
                partObj_current = n;
                m_iPartObjIdx = partObj_current;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Select_List_AddPartObj()
{
    ImGui::Text("AddPartObj List");

    static int addPartObj_current = 0;

    if (ImGui::BeginListBox("###AddPartObjList", ImVec2(300, 50)))
    {
        for (int n = 0; n < m_vecAddPartObj.size(); n++)
        {
            const bool is_selected = (addPartObj_current == n);
            if (ImGui::Selectable(m_vecAddPartObj[n].c_str(), is_selected))
            {
                addPartObj_current = n;
                m_iAddPartObjIdx = addPartObj_current;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void CImgui_Manager::Add_PartObject()
{
    vector<string>::iterator iter = m_vecPartObj.begin();

    for (size_t i = 0; i < m_iPartObjIdx; ++i)
        ++iter;

    m_vecAddPartObj.emplace_back((*iter));
}

void CImgui_Manager::Setting_PartObj()
{
    ImGui::Text("");
    ImGui::Text("Data Setting");

    ImGui::Text("Right Radian");
    ImGui::SameLine();
    static _float fRightRadian = 0.f;
    ImGui::InputFloat("###Right Radian", &fRightRadian);

    ImGui::Text("Look Radian");
    ImGui::SameLine();
    static _float fLookRadian = 0.f;
    ImGui::InputFloat("###Look Radian", &fLookRadian);

    ImGui::Text("Up Radian");
    ImGui::SameLine();
    static _float fUpRadian = 0.f;
    ImGui::InputFloat("###Up Radian", &fUpRadian);

    ImGui::Text("Vector Pos");
    ImGui::Text("X : ");
    ImGui::SameLine();
    static _float fVecX = 0.f;
    ImGui::InputFloat("###Vector PosX", &fVecX);
    ImGui::Text("Y : ");
    ImGui::SameLine();
    static _float fVecY = 0.f;
    ImGui::InputFloat("###Vector PosY", &fVecY);
    ImGui::Text("Z : ");
    ImGui::SameLine();
    static _float fVecZ = 0.f;
    ImGui::InputFloat("###Vector PosZ", &fVecZ);

    if (ImGui::Button("Data Apply"))
    {
        // 입력된 값들 PartObj에 적용해주기
        //Tool Obj 에 접근
        vector<CToolPartObj*>::iterator iter = (CToolObj_Manager::GetInstance()->Get_ToolPartObjs()).begin();
        for (size_t i = 0; i < m_iAddPartObjIdx; ++i)
            ++iter;

        (*iter)->Set_RightRadian(fRightRadian);
        (*iter)->Set_LookRadian(fLookRadian);
        (*iter)->Set_UpRadian(fUpRadian);
        _vector vecPos = XMVectorSet(fVecX, fVecY, fVecZ, 1.f);
        (*iter)->Set_Pos(vecPos);

        (*iter)->Set_Radian(fRightRadian, fLookRadian, fUpRadian);
    }
}

#pragma region LoadFunction
void CImgui_Manager::Load_ApplySound(_uint iAnimIdx, string pSoundFile)
{
    map<_int, vector<string>>::iterator applysound = m_mapApplySound.find(iAnimIdx);
    (*applysound).second.emplace_back(pSoundFile);
}

void CImgui_Manager::Load_ApplyEffect(_uint iAnimIdx, string pEffectFile)
{
    map<_int, vector<string>>::iterator applyeffect = m_mapApplyEffect.find(iAnimIdx);
    (*applyeffect).second.emplace_back(pEffectFile);
}

void CImgui_Manager::Load_EffectBoneIdx(_uint iAnimIdx, _int iBoneIdx)
{
    map<_int, vector<_int>>::iterator effectboneIdx = m_mapBoneIdx.find(iAnimIdx);
    (*effectboneIdx).second.emplace_back(iBoneIdx);
}

void CImgui_Manager::Load_isAble(_uint iAnimIdx, _bool isAble)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("Type");
    m_isCollderAble = isAble;
    if (m_isCollderAble)
    {
        (*iterValue).second = "Able";
    }
    else
    {
        (*iterValue).second = "Disable";
    }
}

void CImgui_Manager::Load_ColliderIdx(_uint iAnimIdx, _uint iColliderBoneIdx)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("Position");
    m_iCollideBoneIdx = iColliderBoneIdx;

    vector<_char*>::iterator boneName = m_vecBone.begin();
    (*iterValue).second = boneName[m_iBoneIdx];
}

void CImgui_Manager::Load_StartKeyframe(_uint iAnimIdx, _float fStartKeyframe)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("StartKeyFrame");
    m_fStartKeyframe = fStartKeyframe;
    (*iterValue).second = to_string(m_fStartKeyframe);
}

void CImgui_Manager::Load_EndKeyframe(_uint iAnimIdx, _float fEndKeyframe)
{
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("EndKeyFrame");
    m_fEndKeyframe = fEndKeyframe;
    (*iterValue).second = to_string(m_fEndKeyframe);
}

void CImgui_Manager::Load_PartObj(const _char* szName)
{
    m_vecAddPartObj.emplace_back(szName);
}

_bool CImgui_Manager::IsColliderSave(_uint iAnimIdx)
{
    // 예외 처리를 통해 Collider 저장 여부에 대한 값을 반환하는 함수
    // - map string value 중에 하나라도 "" 가 있으면 false
    map<_int, map<string, string>>::iterator iter = m_mapColliderMap.find(iAnimIdx);
    map<string, string>::iterator iterValue = (*iter).second.find("Type");

    if ((*iterValue).second == "")
    {
        return false;
    }
    else
    {
        return true;;
    }
}
#pragma endregion LoadFunction

void CImgui_Manager::Add_ApplySound()
{
    vector<const _char*>::iterator iter = m_vecSound.begin();

    for (size_t i = 0; i < m_iSoundIdx; ++i)
        ++iter;

    map<_int, vector<string>>::iterator applysound = m_mapApplySound.find(m_iAnimIdx);
    (*applysound).second.emplace_back(*iter);
}

void CImgui_Manager::Remove_ApplySound()
{
    vector<const _char*>::iterator iter = m_vecSound.begin();

    for (size_t i = 0; i < m_iSoundIdx; ++i)
        ++iter;

    map<_int, vector<string>>::iterator applysound = m_mapApplySound.find(m_iAnimIdx);
    
    //m_iApplySoundIdx 에 해당하는 값 지우기
    vector<string>::iterator iterApply = (*applysound).second.begin();

    for (size_t i = 0; i < m_iApplySoundIdx; ++i)
        ++iterApply;

    (*applysound).second.erase(iterApply);
}

void CImgui_Manager::Play_Sound()
{
    // Sound List의 선택 인덱스의 파일명을 재생
    vector<const _char*>::iterator iter = m_vecSound.begin();

    for (size_t i = 0; i < m_iSoundIdx; ++i)
        ++iter;

    _tchar wszSound[MAX_PATH] = L"";
    MultiByteToWideChar(CP_ACP, 0, (*iter), strlen((*iter)), wszSound, MAX_PATH);

    //CGameInstance::GetInstance()->PlaySound_Z(wszSound, SOUND_EFFECT, 1.f);
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    for (auto& pAnimName : m_vecAnim)
    {
        Safe_Delete(pAnimName);
    }
    m_vecAnim.clear();

    for (auto& pChannelName : m_vecChannel)
    {
        Safe_Delete(pChannelName);
    }
    m_vecChannel.clear();

    for (auto& pBoneName : m_vecBone)
    {
        Safe_Delete(pBoneName);
    }
    m_vecBone.clear();

    m_vecSound.clear();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#pragma once
#include "ImguiMgr.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "Particle_STrail.h"

#include "Andras.h"
#include "ElectricCylinder.h"

#include "HoverBoard.h"
#include "QuarterCamera.h"


CImguiMgr::CImguiMgr()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

wstring CImguiMgr::utf8_to_wstring(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

HRESULT CImguiMgr::Add_Texture_Prototype(const wstring& path, const wstring& name)
{
	if (m_pGameInstance->IsPrototype(m_pGameInstance->Get_CurrentLevel(), name) == true)
	{
		m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), name));
		return S_OK;
	}
	else
	{
		if (FAILED(m_pGameInstance->Add_Prototype(m_pGameInstance->Get_CurrentLevel(), name,
			CTexture::Create(m_pDevice, m_pContext, path, 1))))
			return E_FAIL;
		m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), name));
	}
	return S_OK;
}

HRESULT CImguiMgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::StyleColorsDark();

	m_pDevice = pDevice;
	m_pContext = pContext;

	ModelName.emplace_back("Wander");
	ModelName.emplace_back("Andras");
	ModelName.emplace_back("HoverBoard");
	ModelName.emplace_back("AndrasCut");
	for (auto& iter : bShow)
		iter = false;

	return S_OK;
}

ImTextureID CImguiMgr::DirectXTextureToImTextureID()
{
	ID3D11ShaderResourceView* SRV = m_pTextureCom->Get_Texture();
	return reinterpret_cast<ImTextureID>(SRV);
}

void CImguiMgr::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	
	Visible_Data();

	//코드
	//EffectTool();

	ImGui::Render();


	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImguiMgr::Tick(_float fTimiedelta)
{
}

void CImguiMgr::Model_Change()
{
	ImGui::Begin("Model_List");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
	if (ImGui::BeginListBox("Models", list_box_size))
	{
		for (int i = 0; i < ModelName.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(ModelName[i].c_str(), is_selected))
			{
				current_item = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < ModelName.size())
	{
		if (ImGui::Button("Change_Model", ButtonSize))
		{
			m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"));
			switch (current_item)
			{
			case 0:		//Wander
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_PlayerDummy"));
				break;
			case 1:		//Andras
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_Andras"));
				break;
			case 2:		//HoverBoard
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_HoverBoard"));
				break;
			case 3:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_CutSceneAndras"));
				break;
			default:
				break;
			}
			CurModel = current_item;
			TrailMat = nullptr;
		}
	}

	ImGui::End();
}

void CImguiMgr::Sound_Tool(_bool* Open)
{
	ImGui::SetNextWindowSize(ImVec2(800.f, 600.f));
	ImGui::Begin("Sound_Tool", Open, ImGuiWindowFlags_NoResize);
	ImVec2 ButtonSize = { 100,30 };

	static _bool  reverb = false;
	static _bool  Pause = false;
	static _bool  Echo = false;
	static _float fVolume = 0.5f;
	static _float fRoomsize = 0.f;
	static _float fDecayTime = 0.f;
	static _float wetMix = 0.f;
	static _float fPosition = 0.f;
	static _float fPitch = 1.f;

	static _float Echodelay = 0.f;
	static _float EchoWetLevel = 0.f;



	static string currentPath = "../../Client/Bin/Resources/Sound/";
	static string selectedFile = "";
	static string fullPath = "";
	static string curFileName = "";

	vector<std::string> files = GetFilesInDirectory(currentPath);
	if (ImGui::Button("..", ImVec2(50, 30))) {
		currentPath = std::filesystem::path(currentPath).parent_path().string();
	}

	if (ImGui::BeginListBox("##file_list", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
		for (const auto& file : files) {
			std::string fileFullPath = currentPath + "/" + file;

			// Check if it's a directory
			if (std::filesystem::is_directory(fileFullPath)) {
				if (ImGui::Selectable((file + "/").c_str(), selectedFile == file + "/")) {
					currentPath = fileFullPath;
					selectedFile = file + "/";
				}
			}
			else {
				if (ImGui::Selectable(file.c_str(), selectedFile == file)) {
					selectedFile = file;
					fullPath = fileFullPath;
				}
			}

			// Set focus on the selected item
			if (selectedFile == file || selectedFile == file + "/") {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (!selectedFile.empty() && Is_SoundFile(selectedFile)) 
	{
		if (ImGui::Button("Play", ButtonSize))
		{
			wstring wstr(selectedFile.begin(), selectedFile.end());
			const TCHAR* tchar_str = wstr.c_str();
			m_pGameInstance->Play_Effect_Sound(tchar_str, SOUND_EFFECT, fPosition, fPitch);
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause", ButtonSize))
		{
			Pause = !Pause;
			m_pGameInstance->Sound_Pause(SOUND_EFFECT, Pause);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", ButtonSize))
		{
			m_pGameInstance->StopAll();
		}
	}
	
	ImGui::SliderFloat("Volume", &fVolume, 0.0f, 1.0f, "Volume = %.3f");
	ImGui::SliderFloat("Position", &fPosition, 0.0f, 280.f, "Position = %.3f");
	ImGui::SliderFloat("Pitch", &fPitch, 0.f, 2.0f, "Pitch = %.3f");

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Reverb");

	ImGui::SliderFloat("Room Size", &fRoomsize, 0.0f, 10.f, "Room Size = %.3f");
	ImGui::SliderFloat("Decay Time", &fDecayTime, 0.0f, 10.f, "Decay Time = %.3f");
	ImGui::SliderFloat("Wet Mix", &wetMix, -10.f, 10.f, "Wet Mix = %.3f");

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Echo");

	ImGui::SliderFloat("Echo_Delay", &Echodelay, 0.0f, 2000.f, "Echo_Delay = %.3f");
	ImGui::SliderFloat("Echo Wet Level", &EchoWetLevel, -10.f, 10.f, "Echo Wet Level = %.3f");

	m_pGameInstance->Set_Echo_Param(Echodelay, EchoWetLevel);
	m_pGameInstance->Set_Reverb_Param(fRoomsize, fDecayTime, wetMix);
	m_pGameInstance->Set_Effect_Volume(fVolume);

	ImGui::Checkbox("Reverb", &reverb);
	if (reverb)
		m_pGameInstance->Enable_Reverb();
	else
		m_pGameInstance->Disable_Reverb();

	ImGui::Checkbox("Echo", &Echo);
	if (Echo)
		m_pGameInstance->Enable_Echo();
	else
		m_pGameInstance->Disable_Echo();

	ImGui::End();
}

void CImguiMgr::Change_Camera()
{
	CQuarterCamera* Camera = static_cast<CQuarterCamera*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(),
		TEXT("Layer_Camera")));
	Camera->Change_Camera();
}

void CImguiMgr::Visible_Data()
{
	ImGui::Begin("DATA");
	ImGui::Text("Frame : %f", ImGui::GetIO().Framerate);

	ImGui::Checkbox("Texture_FileSystem", &bShow[0]);
	if (bShow[0] == true)
		Load_Texture();

	ImGui::Checkbox("Particle_Tool", &bShow[1]);
	if (bShow[1] == true)
	{
		EffectTool_Rework();
	}


	ImGui::Checkbox("Trail_Tool", &bShow[2]);
	if (bShow[2] == true)
	{
		if (TrailMat == nullptr)
		{
			MSG_BOX("행렬을 먼저 대입해주세요");
			bShow[2] = false;
		}
		else
		{
			Trail_Tool();
		}
	}
	ImGui::Checkbox("FrameTextureTool", &bShow[3]);
	if (bShow[3] == true)
	{
		FrameTextureTool();
	}

	ImGui::Checkbox("SwordTrailTool", &bShow[4]);
	if(bShow[4] == true)
		SwordTrail_Tool();

	ImGui::Checkbox("DistortionTool", &bShow[5]);
	if (bShow[5] == true)
		Distortion_Tool();

	ImGui::Checkbox("ElectronicTool", &bShow[6]);
	if (bShow[6] == true)
		Electron_Tool();

	ImGui::Checkbox("Fire Tool", &bShow[7]);
	if (bShow[7] == true)
		FireTool();

	ImGui::Checkbox("Lazer_Tool", &bShow[8]);
	if (bShow[8] == true)
		Lazer_Tool();

	ImGui::Checkbox("Tornado_Tool", &bShow[9]);
	if (bShow[9] == true)
		Tornado_Tool();

	ImGui::Checkbox("FirePillar_Tool", &bShow[10]);
	if (bShow[10] == true)
		FirePillarTool();

	ImGui::Checkbox("Heal_Tool", &bShow[11]);
	if (bShow[11] == true)
		HealEffectTool();

	ImGui::Checkbox("Skill_Tool", &bShow[12]);
	if (bShow[12] == true)
		SwingEffectTool();

	ImGui::Checkbox("Model_Change", &bShow[13]);
	if (bShow[13] == true)
		Model_Change();

	ImGui::Checkbox("SoundTool", &bShow[14]);
	if (bShow[14] == true)
		Sound_Tool(&bShow[14]);
	
	ImGui::Checkbox("MeteorTool", &bShow[15]);
	if (bShow[15] == true)
		Meteor_Tool(&bShow[15]);

	ImGui::Checkbox("PhysXTool", &bShow[16]);
	if (bShow[16] == true)
		PhysX_Particle_Tool(&bShow[16]);

	ImGui::Checkbox("HedgeHogTool", &bShow[17]);
	if (bShow[17] == true)
		Hedgehog_Tool(&bShow[17]);

	ImGui::Checkbox("GroundSlashTool", &bShow[18]);
	if (bShow[18] == true)
		GroundSlash_Tool(&bShow[18]);

	ImGui::Checkbox("HammerSpawnTool", &bShow[19]);
	if (bShow[19] == true)
		HammerSpawn_Tool(&bShow[19]);

	ImGui::Checkbox("Shield_Tool", &bShow[20]);
	if (bShow[20] == true)
		Shield_Tool(&bShow[20]);

	ImGui::Checkbox("FireFly_Tool", &bShow[21]);
	if (bShow[21] == true)
		FireFly_Tool(&bShow[21]);

	ImGui::Checkbox("BlackHole_Tool", &bShow[22]);
	if (bShow[22] == true)
		BlackHole_Tool(&bShow[22]);
	
	ImGui::Checkbox("WellCylinder_Tool", &bShow[23]);
	if (bShow[23] == true)
		WellCylinder_Tool(&bShow[23]);

	ImGui::Checkbox("Magic_Cast_Tool", &bShow[24]);
	if (bShow[24] == true)
		Magic_Cast_Tool(&bShow[24]);

	ImGui::Checkbox("AndrasCutsceneTool", &bShow[25]);
	if (bShow[25] == true)
		Andras_CutScene_Tool(&bShow[25]);
	

	if(ImGui::Button("Change_Camera"))
		Change_Camera();


	if (ImGui::Button("Bind_Sword_Matrix"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_WeaponMat();
			break;
		}
		case 1:
		{
			break;
		}
		default:
			break;
		}

	}
	if (ImGui::Button("Bind_Body_Matrix"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_WorldMat();
			break;
		}
		case 1:
		{
			CAndras* Andras = static_cast<CAndras*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = Andras->Get_WorldMat();
			break;
		}
		case 2:
		{
			CHoverBoard* HoverBoard = static_cast<CHoverBoard*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = HoverBoard->Get_WorldMat();
			break;
		}
		default:
			break;
		}

	}
	if (ImGui::Button("Bind_Head"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_HeadMAt();
			break;
		}
		case 1:
		{
			CAndras* Andras = static_cast<CAndras*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = Andras->Get_HeadMat();
			break;
		}
		default:
			break;
		}

	}
	ImGui::End();
}

void CImguiMgr::EffectTool_Rework()
{
	ImGui::Begin("Effect_Editor");

	static CParticle::PARTICLEDESC parentsDesc{};
	static CParticleMesh::PARTICLEMESH MeshDesc{};
	static CParticle_Point::PARTICLEPOINT PointDesc{};
	static CParticle_Rect::PARTICLERECT RectDesc{};
	static PARTICLETYPE eParticleType = PART_POINT;
	

	if (ChangedDesc)
	{
		switch (eParticleType)
		{
		case Effect::PART_POINT:
			PointDesc = *((CParticle_Point::PARTICLEPOINT*)Variants);
			parentsDesc = PointDesc.SuperDesc;
			break;
		case Effect::PART_MESH:
			MeshDesc = *((CParticleMesh::PARTICLEMESH*)Variants);
			parentsDesc = MeshDesc.SuperDesc;
			break;
		case Effect::PART_RECT:
			RectDesc = *((CParticle_Rect::PARTICLERECT*)Variants);
			parentsDesc = RectDesc.SuperDesc;
			break;
		default:
			break;
		}
		ChangedDesc = false;
	}
	else
	{
		PointDesc.Texture = m_pTextureProtoName;
		PointDesc.TexturePath = m_pTextureFilePath;
		RectDesc.Texture = m_pTextureProtoName;
		RectDesc.TexturePath = m_pTextureFilePath;
	}




	MeshDesc.particleType = PART_MESH;
	PointDesc.particleType = PART_POINT;
	RectDesc.particleType = PART_RECT;

#pragma region PARTICLEDESC

	if (ImGui::RadioButton("Point", eParticleType == PART_POINT))
		eParticleType = PART_POINT;
	ImGui::SameLine();
	if (ImGui::RadioButton("Mesh", eParticleType == PART_MESH))
		eParticleType = PART_MESH;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rect", eParticleType == PART_RECT))
		eParticleType = PART_RECT;

	if (eParticleType == PART_MESH)
	{
		CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Mesh_Type");
		if (ImGui::RadioButton("Cube", MeshDesc.eModelType == EFFECTMODELTYPE::CUBE))
			MeshDesc.eModelType = EFFECTMODELTYPE::CUBE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Circle", MeshDesc.eModelType == EFFECTMODELTYPE::CIRCLE))
			MeshDesc.eModelType = EFFECTMODELTYPE::CIRCLE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Slash", MeshDesc.eModelType == EFFECTMODELTYPE::SLASH))
			MeshDesc.eModelType = EFFECTMODELTYPE::SLASH;
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf0", MeshDesc.eModelType == EFFECTMODELTYPE::LEAF0))
			MeshDesc.eModelType = EFFECTMODELTYPE::LEAF0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf1", MeshDesc.eModelType == EFFECTMODELTYPE::LEAF1))
			MeshDesc.eModelType = EFFECTMODELTYPE::LEAF1;

		if (ImGui::RadioButton("Blade", MeshDesc.eModelType == EFFECTMODELTYPE::BLADE_SLASH))
			MeshDesc.eModelType = EFFECTMODELTYPE::BLADE_SLASH;
		ImGui::SameLine();
		if (ImGui::RadioButton("Blade_Long", MeshDesc.eModelType == EFFECTMODELTYPE::BLADE_SLASH_LONG))
			MeshDesc.eModelType = EFFECTMODELTYPE::BLADE_SLASH_LONG;
		ImGui::SameLine();
		if (ImGui::RadioButton("Grass", MeshDesc.eModelType == EFFECTMODELTYPE::GRASS))
			MeshDesc.eModelType = EFFECTMODELTYPE::GRASS;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock1", MeshDesc.eModelType == EFFECTMODELTYPE::ROCK0))
			MeshDesc.eModelType = EFFECTMODELTYPE::ROCK0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock2", MeshDesc.eModelType == EFFECTMODELTYPE::ROCK1))
			MeshDesc.eModelType = EFFECTMODELTYPE::ROCK1;

		if (ImGui::RadioButton("Needle", MeshDesc.eModelType == EFFECTMODELTYPE::NEEDLE))
			MeshDesc.eModelType = EFFECTMODELTYPE::NEEDLE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Bubble", MeshDesc.eModelType == EFFECTMODELTYPE::BUBBLE))
			MeshDesc.eModelType = EFFECTMODELTYPE::BUBBLE;

	}

	ImGui::Checkbox("Bloom", &parentsDesc.IsBloom);
	ImGui::SameLine();
	ImGui::Checkbox("Blur", &parentsDesc.IsBlur);
	if (parentsDesc.IsBloom == true)
	{
		ImGui::InputFloat("BloomPower", &parentsDesc.fBlurPower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&parentsDesc.vBloomColor));
	}
	ImGui::Checkbox("Desolve", &parentsDesc.Desolve);
	if (parentsDesc.Desolve == true)
	{
		if (ImGui::InputInt("DesolveNumber", &parentsDesc.DesolveNum))
		{
			if (parentsDesc.DesolveNum < 0)
				parentsDesc.DesolveNum = 0;
			if (parentsDesc.DesolveNum > 45)
				parentsDesc.DesolveNum = 45;
		}
		ImGui::ColorEdit3("DesolveColor", reinterpret_cast<float*>(&parentsDesc.vDesolveColor));
		ImGui::InputFloat("DesolveLength", &parentsDesc.fDesolveLength);
	}
	ImGui::Checkbox("ColorMapping", &parentsDesc.IsColor);
	if (parentsDesc.IsColor == true)
	{
		ImGui::ColorEdit3("Start_Color", reinterpret_cast<float*>(&parentsDesc.vStartColor));
		ImGui::ColorEdit3("End_Color", reinterpret_cast<float*>(&parentsDesc.vEndColor));
	}
	ImGui::Checkbox("AlphaLerp", &parentsDesc.IsAlpha);
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&parentsDesc.vStartPos));

#pragma region FUNCTYPE
	if (ImGui::RadioButton("Spread", parentsDesc.eType == SPREAD))
	{
		parentsDesc.eType = SPREAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Drop", parentsDesc.eType == DROP))
	{
		parentsDesc.eType = DROP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOut", parentsDesc.eType == GROWOUT))
	{
		parentsDesc.eType = GROWOUT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Tornado", parentsDesc.eType == TORNADO))
	{
		parentsDesc.eType = TORNADO;
	}
	if (ImGui::RadioButton("Spread_SizeUp", parentsDesc.eType == SPREAD_SIZEUP))
	{
		parentsDesc.eType = SPREAD_SIZEUP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_NonRotation", parentsDesc.eType == SPREAD_NONROTATION))
	{
		parentsDesc.eType = SPREAD_NONROTATION;
	}


	if (ImGui::RadioButton("Spread_SpeedDown", parentsDesc.eType == SPREAD_SPEED_DOWN))
	{
		parentsDesc.eType = SPREAD_SPEED_DOWN;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("SlashEffect", parentsDesc.eType == SLASH_EFFECT))
	{
		parentsDesc.eType = SLASH_EFFECT;
	}
	if (ImGui::RadioButton("Spread_SpeedDown_SizeUp", parentsDesc.eType == SPREAD_SPEED_DOWN_SIZE_UP))
	{
		parentsDesc.eType = SPREAD_SPEED_DOWN_SIZE_UP;
	}
	if (ImGui::RadioButton("Gather", parentsDesc.eType == GATHER))
	{
		parentsDesc.eType = GATHER;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Extinction", parentsDesc.eType == EXTINCTION))
	{
		parentsDesc.eType = EXTINCTION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOutY", parentsDesc.eType == GROWOUTY))
	{
		parentsDesc.eType = GROWOUTY;
	}
	if (ImGui::RadioButton("GrowOutSpeedDown", parentsDesc.eType == GROWOUT_SPEEDDOWN))
	{
		parentsDesc.eType = GROWOUT_SPEEDDOWN;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Leaf_Fall", parentsDesc.eType == LEAF_FALL))
	{
		parentsDesc.eType = LEAF_FALL;
	}

	if (ImGui::RadioButton("Spiral_Gather", parentsDesc.eType == SPIRAL_EXTINCTION))
	{
		parentsDesc.eType = SPIRAL_EXTINCTION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spiral_Spread", parentsDesc.eType == SPIRAL_SPERAD))
	{
		parentsDesc.eType = SPIRAL_SPERAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("LenzFlare", parentsDesc.eType == LENZ_FLARE))
	{
		parentsDesc.eType = LENZ_FLARE;
	}
	if (ImGui::RadioButton("Blow", parentsDesc.eType == BLOW))
	{
		parentsDesc.eType = BLOW;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Up_Slowly", parentsDesc.eType == UP_TO_STOP))
	{
		parentsDesc.eType = UP_TO_STOP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("OnlyUp", parentsDesc.eType == ONLY_UP))
	{
		parentsDesc.eType = ONLY_UP;
	}

#pragma endregion FUNCTYPE
	
#pragma endregion PARTICLEDESC

#pragma region INSTANCEDESC
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &parentsDesc.InstanceDesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vOffsetPos));
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vPivotPos));
	ImGui::InputFloat3("Range", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vRange));
	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vSize));
	ImGui::InputFloat2("Speed", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vSpeed));
	ImGui::InputFloat2("LifeTime", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vLifeTime));
	ImGui::InputFloat2("Gravity", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vGravity));
	ImGui::Checkbox("IsLoop", &parentsDesc.InstanceDesc.isLoop);
#pragma endregion INSTANCEDESC

	void* value2 = nullptr;
	switch (eParticleType)
	{
	case PARTICLETYPE::PART_POINT:
		PointDesc.SuperDesc = parentsDesc;
		value2 = &PointDesc;
		break;
	case PARTICLETYPE::PART_MESH:
		MeshDesc.SuperDesc = parentsDesc;
		value2 = &MeshDesc;
		break;
	case PARTICLETYPE::PART_RECT:
		RectDesc.SuperDesc = parentsDesc;
		value2 = &RectDesc;
		break;
	default:
		break;
	}

#pragma region BUTTON
	if (ImGui::Button("Add", ImVec2(100, 30)))
	{
		switch (eParticleType)
		{
		case PARTICLETYPE::PART_POINT:
		{
			if (PointDesc.Texture == TEXT("") || PointDesc.TexturePath == TEXT(""))
				MSG_BOX("텍스쳐를 먼저 선택해주세요");
			else
			{
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticlePoint"), &PointDesc);
			}
			break;
		}
		case PARTICLETYPE::PART_MESH:
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
				TEXT("Prototype_GameObject_ParticleMesh"), &MeshDesc);
			break;
		case PARTICLETYPE::PART_RECT:
		{
			if (RectDesc.Texture == TEXT("") || RectDesc.TexturePath == TEXT(""))
				MSG_BOX("텍스쳐를 먼저 선택해주세요");
			else
			{
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleRect"), &RectDesc);
			}
			break;
		}
		default:
			break;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", ImVec2(100, 30)))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"));
	}

	static char ParticleNames[256] = "";
	ImGui::InputText("Name", ParticleNames, IM_ARRAYSIZE(ParticleNames));

	if (ImGui::Button("PushBack_List", ImVec2(200, 30)))		
	{
		if (ParticleNames[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			switch (eParticleType)
			{
			case Effect::PART_POINT:
				Store_Particles(ParticleNames, eParticleType, &PointDesc);
				break;
			case Effect::PART_MESH:
				Store_Particles(ParticleNames, eParticleType, &MeshDesc);
				break;
			case Effect::PART_RECT:
				Store_Particles(ParticleNames, eParticleType, &RectDesc);
				break;
			default:
				MSG_BOX("형식이 잘못되었습니다.");
				break;
			}
		}
	}

	if (ImGui::Button("Save", ImVec2(200, 30)))
	{
		if (FAILED(Save_Particles()))
			MSG_BOX("Failed_Save");
		else
			MSG_BOX("Succeed_Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(200, 30)))
	{
		if (FAILED(Load_Particles()))
			MSG_BOX("Failed_Load");
		else
			MSG_BOX("Succeed_Load");
	}
#pragma endregion BUTTON

	
	eParticleType = ParticleListBox(eParticleType, &Variants, value2);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Particles(char* Name, PARTICLETYPE type, void* pValue)
{
	string sName = Name;

	switch (type)
	{
	case Effect::PART_POINT:
	{
		CParticle_Point::PARTICLEPOINT* PointDesc = new CParticle_Point::PARTICLEPOINT;
		*PointDesc = *((CParticle_Point::PARTICLEPOINT*)pValue);
		m_Types.emplace_back(make_pair(PART_POINT, PointDesc));
		break;
	}
	case Effect::PART_MESH:
	{
		CParticleMesh::PARTICLEMESH* MeshDesc = new CParticleMesh::PARTICLEMESH;
		*MeshDesc = *((CParticleMesh::PARTICLEMESH*)pValue);
		m_Types.emplace_back(make_pair(PART_MESH, MeshDesc));
		break;
	}
	case Effect::PART_RECT:
	{
		CParticle_Rect::PARTICLERECT* RectDesc = new CParticle_Rect::PARTICLERECT;
		*RectDesc = *((CParticle_Rect::PARTICLERECT*)pValue);
		m_Types.emplace_back(make_pair(PART_RECT, RectDesc));
		break;
	}
	default:
		break;
	}
	ParticleNames.emplace_back(sName);

	return S_OK;
}

PARTICLETYPE CImguiMgr::ParticleListBox(PARTICLETYPE type, void** pValue, void* pValue2)
{
	//if (m_Particles.size() < 1)
	//	return type;
	if (m_Types.size() < 1)
		return type;


	if (m_Types.size() != ParticleNames.size())
	{
		MSG_BOX("Size Error");
		return type;
	}
	ImGui::Begin("Particle_List_Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
	if (ImGui::BeginListBox("Particle", list_box_size))
	{
		for (int i = 0; i < ParticleNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(ParticleNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < m_Types.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{	
			switch (m_Types[current_item].first)
			{
			case PART_POINT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticlePoint"), m_Types[current_item].second);
				break;
			case PART_MESH:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleMesh"), m_Types[current_item].second);
				break;
			case PART_RECT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleRect"), m_Types[current_item].second);
				break;
			default:
				break;
			}
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*pValue = m_Types[current_item].second;
			ChangedDesc = true;
			ImGui::End();
			return m_Types[current_item].first;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			if (m_Types[current_item].first != type)
				MSG_BOX("다른 타입은 수정할 수 없습니다.");
			else
			{
				switch (type)
				{
				case Effect::PART_POINT:
				{
					Erase_Particle(current_item);
					CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pValue2);
					m_Types[current_item].second = new CParticle_Point::PARTICLEPOINT(val);
					break;
				}
				case Effect::PART_MESH:
				{
					Erase_Particle(current_item);
					CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pValue2);
					m_Types[current_item].second = new CParticleMesh::PARTICLEMESH(val);
					break;
				}
				case Effect::PART_RECT:
				{
					Erase_Particle(current_item);
					CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pValue2);
					m_Types[current_item].second = new CParticle_Rect::PARTICLERECT(val);
					break;
				}
				default:
					break;
				}
			}
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			Erase_Particle(current_item);
			m_Types.erase(m_Types.begin() + current_item);
			ParticleNames.erase(ParticleNames.begin() + current_item);

			if (current_item >= m_Types.size())
				current_item = m_Types.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (int i = 0; i < m_Types.size(); ++i)
				Erase_Particle(i);

			m_Types.clear();
			//m_Particles.clear();
			ParticleNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();

	return type;
}

HRESULT CImguiMgr::Save_Particles()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Particles.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Types.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& pair : m_Types)
	{
		PARTICLETYPE type = pair.first;
		file.write((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case Effect::PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pair.second);
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			save_wstring_to_stream(val.Texture, file);
			save_wstring_to_stream(val.TexturePath, file);
			break;
		}
		case Effect::PART_MESH:
		{
			CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pair.second);
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			file.write((char*)&val.eModelType, sizeof(EFFECTMODELTYPE));
			break;
		}
		case Effect::PART_RECT:
		{
			CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pair.second);
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			save_wstring_to_stream(val.Texture, file);
			save_wstring_to_stream(val.TexturePath, file);
			break;
		}
		}
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Particle.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : ParticleNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/ParticleIndex.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < ParticleNames.size(); ++i)
	{
		NumberFile << i << ". " << ParticleNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Particles()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Particles.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (int i = 0; i < m_Types.size(); ++i)
		Erase_Particle(i);
	m_Types.clear();
	ParticleNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		PARTICLETYPE type;
		inFile.read((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT* Arg = new CParticle_Point::PARTICLEPOINT;
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_POINT;
			m_Types.emplace_back(make_pair(PART_POINT, Arg));
			
			break;
		}
		case PART_MESH:
		{
			CParticleMesh::PARTICLEMESH* Arg = new CParticleMesh::PARTICLEMESH();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			inFile.read((char*)&Arg->eModelType, sizeof(EFFECTMODELTYPE));
			Arg->particleType = PART_MESH;
			m_Types.emplace_back(make_pair(PART_MESH, Arg));
			break;
		}
		case PART_RECT:
		{
			CParticle_Rect::PARTICLERECT* Arg = new CParticle_Rect::PARTICLERECT();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_RECT;
			m_Types.emplace_back(make_pair(PART_RECT, Arg));
			break;
		}
		}
	}

	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Particle.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		ParticleNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::Load_Texture()
{
	ImGui::Begin("File Browser");

	static std::string currentPath = "../../Client/Bin/Resources/Textures/";
	static std::string selectedFile = "";
	static std::string fullPath = "";
	std::vector<std::string> files = GetFilesInDirectory(currentPath);

	if (ImGui::Button("..", ImVec2(50,30))) {
		currentPath = std::filesystem::path(currentPath).parent_path().string();
	}
	ImGui::SameLine();
	if (ImGui::Button("Convert_To_DDS_This_Folder", ImVec2(200.f, 30.f)))
	{
		string outputDirectory = "../../Client/Bin/Resources/Textures/DDS_Storage/";
		vector<string> imageFiles = GetFilesTexture(currentPath);
		for (const auto& file : imageFiles) {
			ConvertToDDSWithMipmap(file, outputDirectory);
		}
		MSG_BOX("DDS 추출 완료");
	}



	//()


	if (ImGui::BeginListBox("##file_list", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
		for (const auto& file : files) {
			std::string fileFullPath = currentPath + "/" + file;
			if (std::filesystem::is_directory(fileFullPath)) {
				if (ImGui::Selectable((file + "/").c_str())) {
					currentPath = fileFullPath;
				}
			}
			else {
				if (ImGui::Selectable(file.c_str())) {
					selectedFile = file;
					fullPath = fileFullPath;
				}
			}
		}
		ImGui::EndListBox();
	}

	if (IsPNGFile(selectedFile) || IsDDSFile(selectedFile) || ISTex(selectedFile)) {
		string selectedFilePath = fullPath;
		wstring wPath = utf8_to_wstring(selectedFilePath);
		wstring wName = utf8_to_wstring(selectedFile);
		Add_Texture_Prototype(wPath, wName);
		if (m_pTextureCom != nullptr)
		{
			if (ImGui::ImageButton(DirectXTextureToImTextureID(), ImVec2(300.f, 300.f), ImVec2(0.f, 0.f)))
			{
				m_pTextureProtoName = wName;
				m_pTextureFilePath = wPath;
			}
		}
	}

	if (ImGui::Button("Convert_To_DDS_Here", ImVec2(200.f, 30.f)))
	{
		vector<string> imageFiles = GetFilesTexture(currentPath);
		for (const auto& file : imageFiles) {
			ConvertToDDSWithMipmap_PathHere(file);
		}
		MSG_BOX("DDS 추출 완료");
	}
	ImGui::End();
}

void CImguiMgr::Trail_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	
	ImGui::Begin("Trail_Effect_Editor");
	static CParticle_Trail::TRAIL_DESC classdesc{};
	static CVIBuffer_Trail::TRAILDESC traildesc{};

	if (ChangedTrail == true)
	{
		traildesc = classdesc.traildesc;
		traildesc.ParentMat = TrailMat;
		ChangedTrail = false;
		m_pTextureProtoName = classdesc.Texture;
		m_pTextureFilePath = classdesc.TexturePath;
	}


	if (ImGui::RadioButton("SwordTrail", classdesc.eFuncType == TRAIL_EXTINCT))
		classdesc.eFuncType = TRAIL_EXTINCT;
	ImGui::SameLine();
	if (ImGui::RadioButton("EternalTrail", classdesc.eFuncType == TRAIL_ETERNAL))
		classdesc.eFuncType = TRAIL_ETERNAL;
	ImGui::SameLine();
	if (ImGui::RadioButton("CatMulrom", classdesc.eFuncType == TRAIL_CATMULROM))
		classdesc.eFuncType = TRAIL_CATMULROM;
	
	
	ImGui::ColorEdit3("Start_Color", reinterpret_cast<float*>(&classdesc.vStartColor));
	ImGui::ColorEdit3("End_Color", reinterpret_cast<float*>(&classdesc.vEndColor));
	traildesc.ParentMat = TrailMat;
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &traildesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&traildesc.vPivotPos));
	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&traildesc.vSize));
	ImGui::InputFloat("Speed", &traildesc.vSpeed);
	ImGui::InputFloat("lifetime", &traildesc.fLifeTime);
	ImGui::Checkbox("Desolve", &classdesc.Desolve);
	if (classdesc.Desolve == true)
	{
		ImGui::InputInt("DesolveTextureNum", &classdesc.DesolveNum);
	}
	ImGui::Checkbox("Blur", &classdesc.Blur);
	ImGui::Checkbox("Bloom", &classdesc.Bloom);
	if (classdesc.Bloom == true)
	{
		ImGui::InputFloat("BloomPower", &classdesc.fBloompower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&classdesc.vBloomColor));
	}
	classdesc.traildesc = traildesc;
	classdesc.Texture = m_pTextureProtoName;
	classdesc.TexturePath = m_pTextureFilePath;

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (classdesc.Texture == TEXT("") || classdesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐 파일을 선택해 주세요.");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Trail"),
				TEXT("Prototype_GameObject_Trail"), &classdesc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Trail"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ButtonSize))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Trails(effectname, &classdesc);
		}
	}
	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_TrailList()))
		{
			MSG_BOX("Failed_Save_Trail");
		}
		else
			MSG_BOX("Succeed_Save_Trail");
	}
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_TrailList()))
			MSG_BOX("Failed_Load_Trail");
		else
			MSG_BOX("Succeed_Load_Trail");
	}
	Trail_ListBox(&classdesc);

	ImGui::End();
}

void CImguiMgr::Trail_ListBox(CParticle_Trail::TRAIL_DESC* vDesc)
{
	if (TrailEffects.size() < 1)
		return;

	if (TrailEffects.size() != TrailEffectsNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Trail_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Trail_List", list_box_size))
	{
		for (int i = 0; i < TrailEffectsNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(TrailEffectsNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < TrailEffects.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			CParticle_Trail::TRAIL_DESC* traildesc = TrailEffects[current_item].get();
			traildesc->traildesc.ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail"), traildesc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*vDesc = *TrailEffects[current_item].get();
			ChangedTrail = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			*TrailEffects[current_item] = *((CParticle_Trail::TRAIL_DESC*)vDesc);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			TrailEffects[current_item].reset();
			TrailEffects.erase(TrailEffects.begin() + current_item);
			TrailEffectsNames.erase(TrailEffectsNames.begin() + current_item);

			if (current_item >= TrailEffects.size())
				current_item = TrailEffects.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : TrailEffects)
				iter.reset();
			TrailEffects.clear();
			TrailEffectsNames.clear();
			current_item = 0;
		}

	}

	ImGui::End();
}

HRESULT CImguiMgr::Store_Trails(char* Name, void* Value)
{
	string sName = Name;
	CParticle_Trail::TRAIL_DESC* rawPtr = reinterpret_cast<CParticle_Trail::TRAIL_DESC*>(Value);
	shared_ptr<CParticle_Trail::TRAIL_DESC> traildesc = make_shared<CParticle_Trail::TRAIL_DESC>(*rawPtr);
	TrailEffects.emplace_back(traildesc);
	TrailEffectsNames.emplace_back(sName);
	return S_OK;
}

HRESULT CImguiMgr::Save_TrailList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Trail.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = TrailEffects.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& iter : TrailEffects)
	{
		file.write((char*)&iter->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		file.write((char*)&iter->vStartColor, sizeof(_float3));
		file.write((char*)&iter->vEndColor, sizeof(_float3));
		file.write((char*)&iter->vBloomColor, sizeof(_float3));
		file.write((char*)&iter->fBloompower, sizeof(_float));
		file.write((char*)&iter->Desolve, sizeof(_bool));
		file.write((char*)&iter->Blur, sizeof(_bool));
		file.write((char*)&iter->Bloom, sizeof(_bool));
		file.write((char*)&iter->eFuncType, sizeof(TRAILFUNCTYPE));
		file.write((char*)&iter->DesolveNum, sizeof(_int));
		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : TrailEffectsNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < TrailEffectsNames.size(); ++i)
	{
		NumberFile << i << ". " << TrailEffectsNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_TrailList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Trail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : TrailEffects)
		iter.reset();
	TrailEffects.clear();
	TrailEffectsNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CParticle_Trail::TRAIL_DESC> readFile = make_shared<CParticle_Trail::TRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		inFile.read((char*)&readFile->vStartColor, sizeof(_float3));
		inFile.read((char*)&readFile->vEndColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloompower, sizeof(_float));
		inFile.read((char*)&readFile->Desolve, sizeof(_bool));
		inFile.read((char*)&readFile->Blur, sizeof(_bool));
		inFile.read((char*)&readFile->Bloom, sizeof(_bool));
		inFile.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
		inFile.read((char*)&readFile->DesolveNum, sizeof(_int));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		TrailEffects.emplace_back(readFile);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		TrailEffectsNames.emplace_back(str);
	}
	NameFile.close();
	return S_OK;
}

void CImguiMgr::SwordTrail_Tool()
{
	ImGui::Begin("SwordTrail_Tool");

	ImVec2 ButtonSize = { 100.f,30.f };

	static CSTrail::STRAIL_DESC StaticDesc = {};

	if (ChangedSwordTrail != true)
	{
		StaticDesc.traildesc.ParentMat = TrailMat;
		StaticDesc.Texture = m_pTextureProtoName;
		StaticDesc.TexturePath = m_pTextureFilePath;
	}
	else
	{
		StaticDesc.traildesc.ParentMat = TrailMat;
		ChangedSwordTrail = false;
	}
	
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &StaticDesc.traildesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&StaticDesc.traildesc.vOffsetPos));
	ImGui::InputFloat("Size", reinterpret_cast<float*>(&StaticDesc.traildesc.vSize));
	ImGui::InputFloat("LifeTime", &StaticDesc.traildesc.fLifeTime);
	ImGui::InputFloat("MovingTime", &StaticDesc.traildesc.fMaxTime);
	ImGui::InputInt("DesolveNum", &StaticDesc.iDesolveNum);
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&StaticDesc.vColor));

	ImGui::Checkbox("Bloom", &StaticDesc.isBloom);
	if (StaticDesc.isBloom == true)
	{
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&StaticDesc.vBloomColor));
		ImGui::InputFloat("BloomPower", &StaticDesc.fBloomPower);
	}
	ImGui::Checkbox("Distortion", &StaticDesc.isDestortion);

#pragma region BUTTONS
	if (ImGui::Button("Add", ButtonSize))
	{
		if (StaticDesc.Texture == TEXT("") || StaticDesc.TexturePath == TEXT("") || StaticDesc.traildesc.ParentMat == nullptr)
			MSG_BOX("텍스쳐와 행렬을 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"),
				TEXT("Prototype_GameObject_Sword_Trail"), &StaticDesc);
		}
	}

	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"));
	}

	static char Names[256] = "";
	ImGui::InputText("Name", Names, IM_ARRAYSIZE(Names));
	if (ImGui::Button("Store", ButtonSize))
	{
		if (Names[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreSwordTrail(Names, StaticDesc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_SwordTrail()))
		{
			MSG_BOX("Failed_SaveTrail");
		}
		else
			MSG_BOX("Save Success");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_SwordTrail()))
		{
			MSG_BOX("Failed_LoadTrail");
		}
		else
			MSG_BOX("Load Success");
	}
#pragma endregion BUTTONS

	SwordTrailListBox(&StaticDesc);


	ImGui::End();
}

HRESULT CImguiMgr::StoreSwordTrail(char* Name, CSTrail::STRAIL_DESC trail)
{
	string sName = Name;
	shared_ptr<CSTrail::STRAIL_DESC> RawPtr = make_shared<CSTrail::STRAIL_DESC>(trail);
	m_SwordTrails.emplace_back(RawPtr);
	SwordTrailNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::SwordTrailListBox(CSTrail::STRAIL_DESC* trail)
{
	if (m_SwordTrails.size() < 1)
		return;

	if (m_SwordTrails.size() != SwordTrailNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Sword_Trail_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("SwordTrailList", list_box_size))
	{
		for (int i = 0; i < SwordTrailNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(SwordTrailNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < m_SwordTrails.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			CSTrail::STRAIL_DESC* traildesc = m_SwordTrails[current_item].get();
			traildesc->traildesc.ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_Sword_Trail"), traildesc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*trail = *m_SwordTrails[current_item].get();
			ChangedSwordTrail = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			shared_ptr<CSTrail::STRAIL_DESC> newItem = make_shared<CSTrail::STRAIL_DESC>(*trail);
			m_SwordTrails[current_item] = newItem;
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_SwordTrails[current_item].reset();
			m_SwordTrails.erase(m_SwordTrails.begin() + current_item);
			SwordTrailNames.erase(SwordTrailNames.begin() + current_item);

			if (current_item >= m_SwordTrails.size())
				current_item = m_SwordTrails.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_SwordTrails)
				iter.reset();
			m_SwordTrails.clear();
			SwordTrailNames.clear();
			current_item = 0;
		}

	}

	ImGui::End();

}

HRESULT CImguiMgr::Save_SwordTrail()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/SwordTrail.Bin";

	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_SwordTrails.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& iter : m_SwordTrails)
	{
		file.write((char*)&iter->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		file.write((char*)&iter->isBloom, sizeof(_bool));
		file.write((char*)&iter->isDestortion, sizeof(_bool));
		file.write((char*)&iter->iDesolveNum, sizeof(_int));
		file.write((char*)&iter->vColor, sizeof(_float3));
		file.write((char*)&iter->vBloomColor, sizeof(_float3));
		file.write((char*)&iter->fBloomPower, sizeof(_float));

		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : SwordTrailNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < SwordTrailNames.size(); ++i)
	{
		NumberFile << i << ". " << SwordTrailNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_SwordTrail()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/SwordTrail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : m_SwordTrails)
		iter.reset();
	m_SwordTrails.clear();
	SwordTrailNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		//file.write((char*)&iter->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		//file.write((char*)&iter->isBloom, sizeof(_bool));
		//file.write((char*)&iter->isDestortion, sizeof(_bool));
		//file.write((char*)&iter->iDesolveNum, sizeof(_int));
		//file.write((char*)&iter->vColor, sizeof(_float3));
		//file.write((char*)&iter->vBloomColor, sizeof(_float3));
		//file.write((char*)&iter->fBloomPower, sizeof(_float));




		shared_ptr<CSTrail::STRAIL_DESC> readFile = make_shared<CSTrail::STRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		readFile->traildesc.ParentMat = nullptr;
		inFile.read((char*)&readFile->isBloom, sizeof(_bool));
		inFile.read((char*)&readFile->isDestortion, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloomPower, sizeof(_float));

		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_SwordTrails.emplace_back(readFile);
	}
	inFile.close();


	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		SwordTrailNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::Distortion_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Distortion_Editor");
	static CDistortionEffect::DISTORTIONEFFECT DisDesc{};
	if (!ChangedDistortion)
	{
		DisDesc.Texture = m_pTextureProtoName;
		DisDesc.TexturePath = m_pTextureFilePath;
	}
	else
	{
		DisDesc.vStartpos = { 0.f,0.f,0.f,1.f };
		ChangedDistortion = false;
	}

	ImGui::Checkbox("Extinct&Grow", &DisDesc.bFuncType);
	ImGui::Checkbox("Desolve", &DisDesc.bDisolve);
	if (DisDesc.bDisolve == true)
	{
		ImGui::InputInt("DesolveNum", &DisDesc.iDesolveNum);
	}
	ImGui::InputFloat2("StartScale", reinterpret_cast<float*>(&DisDesc.vStartScale));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&DisDesc.vStartpos));
	ImGui::InputFloat("Speed", &DisDesc.fSpeed);
	ImGui::InputFloat("LifeTime", &DisDesc.fLifeTime);

#pragma region BUTTONS
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (DisDesc.Texture == TEXT("") || DisDesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐를 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"),
				TEXT("Prototype_GameObject_Distortion_Effect"), &DisDesc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"));
	}

	static char Names[256] = "";
	ImGui::InputText("Name", Names, IM_ARRAYSIZE(Names));
	if (ImGui::Button("Store", ImVec2(200,30)))
	{
		if (Names[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreDistortion(Names, DisDesc);
		}
	}

	if (ImGui::Button("Save", ImVec2(100, 30)))
	{
		if (FAILED(Save_Distortion()))
			MSG_BOX("Failed_Save");
		else
			MSG_BOX("Succeed_Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(100, 30)))
	{
		if (FAILED(Load_Distortion()))
			MSG_BOX("Failed_Load");
		else
			MSG_BOX("Succeed_Load");
	}

#pragma endregion BUTTONS
	Distortion_ListBox(&DisDesc);
	ImGui::End();
}

HRESULT CImguiMgr::StoreDistortion(char* Name, CDistortionEffect::DISTORTIONEFFECT distortion)
{
	string sName = Name;
	shared_ptr<CDistortionEffect::DISTORTIONEFFECT> RawPtr = make_shared<CDistortionEffect::DISTORTIONEFFECT>(distortion);
	m_Distortions.emplace_back(RawPtr);
	DistortionNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Distortion_ListBox(CDistortionEffect::DISTORTIONEFFECT* distortion)
{
	if (m_Distortions.size() < 1)
		return;
	if (m_Distortions.size() != DistortionNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}
	ImGui::Begin("Distortion_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Distortion_List", list_box_size))
	{
		for (int i = 0; i < DistortionNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(DistortionNames[i].c_str(), is_selected))
			{
				current_item = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma region BUTTONS
	if (current_item >= 0 && current_item < m_Distortions.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"),
				TEXT("Prototype_GameObject_Distortion_Effect"), m_Distortions[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*distortion = *m_Distortions[current_item].get();
			ChangedDistortion = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			shared_ptr<CDistortionEffect::DISTORTIONEFFECT> newItem = make_shared<CDistortionEffect::DISTORTIONEFFECT>(*distortion);
			m_Distortions[current_item] = newItem;
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Distortions[current_item].reset();
			m_Distortions.erase(m_Distortions.begin() + current_item);
			DistortionNames.erase(DistortionNames.begin() + current_item);

			if (current_item >= m_Distortions.size())
				current_item = m_Distortions.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Distortions)
				iter.reset();
			m_Distortions.clear();
			DistortionNames.clear();
			current_item = 0;
		}


	}
#pragma endregion BUTTONS
	ImGui::End();
}

HRESULT CImguiMgr::Save_Distortion()
{
	//CDistortionEffect::DISTORTIONEFFECT
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Distortion.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Distortions.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Distortions)
	{
		file.write((char*)&iter->bFuncType, sizeof(_bool));
		file.write((char*)&iter->bDisolve, sizeof(_bool));
		file.write((char*)&iter->iDesolveNum, sizeof(_int));
		file.write((char*)&iter->vStartScale, sizeof(_float2));
		file.write((char*)&iter->vStartpos, sizeof(_float4));
		file.write((char*)&iter->fSpeed, sizeof(_float));
		file.write((char*)&iter->fLifeTime, sizeof(_float));
		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
	}
	file.close();


	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : DistortionNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < DistortionNames.size(); ++i)
	{
		NumberFile << i << ". " << DistortionNames[i] << std::endl;
	}
	NumberFile.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Distortion()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Distortion.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Distortions)
		iter.reset();
	m_Distortions.clear();
	DistortionNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CDistortionEffect::DISTORTIONEFFECT> readFile = make_shared<CDistortionEffect::DISTORTIONEFFECT>();
		inFile.read((char*)&readFile->bFuncType, sizeof(_bool));
		inFile.read((char*)&readFile->bDisolve, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vStartScale, sizeof(_float2));
		inFile.read((char*)&readFile->vStartpos, sizeof(_float4));
		inFile.read((char*)&readFile->fSpeed, sizeof(_float));
		inFile.read((char*)&readFile->fLifeTime, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_Distortions.emplace_back(readFile);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		DistortionNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::FireTool()
{
	ImGui::Begin("Fire_Editor");
	ImVec2 ButtonSize = { 100.f,30.f };

	static CFireEffect::FIREEFFECTDESC FireDesc{};

	ImGui::InputFloat3("ScrollSpeeds", reinterpret_cast<float*>(&FireDesc.ScrollSpeeds));
	ImGui::InputFloat3("FireScales", reinterpret_cast<float*>(&FireDesc.Scales));
	ImGui::InputFloat2("Distortion1", reinterpret_cast<float*>(&FireDesc.distortion1));
	ImGui::InputFloat2("Distortion2", reinterpret_cast<float*>(&FireDesc.distortion2));
	ImGui::InputFloat2("Distortion3", reinterpret_cast<float*>(&FireDesc.distortion3));
	ImGui::InputFloat("DistortionScale", &FireDesc.distortionScale);
	ImGui::InputFloat("DistortionBias", &FireDesc.distortionBias);

	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&FireDesc.vStartPos));
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&FireDesc.vOffsetPos));
	ImGui::InputFloat2("StartScale", reinterpret_cast<float*>(&FireDesc.vStartScale));


	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Fire"), TEXT("Prototype_GameObject_Fire_Effect"), &FireDesc);
	}

	if (ImGui::Button("Clear", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Fire"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f,30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreFires(effectname, FireDesc);
		}
	}


	FireListBox(&FireDesc);

	ImGui::End();
}

HRESULT CImguiMgr::StoreFires(char* Name, CFireEffect::FIREEFFECTDESC Fire)
{
	string sName = Name;
	shared_ptr<CFireEffect::FIREEFFECTDESC> StockValue = make_shared<CFireEffect::FIREEFFECTDESC>(Fire);
	m_Fires.emplace_back(StockValue);
	FireNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::FireListBox(CFireEffect::FIREEFFECTDESC* Fire)
{
	if (m_Fires.size() < 1)
		return;

	if (m_Fires.size() != FireNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Fire_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Fire_List", list_box_size))
	{
		for (int i = 0; i < FireNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(FireNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < FireNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Fire"), TEXT("Prototype_GameObject_Fire_Effect"), m_Fires[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Fire = *m_Fires[current_item].get();
			ChangedFire = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Fires[current_item] = make_shared<CFireEffect::FIREEFFECTDESC>(*Fire);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Fires[current_item].reset();
			m_Fires.erase(m_Fires.begin() + current_item);
			FireNames.erase(FireNames.begin() + current_item);

			if (current_item >= m_Fires.size())
				current_item = m_Fires.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Fires)
				iter.reset();
			m_Fires.clear();
			FireNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();
}

void CImguiMgr::Electron_Tool()
{
	ImGui::Begin("Electric_Editor");
	ImVec2 ButtonSize = { 100.f,30.f };

	static CElectronic::ELECTRONICDESC Desc{};
	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&Desc.vSize));
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	ImGui::InputFloat("LifeTime", &Desc.fMaxLifeTime);
	ImGui::InputFloat("BloomPower", &Desc.fBloomPower);
	ImGui::InputFloat("UVSpeed", &Desc.fUVSpeed);

	ImGui::ColorEdit3("BaseColor", reinterpret_cast<float*>(&Desc.vColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.vBloomColor));
	ImGui::InputInt("Particle", &Desc.ParticleIndex);

	if (ImGui::Button("Generate", ButtonSize))
	{
		//Prototype_GameObject_Electronic_Effect
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Electronic"), TEXT("Prototype_GameObject_Electronic_Effect"), &Desc);
	}

	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Electronic"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Lightnings(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Lightning()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Lightning()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	Electron_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Lightnings(char* Name, CElectronic::ELECTRONICDESC Lightning)
{
	string sName = Name;
	shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(Lightning);
	m_Lightnings.emplace_back(StockValue);
	LightningNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Electron_ListBox(CElectronic::ELECTRONICDESC* Lightning)
{
	if (m_Lightnings.size() < 1)
		return;

	if (m_Lightnings.size() != LightningNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Lightning_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("LightningList", list_box_size))
	{
		for (int i = 0; i < LightningNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(LightningNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < LightningNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Electronic"), TEXT("Prototype_GameObject_Electronic_Effect"), m_Lightnings[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Lightning = *m_Lightnings[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Lightnings[current_item] = make_shared<CElectronic::ELECTRONICDESC>(*Lightning);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Lightnings[current_item].reset();
			m_Lightnings.erase(m_Lightnings.begin() + current_item);
			LightningNames.erase(LightningNames.begin() + current_item);

			if (current_item >= m_Lightnings.size())
				current_item = m_Lightnings.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Lightnings)
				iter.reset();
			m_Lightnings.clear();
			LightningNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();
}

HRESULT CImguiMgr::Save_Lightning()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lightning.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Lightnings.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Lightnings)
	{
		file.write((char*)iter.get(), sizeof(CElectronic::ELECTRONICDESC));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightning.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : LightningNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightnings.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < LightningNames.size(); ++i)
	{
		NumberFile << i << ". " << LightningNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Lightning()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lightning.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Lightnings)
		iter.reset();
	m_Lightnings.clear();
	LightningNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CElectronic::ELECTRONICDESC readFile{};
		//shared_ptr<CElectronic::ELECTRONICDESC> readFile = make_shared<CElectronic::ELECTRONICDESC>();
		inFile.read((char*)&readFile, sizeof(CElectronic::ELECTRONICDESC));
		shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(readFile);
		m_Lightnings.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightning.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		LightningNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::Lazer_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Lazer_Edditor");

	static CAndrasLazer::ANDRAS_LAZER_TOTALDESC TotalDesc{};

	TotalDesc.ShooterMat = TrailMat;
	ImGui::InputFloat("LifeTime", &TotalDesc.fLifeTime);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Base_Lazer");

	ImGui::InputFloat3("B_MaxSize", reinterpret_cast<float*>(&TotalDesc.BaseDesc.vMaxSize));
	ImGui::InputFloat3("B_Offset", reinterpret_cast<float*>(&TotalDesc.BaseDesc.vOffset));
	ImGui::InputFloat("B_RotSpeed", &TotalDesc.BaseDesc.fRotationSpeed);
	ImGui::InputFloat("B_BloomPower", &TotalDesc.BaseDesc.fBloomPower);
	ImGui::InputFloat("B_DistortionPower", &TotalDesc.BaseDesc.fDistortionPower);
	ImGui::InputFloat("B_UVSpeed", &TotalDesc.BaseDesc.fUVSpeed);
	ImGui::InputInt("B_NumDesolve", &TotalDesc.BaseDesc.NumDesolve);
	ImGui::ColorEdit3("B_Color", reinterpret_cast<float*>(&TotalDesc.BaseDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Cylinder");

	ImGui::InputFloat3("C_MaxSize", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.vMaxSize));
	ImGui::InputFloat3("C_Offset", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.vOffset));
	ImGui::InputFloat("C_RotSpeed", &TotalDesc.CylinderDesc.fRotationSpeed);
	ImGui::InputFloat("C_BloomPower", &TotalDesc.CylinderDesc.fBloomPower);
	ImGui::InputFloat("C_DistortionPower", &TotalDesc.CylinderDesc.fDistortionPower);
	ImGui::InputFloat("C_UVSpeed", &TotalDesc.CylinderDesc.fUVSpeed);
	ImGui::InputInt("C_NumDesolve", &TotalDesc.CylinderDesc.NumDesolve);
	ImGui::ColorEdit3("C_Color", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Screw");

	ImGui::InputFloat3("S_MaxSize", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.vMaxSize));
	ImGui::InputFloat3("S_Offset", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.vOffset));
	ImGui::InputFloat("S_RotSpeed", &TotalDesc.ScrewDesc.fRotationSpeed);
	ImGui::InputFloat("S_BloomPower", &TotalDesc.ScrewDesc.fBloomPower);
	ImGui::InputFloat("S_DistortionPower", &TotalDesc.ScrewDesc.fDistortionPower);
	ImGui::InputFloat("S_UVSpeed", &TotalDesc.ScrewDesc.fUVSpeed);
	ImGui::InputInt("S_NumDesolve", &TotalDesc.ScrewDesc.NumDesolve);
	ImGui::ColorEdit3("S_Color", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Electron");

	ImGui::InputFloat("E_Interval", &TotalDesc.ElectricInterval);
	ImGui::InputFloat3("E_MaxSize", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.vMaxSize));
	ImGui::InputFloat3("E_Offset", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.vOffset));
	ImGui::InputFloat("E_LifeTime", &TotalDesc.ElectricDesc.fMaxLifeTime);
	ImGui::InputFloat("E_UVSpeed", &TotalDesc.ElectricDesc.fUVSpeed);
	ImGui::ColorEdit3("E_Color", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.fColor));
	ImGui::ColorEdit3("E_BloomColor", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.fBloomColor));
	ImGui::InputFloat("E_BloomPower", &TotalDesc.ElectricDesc.fBloomPower);
	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Rain");

	ImGui::InputFloat3("R_MaxSize", reinterpret_cast<float*>(&TotalDesc.RainDesc.vMaxSize));
	ImGui::InputFloat3("R_Offset", reinterpret_cast<float*>(&TotalDesc.RainDesc.vOffset));
	ImGui::InputFloat("R_RotSpeed", &TotalDesc.RainDesc.fRotationSpeed);
	ImGui::InputFloat("R_BloomPower", &TotalDesc.RainDesc.fBloomPower);
	ImGui::InputFloat("R_DistortionPower", &TotalDesc.RainDesc.fDistortionPower);
	ImGui::InputFloat("R_UVSpeed", &TotalDesc.RainDesc.fUVSpeed);
	ImGui::InputInt("R_NumDesolve", &TotalDesc.RainDesc.NumDesolve);
	ImGui::ColorEdit3("R_Color", reinterpret_cast<float*>(&TotalDesc.RainDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Shield");

	ImGui::InputFloat("SH_Interval", &TotalDesc.ShieldInterval);
	ImGui::InputFloat("SH_LifeTime", &TotalDesc.ShieldDesc.fMaxLifeTime);
	ImGui::InputFloat3("SH_MaxSize", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.vMaxSize));
	ImGui::InputFloat3("SH_Offset", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.vOffset));
	ImGui::InputFloat("SH_BloomPower", &TotalDesc.ShieldDesc.fBloomPower);
	ImGui::InputFloat("SH_UVSpeed", &TotalDesc.ShieldDesc.fUVSpeed);
	ImGui::ColorEdit3("SH_Color", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.fColor));
	ImGui::ColorEdit3("SH_BloomColor", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.fBloomColor));


	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Cast");
	ImGui::InputFloat3("Cast_Size", reinterpret_cast<float*>(&TotalDesc.CastDesc.vSize));
	ImGui::ColorEdit3("Cast_Color", reinterpret_cast<float*>(&TotalDesc.CastDesc.fColor));
	ImGui::ColorEdit3("Cast_BloomColor", reinterpret_cast<float*>(&TotalDesc.CastDesc.BloomColor));
	ImGui::InputFloat3("Cast_Offset", reinterpret_cast<float*>(&TotalDesc.CastDesc.vOffset));
	ImGui::InputFloat("Cast_BloomPower", &TotalDesc.CastDesc.fBloomPower);
	ImGui::InputFloat("Cast_LifeTime", &TotalDesc.CastDesc.fMaxLifeTime);
	ImGui::InputFloat2("Cast_ThreadRatio", reinterpret_cast<float*>(&TotalDesc.CastDesc.fThreadRatio));
	ImGui::InputFloat("Cast_SlowStrength", &TotalDesc.CastDesc.fSlowStrength);
	
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TotalDesc.ShooterMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerLazer"),
				TEXT("Prototype_GameObject_AndrasLazerSpawner"), &TotalDesc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerLazer"));
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Lazer(effectname, TotalDesc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Lazer()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Lazer()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	Lazer_ListBox(&TotalDesc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Lazer(char* Name, CAndrasLazer::ANDRAS_LAZER_TOTALDESC Lazer)
{
	string sName = Name;
	shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC> StockValue = make_shared<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>(Lazer);
	m_Lazers.emplace_back(StockValue);
	LazerNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Lazer_ListBox(CAndrasLazer::ANDRAS_LAZER_TOTALDESC* Lazer)
{
#pragma region exception
	if (m_Lazers.size() < 1)
		return;

	if (m_Lazers.size() != LazerNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Lazer_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception
#pragma region LISTBOX
	if (ImGui::BeginListBox("Lazer_List", list_box_size))
	{
		for (int i = 0; i < LazerNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(LazerNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX
#pragma region BUTTONS
	if (current_item >= 0 && current_item < LazerNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CAndrasLazer::ANDRAS_LAZER_TOTALDESC* Desc = m_Lazers[current_item].get();
			Desc->ShooterMat = TrailMat;
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("LayerLazer"), TEXT("Prototype_GameObject_AndrasLazerSpawner"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Lazer = *m_Lazers[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Lazers[current_item] = make_shared<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>(*Lazer);
		}
		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Lazers[current_item].reset();
			m_Lazers.erase(m_Lazers.begin() + current_item);
			LazerNames.erase(LazerNames.begin() + current_item);

			if (current_item >= m_Lazers.size())
				current_item = m_Lazers.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Lazers)
				iter.reset();
			m_Lazers.clear();
			LazerNames.clear();
			current_item = 0;
		}
	}
#pragma endregion BUTTONS
	ImGui::End();
}

HRESULT CImguiMgr::Save_Lazer()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lazer.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Lazers.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Lazers)
	{
		file.write((char*)iter.get(), sizeof(CAndrasLazer::ANDRAS_LAZER_TOTALDESC));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lazers.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : LazerNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lazers.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < LazerNames.size(); ++i)
	{
		NumberFile << i << ". " << LazerNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Lazer()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lazer.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Lazers)
		iter.reset();
	m_Lazers.clear();
	LazerNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CAndrasLazer::ANDRAS_LAZER_TOTALDESC readFile{};
		inFile.read((char*)&readFile, sizeof(CAndrasLazer::ANDRAS_LAZER_TOTALDESC));
		readFile.ShooterMat = nullptr;
		shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC> StockValue = make_shared<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>(readFile);
		m_Lazers.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lazers.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		LazerNames.emplace_back(str);
	}
	NameFile.close();
	return S_OK;
}

void CImguiMgr::Tornado_Tool()
{
#pragma region VALUES
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Tornado_Editor");

	static CTornadoEffect::TORNADODESC TDesc{};
	ImGui::InputFloat("Speed", &TDesc.fSpeed);
	ImGui::InputFloat("Rot_Speed", &TDesc.fRotationSpeed);
	ImGui::InputFloat("LifeTime", &TDesc.fLifeTime);
	ImGui::InputInt("NumParticle", &TDesc.NumParticle);

	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&TDesc.vStartPos));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind");

	ImGui::InputFloat3("WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc.vMaxSize));
	ImGui::InputFloat3("WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc.vOffset));
	ImGui::ColorEdit3("WindColor", reinterpret_cast<float*>(&TDesc.WindDesc.fColor));
	ImGui::ColorEdit3("WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc.fColor2));


	ImGui::InputFloat("WindRotSpeed", &TDesc.WindDesc.fRotationSpeed);
	ImGui::InputFloat("WindUVSpeed", &TDesc.WindDesc.fUVSpeed);
	ImGui::InputFloat("WindGrowSpeed", &TDesc.WindDesc.fGrowSpeed);
	ImGui::InputFloat("WindBloomPower", &TDesc.WindDesc.fBloomPower);
	ImGui::InputInt("WindNumDesolve", &TDesc.WindDesc.NumDesolve);

	ImGui::InputFloat("RadialStrength", &TDesc.WindDesc.RadicalStrength);
	ImGui::Checkbox("Distortion", &TDesc.WindDesc.IsDistortion);
	ImGui::Checkbox("Opacity", &TDesc.WindDesc.Opacity);
	if (TDesc.WindDesc.Opacity == true)
		ImGui::InputFloat("OpacityStrength", &TDesc.WindDesc.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind2");

	ImGui::InputFloat3("2WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc2.vMaxSize));
	ImGui::InputFloat3("2WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc2.vOffset));
	ImGui::ColorEdit3("2WindColor", reinterpret_cast<float*>(&TDesc.WindDesc2.fColor));
	ImGui::ColorEdit3("2WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc2.fColor2));


	ImGui::InputFloat("2WindRotSpeed", &TDesc.WindDesc2.fRotationSpeed);
	ImGui::InputFloat("2WindUVSpeed", &TDesc.WindDesc2.fUVSpeed);
	ImGui::InputFloat("2WindGrowSpeed", &TDesc.WindDesc2.fGrowSpeed);
	ImGui::InputFloat("2WindBloomPower", &TDesc.WindDesc2.fBloomPower);
	ImGui::InputInt("2WindNumDesolve", &TDesc.WindDesc2.NumDesolve);

	ImGui::InputFloat("2RadialStrength", &TDesc.WindDesc2.RadicalStrength);
	ImGui::Checkbox("2Distortion", &TDesc.WindDesc2.IsDistortion);
	ImGui::Checkbox("2Opacity", &TDesc.WindDesc2.Opacity);
	if (TDesc.WindDesc2.Opacity == true)
		ImGui::InputFloat("2OpacityStrength", &TDesc.WindDesc2.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind3");

	ImGui::InputFloat3("3WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc3.vMaxSize));
	ImGui::InputFloat3("3WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc3.vOffset));
	ImGui::ColorEdit3("3WindColor", reinterpret_cast<float*>(&TDesc.WindDesc3.fColor));
	ImGui::ColorEdit3("3WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc3.fColor2));


	ImGui::InputFloat("3WindRotSpeed", &TDesc.WindDesc3.fRotationSpeed);
	ImGui::InputFloat("3WindUVSpeed", &TDesc.WindDesc3.fUVSpeed);
	ImGui::InputFloat("3WindGrowSpeed", &TDesc.WindDesc3.fGrowSpeed);
	ImGui::InputFloat("3WindBloomPower", &TDesc.WindDesc3.fBloomPower);
	ImGui::InputInt("3WindNumDesolve", &TDesc.WindDesc3.NumDesolve);

	ImGui::InputFloat("3RadialStrength", &TDesc.WindDesc3.RadicalStrength);
	ImGui::Checkbox("3Distortion", &TDesc.WindDesc3.IsDistortion);
	ImGui::Checkbox("3Opacity", &TDesc.WindDesc3.Opacity);
	if (TDesc.WindDesc3.Opacity == true)
		ImGui::InputFloat("3OpacityStrength", &TDesc.WindDesc3.OpacityPower);


	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Root");

	ImGui::InputFloat3("RootMaxSize", reinterpret_cast<float*>(&TDesc.RootDesc.vMaxSize));
	ImGui::InputFloat3("RootOffset", reinterpret_cast<float*>(&TDesc.RootDesc.vOffset));
	ImGui::ColorEdit3("RootColor", reinterpret_cast<float*>(&TDesc.RootDesc.fColor));


	ImGui::InputFloat("Interval", &TDesc.fRootInterval);
	ImGui::InputFloat("StartYScale", &TDesc.RootDesc.StartYScale);
	ImGui::InputFloat("RootRotSpeed", &TDesc.RootDesc.fRotationSpeed);
	ImGui::InputFloat("RootBloompower", &TDesc.RootDesc.fBloomPower);
	ImGui::InputFloat("RootLifeTime", &TDesc.RootDesc.fMaxLifeTime);
	ImGui::InputFloat("RootUVSpeed", &TDesc.RootDesc.fUVSpeed);
	ImGui::InputFloat("RootGrowSpeed", &TDesc.RootDesc.fGrowSpeed);
	ImGui::InputInt("RootNumDesolve", &TDesc.RootDesc.NumDesolve);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Ring1");

	ImGui::InputFloat3("RingMaxSize", reinterpret_cast<float*>(&TDesc.RingDesc1.vMaxSize));
	ImGui::InputFloat3("RingOffset", reinterpret_cast<float*>(&TDesc.RingDesc1.vOffset));
	ImGui::ColorEdit3("RingColor", reinterpret_cast<float*>(&TDesc.RingDesc1.fColor));
	ImGui::ColorEdit3("RingColor2", reinterpret_cast<float*>(&TDesc.RingDesc1.fColor2));


	ImGui::InputFloat("RingRotSpeed", &TDesc.RingDesc1.fRotationSpeed);
	ImGui::InputFloat("RingUVSpeed", &TDesc.RingDesc1.fUVSpeed);
	ImGui::InputFloat("RingGrowSpeed", &TDesc.RingDesc1.fGrowSpeed);
	ImGui::InputFloat("RingBloomPower", &TDesc.RingDesc1.fBloomPower);
	ImGui::InputInt("RingNumDesolve", &TDesc.RingDesc1.NumDesolve);

	ImGui::InputFloat("Ring_RadialStrength", &TDesc.RingDesc1.RadicalStrength);
	ImGui::Checkbox("Ring_Distortion", &TDesc.RingDesc1.IsDistortion);
	ImGui::Checkbox("Ring_Opacity", &TDesc.RingDesc1.Opacity);
	if (TDesc.RingDesc1.Opacity == true)
		ImGui::InputFloat("Ring_OpacityStrength", &TDesc.RingDesc1.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Ring2");

	ImGui::InputFloat3("2RingMaxSize", reinterpret_cast<float*>(&TDesc.RingDesc2.vMaxSize));
	ImGui::InputFloat3("2RingOffset", reinterpret_cast<float*>(&TDesc.RingDesc2.vOffset));
	ImGui::ColorEdit3("2RingColor", reinterpret_cast<float*>(&TDesc.RingDesc2.fColor));
	ImGui::ColorEdit3("2RingColor2", reinterpret_cast<float*>(&TDesc.RingDesc2.fColor2));


	ImGui::InputFloat("2RingRotSpeed", &TDesc.RingDesc2.fRotationSpeed);
	ImGui::InputFloat("2RingUVSpeed", &TDesc.RingDesc2.fUVSpeed);
	ImGui::InputFloat("2RingGrowSpeed", &TDesc.RingDesc2.fGrowSpeed);
	ImGui::InputFloat("2RingBloomPower", &TDesc.RingDesc2.fBloomPower);
	ImGui::InputInt("2RingNumDesolve", &TDesc.RingDesc2.NumDesolve);

	ImGui::InputFloat("2Ring_RadialStrength", &TDesc.RingDesc2.RadicalStrength);
	ImGui::Checkbox("2Ring_Distortion", &TDesc.RingDesc2.IsDistortion);
	ImGui::Checkbox("2Ring_Opacity", &TDesc.RingDesc2.Opacity);
	if (TDesc.RingDesc2.Opacity == true)
		ImGui::InputFloat("2Ring_OpacityStrength", &TDesc.RingDesc2.OpacityPower);
#pragma endregion VALUES

	TDesc.pTarget = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(),
		TEXT("LayerDummy"));

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Tornado"), TEXT("Prototype_GameObject_Tornado"), &TDesc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Tornado"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Tornados(effectname, TDesc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Tornado()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Tornado()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}


	Tornado_ListBox(&TDesc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Tornados(char* Name, CTornadoEffect::TORNADODESC desc)
{
	string sName = Name;
	shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(desc);
	m_Tornados.emplace_back(StockValue);
	TornadoNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Tornado_ListBox(CTornadoEffect::TORNADODESC* Tornado)
{
#pragma region exception
	if (m_Tornados.size() < 1)
		return;

	if (m_Tornados.size() != TornadoNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Tornado_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception

	if (ImGui::BeginListBox("Tornado_List", list_box_size))
	{
		for (int i = 0; i < TornadoNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(TornadoNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < TornadoNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CTornadoEffect::TORNADODESC* Desc = m_Tornados[current_item].get();
			Desc->pTarget = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(),
				TEXT("LayerDummy"));
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Tornado"), TEXT("Prototype_GameObject_Tornado"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Tornado = *m_Tornados[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Tornados[current_item] = make_shared<CTornadoEffect::TORNADODESC>(*Tornado);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Tornados[current_item].reset();
			m_Tornados.erase(m_Tornados.begin() + current_item);
			TornadoNames.erase(TornadoNames.begin() + current_item);

			if (current_item >= m_Tornados.size())
				current_item = m_Tornados.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Tornados)
				iter.reset();
			m_Tornados.clear();
			TornadoNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_Tornado()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Tornados.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Tornados.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Tornados)
	{
		file.write((char*)iter.get(), sizeof(CTornadoEffect::TORNADODESC));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : TornadoNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < TornadoNames.size(); ++i)
	{
		NumberFile << i << ". " << TornadoNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Tornado()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Tornados.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Tornados)
		iter.reset();
	m_Tornados.clear();
	TornadoNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CTornadoEffect::TORNADODESC readFile{};
		inFile.read((char*)&readFile, sizeof(CTornadoEffect::TORNADODESC));
		readFile.pTarget = nullptr;
		shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(readFile);
		m_Tornados.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		TornadoNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::FirePillarTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Tornado_Editor");

	static CFirePillar::FIREPILLAR Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat("SizeInterval", &Desc.Interval);
	ImGui::InputFloat4("vStartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	ImGui::InputFloat3("vParentScale", reinterpret_cast<float*>(&Desc.vScale));

	if (ImGui::CollapsingHeader("Charge"))
	{
		ImGui::InputFloat3("Charge_Size", reinterpret_cast<float*>(&Desc.Charge.vSize));
		ImGui::ColorEdit3("Charge_Color", reinterpret_cast<float*>(&Desc.Charge.fColor));
		ImGui::ColorEdit3("Charge_BloomColor", reinterpret_cast<float*>(&Desc.Charge.BloomColor));
		ImGui::InputFloat("Charge_BloomPower", &Desc.Charge.fBloomPower);
		ImGui::InputFloat("Charge_LifeTime", &Desc.Charge.fMaxLifeTime);
		ImGui::InputFloat2("Charge_ThreadRatio", reinterpret_cast<float*>(&Desc.Charge.fThreadRatio));
		ImGui::InputFloat("Charge_SlowStrength", &Desc.Charge.fSlowStrength);
	}

	if (ImGui::CollapsingHeader("Pillar1"))
	{
		ImGui::InputFloat3("P1_vMaxSize", reinterpret_cast<float*>(&Desc.pillar1.vMaxSize));
		ImGui::InputFloat("P1_BloomPower", &Desc.pillar1.fBloomPower);
		ImGui::InputFloat("P1_UVSpeed", &Desc.pillar1.fUVSpeed);
		ImGui::InputFloat("P1_RadicalStrength", &Desc.pillar1.RadicalStrength);
		ImGui::InputFloat("P1_GrowSpeed", &Desc.pillar1.fGrowSpeed);
		ImGui::Checkbox("P1_Distortion", &Desc.pillar1.IsDistortion);
		ImGui::Checkbox("P1_Opacity", &Desc.pillar1.Opacity);
		if (Desc.pillar1.Opacity == true)
			ImGui::InputFloat("P1_OpPower", &Desc.pillar1.OpacityPower);

		ImGui::InputInt("P1_NumDesolve", &Desc.pillar1.NumDesolve);

		ImGui::ColorEdit3("P1_Color1", reinterpret_cast<float*>(&Desc.pillar1.fColor));
		ImGui::ColorEdit3("P1_Color2", reinterpret_cast<float*>(&Desc.pillar1.fColor2));
	}


	if (ImGui::CollapsingHeader("Pillar2"))
	{
		ImGui::InputFloat3("P2_vMaxSize", reinterpret_cast<float*>(&Desc.pillar2.vMaxSize));
		ImGui::InputFloat("P2_BloomPower", &Desc.pillar2.fBloomPower);
		ImGui::InputFloat("P2_UVSpeed", &Desc.pillar2.fUVSpeed);
		ImGui::InputFloat("P2_RadicalStrength", &Desc.pillar2.RadicalStrength);
		ImGui::InputFloat("P2_GrowSpeed", &Desc.pillar2.fGrowSpeed);
		ImGui::Checkbox("P2_Distortion", &Desc.pillar2.IsDistortion);
		ImGui::Checkbox("P2_Opacity", &Desc.pillar2.Opacity);
		if (Desc.pillar2.Opacity == true)
			ImGui::InputFloat("P2_OpPower", &Desc.pillar2.OpacityPower);

		ImGui::InputInt("P2_NumDesolve", &Desc.pillar2.NumDesolve);

		ImGui::ColorEdit3("P2_Color1", reinterpret_cast<float*>(&Desc.pillar2.fColor));
		ImGui::ColorEdit3("P2_Color2", reinterpret_cast<float*>(&Desc.pillar2.fColor2));
	}

	if (ImGui::CollapsingHeader("Pillar4"))
	{
		ImGui::InputFloat3("P4_vMaxSize", reinterpret_cast<float*>(&Desc.pillar4.vMaxSize));
		ImGui::InputFloat("P4_BloomPower", &Desc.pillar4.fBloomPower);
		ImGui::InputFloat("P4_UVSpeed", &Desc.pillar4.fUVSpeed);
		ImGui::InputFloat("P4_RadicalStrength", &Desc.pillar4.RadicalStrength);
		ImGui::InputFloat("P4_GrowSpeed", &Desc.pillar4.fGrowSpeed);
		ImGui::Checkbox("P4_Opacity", &Desc.pillar4.Opacity);
		if (Desc.pillar4.Opacity == true)
			ImGui::InputFloat("P4_OpPower", &Desc.pillar4.OpacityPower);

		ImGui::InputInt("P4_NumDesolve", &Desc.pillar4.NumDesolve);

		ImGui::ColorEdit3("P4_Color1", reinterpret_cast<float*>(&Desc.pillar4.fColor));
		ImGui::ColorEdit3("P4_Color2", reinterpret_cast<float*>(&Desc.pillar4.fColor2));
	}

	if (ImGui::CollapsingHeader("Bottom"))
	{
		ImGui::InputFloat3("Bottom_Size", reinterpret_cast<float*>(&Desc.Bottom.vMaxSize));
		ImGui::InputFloat("Bottom_BloomPower", &Desc.Bottom.fBloomPower);

		ImGui::InputFloat("Bottom_UVSpeed", &Desc.Bottom.fUVSpeed);
		ImGui::InputFloat("Bottom_RadicalStrength", &Desc.Bottom.RadicalStrength);
		ImGui::InputFloat("Bottom_GrowSpeed", &Desc.Bottom.fGrowSpeed);
		ImGui::Checkbox("Bottom_Distortion", &Desc.Bottom.IsDistortion);
		ImGui::Checkbox("Bottom_Opacity", &Desc.Bottom.Opacity);
		if (Desc.Bottom.Opacity == true)
			ImGui::InputFloat("Bottom_OpPower", &Desc.Bottom.OpacityPower);

		ImGui::InputInt("Bottom_NumDesolve", &Desc.Bottom.NumDesolve);

		ImGui::ColorEdit3("Bottom_Color1", reinterpret_cast<float*>(&Desc.Bottom.fColor));
		ImGui::ColorEdit3("Bottom_Color2", reinterpret_cast<float*>(&Desc.Bottom.fColor2));
	}

	if (ImGui::CollapsingHeader("Rock"))
	{
		ImGui::InputFloat3("Rock_MinSize", reinterpret_cast<float*>(&Desc.Ground.vMinSize));
		ImGui::InputFloat3("Rock_MaxSize", reinterpret_cast<float*>(&Desc.Ground.vMaxSize));
		ImGui::InputFloat("Rock_LifeTime", &Desc.Ground.fMaxLifeTime);
		ImGui::InputFloat("Rock_MinRatio", &Desc.Ground.MinRatio);
		ImGui::InputFloat("Rock_MaxRatio", &Desc.Ground.MaxRatio);
	}

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_FirePillar"), TEXT("Prototype_GameObject_FirePillar"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_FirePillar(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_FirePillar(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::End();
}

HRESULT CImguiMgr::Save_FirePillar(CFirePillar::FIREPILLAR* FirePillar)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/FirePillar.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)FirePillar, sizeof(CFirePillar::FIREPILLAR));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_FirePillar(CFirePillar::FIREPILLAR* FirePillar)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/FirePillar.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)FirePillar, sizeof(CFirePillar::FIREPILLAR));
	inFile.close();
	return S_OK;
}

void CImguiMgr::HealEffectTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Heal_Effect_Editor");
	static CHealEffect::HEALEFFECT Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputInt("NumParticle", &Desc.iNumParticle);
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Ribbon");
	ImGui::InputFloat3("RibbonSize", reinterpret_cast<float*>(&Desc.RibbonDesc.vSize));
	ImGui::ColorEdit3("RibbonColor", reinterpret_cast<float*>(&Desc.RibbonDesc.fColor));
	ImGui::ColorEdit3("RibbonBloomColor", reinterpret_cast<float*>(&Desc.RibbonDesc.BloomColor));
	ImGui::InputFloat("RibbonBloomPower", &Desc.RibbonDesc.fBloomPower);
	ImGui::InputFloat("RibbonLifeTime", &Desc.RibbonDesc.fMaxLifeTime);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Spiral");
	ImGui::InputFloat3("SpiralSize", reinterpret_cast<float*>(&Desc.SpiralDesc.vSize));
	ImGui::ColorEdit3("SpiralColor", reinterpret_cast<float*>(&Desc.SpiralDesc.fColor));
	ImGui::ColorEdit3("SpiralBloomColor", reinterpret_cast<float*>(&Desc.SpiralDesc.BloomColor));
	ImGui::InputFloat("SpiralBloomPower", &Desc.SpiralDesc.fBloomPower);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Line");
	ImGui::InputFloat3("LineSize", reinterpret_cast<float*>(&Desc.LineDesc.vSize));
	ImGui::ColorEdit3("LineColor", reinterpret_cast<float*>(&Desc.LineDesc.fColor));
	ImGui::ColorEdit3("LineBloomColor", reinterpret_cast<float*>(&Desc.LineDesc.BloomColor));
	ImGui::InputFloat("LineBloomPower", &Desc.LineDesc.fBloomPower);

	Desc.ParentMat = TrailMat;

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (Desc.ParentMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Heal"), TEXT("Prototype_GameObject_HealEffect"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Heal"));
	}


	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Heal(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Heal()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Heal()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	Heal_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Heal(char* Name, CHealEffect::HEALEFFECT desc)
{
	string sName = Name;
	shared_ptr<CHealEffect::HEALEFFECT> StockValue = make_shared<CHealEffect::HEALEFFECT>(desc);
	m_Heals.emplace_back(StockValue);
	HealNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Heal_ListBox(CHealEffect::HEALEFFECT* Tornado)
{
#pragma region exception
	if (m_Heals.size() < 1)
		return;

	if (m_Heals.size() != HealNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Heal_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception
#pragma region LISTBOX
	if (ImGui::BeginListBox("Heal_List", list_box_size))
	{
		for (int i = 0; i < HealNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(HealNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX

	if (current_item >= 0 && current_item < HealNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CHealEffect::HEALEFFECT* Desc = m_Heals[current_item].get();
			Desc->ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Heal"), TEXT("Prototype_GameObject_HealEffect"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Tornado = *m_Heals[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Heals[current_item] = make_shared<CHealEffect::HEALEFFECT>(*Tornado);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Heals[current_item].reset();
			m_Heals.erase(m_Heals.begin() + current_item);
			HealNames.erase(HealNames.begin() + current_item);

			if (current_item >= m_Heals.size())
				current_item = m_Heals.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Heals)
				iter.reset();
			m_Heals.clear();
			HealNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();
}

HRESULT CImguiMgr::Save_Heal()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Heals.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Heals.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Heals)
	{
		file.write((char*)iter.get(), sizeof(CHealEffect::HEALEFFECT));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Heals.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : HealNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Heal.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < HealNames.size(); ++i)
	{
		NumberFile << i << ". " << HealNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Heal()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Heals.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Heals)
		iter.reset();
	m_Heals.clear();
	HealNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CHealEffect::HEALEFFECT readFile{};
		inFile.read((char*)&readFile, sizeof(CHealEffect::HEALEFFECT));
		readFile.ParentMat = nullptr;
		shared_ptr<CHealEffect::HEALEFFECT> StockValue = make_shared<CHealEffect::HEALEFFECT>(readFile);
		m_Heals.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Heals.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		HealNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::SwingEffectTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("SwingEffect_Editor");

	static CSwingEffect::SWINGEFFECT Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Vane");

	ImGui::InputFloat3("VaneSize", reinterpret_cast<float*>(&Desc.VaneDesc.vSize));
	ImGui::InputFloat3("VaneOffset", reinterpret_cast<float*>(&Desc.VaneDesc.vOffset));
	ImGui::ColorEdit3("VaneColor", reinterpret_cast<float*>(&Desc.VaneDesc.fColor));
	ImGui::ColorEdit3("VaneBloomColor", reinterpret_cast<float*>(&Desc.VaneDesc.BloomColor));
	ImGui::InputFloat("VaneBloomPower", &Desc.VaneDesc.fBloomPower);
	ImGui::InputFloat("VaneLifeTime", &Desc.VaneDesc.fMaxLifeTime);
	ImGui::InputFloat2("VaneThreadRatio", reinterpret_cast<float*>(&Desc.VaneDesc.fThreadRatio));
	ImGui::InputFloat("VaneSlowSpeed", &Desc.VaneDesc.fSlowStrength);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Ribbon");
	ImGui::InputFloat3("Ribbon_Size", reinterpret_cast<float*>(&Desc.RibbonDesc.vSize));
	ImGui::ColorEdit3("Ribbon_Color", reinterpret_cast<float*>(&Desc.RibbonDesc.fColor));
	ImGui::ColorEdit3("Ribbon_BloomColor", reinterpret_cast<float*>(&Desc.RibbonDesc.BloomColor));
	ImGui::InputFloat("Ribbon_BloomPower", &Desc.RibbonDesc.fBloomPower);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Spiral");

	ImGui::InputFloat3("SpiralStartSize", reinterpret_cast<float*>(&Desc.SpiralDesc.vSize));
	ImGui::InputFloat3("SpiralMaxSize", reinterpret_cast<float*>(&Desc.SpiralDesc.vMaxSize));
	ImGui::InputFloat3("SpiralOffset", reinterpret_cast<float*>(&Desc.SpiralDesc.vOffset));
	ImGui::ColorEdit3("SpiralColor", reinterpret_cast<float*>(&Desc.SpiralDesc.fColor));
	ImGui::ColorEdit3("SpiralBloomColor", reinterpret_cast<float*>(&Desc.SpiralDesc.BloomColor));
	ImGui::InputFloat("SpiralBloomPower", &Desc.SpiralDesc.fBloomPower);
	ImGui::InputFloat("SpiralLifeTime", &Desc.SpiralDesc.fMaxLifeTime);
	ImGui::InputFloat2("SpiralThreadRatio", reinterpret_cast<float*>(&Desc.SpiralDesc.fThreadRatio));
	ImGui::InputFloat("SpiralSlowSpeed", &Desc.SpiralDesc.fSlowStrength);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Cylinder");

	ImGui::InputFloat3("CylinderStartSize", reinterpret_cast<float*>(&Desc.CylinderDesc.vSize));
	ImGui::InputFloat3("CylinderMaxSize", reinterpret_cast<float*>(&Desc.CylinderDesc.vEndSized));
	ImGui::InputFloat3("CylinderOffset", reinterpret_cast<float*>(&Desc.CylinderDesc.vOffset));
	ImGui::ColorEdit3("CylinderColor", reinterpret_cast<float*>(&Desc.CylinderDesc.fColor));
	ImGui::ColorEdit3("CylinderBloomColor", reinterpret_cast<float*>(&Desc.CylinderDesc.BloomColor));
	ImGui::InputFloat("CylinderBloomPower", &Desc.CylinderDesc.fBloomPower);
	ImGui::InputFloat("CylinderLifeTime", &Desc.CylinderDesc.fMaxLifeTime);
	ImGui::InputFloat2("CylinderThreadRatio", reinterpret_cast<float*>(&Desc.CylinderDesc.fThreadRatio));
	ImGui::InputFloat("CylinderSlowSpeed", &Desc.CylinderDesc.fSlowStrength);

	Desc.ParentMat = TrailMat;

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (Desc.ParentMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Swing"), TEXT("Prototype_GameObject_SwingEffect"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Swing"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Swing(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Swing()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Swing()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	Swing_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Swing(char* Name, CSwingEffect::SWINGEFFECT desc)
{
	string sName = Name;
	shared_ptr<CSwingEffect::SWINGEFFECT> StockValue = make_shared<CSwingEffect::SWINGEFFECT>(desc);
	m_Swings.emplace_back(StockValue);
	SwingNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Swing_ListBox(CSwingEffect::SWINGEFFECT* Swing)
{
#pragma region exception
	if (m_Swings.size() < 1)
		return;

	if (m_Swings.size() != SwingNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Swing_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception
#pragma region LISTBOX
	if (ImGui::BeginListBox("Swing_List", list_box_size))
	{
		for (int i = 0; i < SwingNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(SwingNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX
	if (current_item >= 0 && current_item < SwingNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CSwingEffect::SWINGEFFECT* Desc = m_Swings[current_item].get();
			Desc->ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Swing"), TEXT("Prototype_GameObject_SwingEffect"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Swing = *m_Swings[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Swings[current_item] = make_shared<CSwingEffect::SWINGEFFECT>(*Swing);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Swings[current_item].reset();
			m_Swings.erase(m_Swings.begin() + current_item);
			SwingNames.erase(SwingNames.begin() + current_item);

			if (current_item >= m_Swings.size())
				current_item = m_Swings.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Swings)
				iter.reset();
			m_Swings.clear();
			SwingNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_Swing()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Swings.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Swings.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Swings)
	{
		file.write((char*)iter.get(), sizeof(CSwingEffect::SWINGEFFECT));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Swings.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : SwingNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Swing.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < SwingNames.size(); ++i)
	{
		NumberFile << i << ". " << SwingNames[i] << std::endl;
	}
	NumberFile.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Swing()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Swings.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Swings)
		iter.reset();
	m_Swings.clear();
	SwingNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CSwingEffect::SWINGEFFECT readFile{};
		inFile.read((char*)&readFile, sizeof(CSwingEffect::SWINGEFFECT));
		readFile.ParentMat = nullptr;
		shared_ptr<CSwingEffect::SWINGEFFECT> StockValue = make_shared<CSwingEffect::SWINGEFFECT>(readFile);
		m_Swings.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Swings.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		SwingNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::FrameTextureTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("FrameTexture_Editor");

	static CTextureFrame::TEXFRAMEDESC classdesc{};

	ImGui::InputInt("Division", &classdesc.iDivision);
	classdesc.Texture = m_pTextureProtoName;
	classdesc.TexturePath = m_pTextureFilePath;
	ImGui::Checkbox("Color", &classdesc.State[0]);
	if (classdesc.State[0] == true)
	{
		ImGui::ColorEdit3("StartColor", reinterpret_cast<float*>(&classdesc.Color[0]));
		ImGui::ColorEdit3("EndColor", reinterpret_cast<float*>(&classdesc.Color[1]));
	}
	ImGui::Checkbox("Desolve", &classdesc.State[1]);
	if (classdesc.State[1] == true)
	{
		if (ImGui::InputInt("DesolveNum", &classdesc.iNumDesolve))
		{
			if (classdesc.iNumDesolve < 0)
				classdesc.iNumDesolve = 0;
			else if (classdesc.iNumDesolve > 15)
				classdesc.iNumDesolve = 15;
		}
	}
	ImGui::Checkbox("Blur", &classdesc.State[2]);
	if (classdesc.State[2] == true)
	{
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&classdesc.Color[2]));
		ImGui::InputFloat("Bloompower", &classdesc.Frame[2]);
	}
	ImGui::Checkbox("Billboard", &classdesc.State[3]);

	ImGui::InputFloat("MaxFrame", &classdesc.Frame[0]);
	ImGui::InputFloat("FrameSpeed", &classdesc.Frame[1]);

	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&classdesc.vSize));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&classdesc.vStartPos));

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (classdesc.Texture == TEXT("") || classdesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐를 먼저 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FrameTex"),
				TEXT("Prototype_GameObject_FrameTexture"), &classdesc);
		}
	}
	ImGui::End();
}

void CImguiMgr::Meteor_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Meteor_Editor", Open);

	static CMeteor::METEOR_DESC Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat3("StartOffset", reinterpret_cast<float*>(&Desc.fStartOffset));
	ImGui::InputFloat4("TargetPos", reinterpret_cast<float*>(&Desc.vTargetPos));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Core");

	ImGui::InputFloat3("Core_Size", reinterpret_cast<float*>(&Desc.CoreDesc.vMaxSize));
	ImGui::InputFloat("Core_LifeTime", &Desc.CoreDesc.fMaxLifeTime);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Wind");
	ImGui::InputFloat("Interval", &Desc.fWindInterval);
	ImGui::InputFloat("Wind_LifeTime", &Desc.WindDesc.fMaxLifeTime);
	ImGui::InputFloat3("Wind_Size", reinterpret_cast<float*>(&Desc.WindDesc.vSize));
	ImGui::ColorEdit3("Wind_Color", reinterpret_cast<float*>(&Desc.WindDesc.fColor));
	ImGui::ColorEdit3("Wind_B_Color", reinterpret_cast<float*>(&Desc.WindDesc.BloomColor));
	ImGui::InputFloat("Wind_B_Power", &Desc.WindDesc.fBloomPower);

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Cylinder");

	ImGui::InputFloat3("CylinderStartSize", reinterpret_cast<float*>(&Desc.CylinderDesc.vSize));
	ImGui::InputFloat3("CylinderMaxSize", reinterpret_cast<float*>(&Desc.CylinderDesc.vEndSized));
	ImGui::InputFloat3("CylinderOffset", reinterpret_cast<float*>(&Desc.CylinderDesc.vOffset));
	ImGui::ColorEdit3("CylinderColor", reinterpret_cast<float*>(&Desc.CylinderDesc.fColor));
	ImGui::ColorEdit3("CylinderBloomColor", reinterpret_cast<float*>(&Desc.CylinderDesc.BloomColor));
	ImGui::InputFloat("CylinderBloomPower", &Desc.CylinderDesc.fBloomPower);
	ImGui::InputFloat("CylinderLifeTime", &Desc.CylinderDesc.fMaxLifeTime);
	ImGui::InputFloat2("CylinderThreadRatio", reinterpret_cast<float*>(&Desc.CylinderDesc.fThreadRatio));
	ImGui::InputFloat("CylinderSlowSpeed", &Desc.CylinderDesc.fSlowStrength);

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Meteor"), TEXT("Prototype_GameObject_Meteor"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Meteor"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Meteor(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Meteor(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_Meteor(CMeteor::METEOR_DESC* pMeteor)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Meteor.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pMeteor, sizeof(CMeteor::METEOR_DESC));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Meteor(CMeteor::METEOR_DESC* pMeteor)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Meteor.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pMeteor, sizeof(CMeteor::METEOR_DESC));
	inFile.close();
	return S_OK;
}

void CImguiMgr::PhysX_Particle_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("PhysX_Particle", Open);

	static CParticle_PhysX::PARTICLE_PHYSXDESC Desc{};
#pragma region MODELTYPE
	if (ImGui::CollapsingHeader("Model_Type"))
	{
		if (ImGui::RadioButton("Cube", Desc.eModelType == EFFECTMODELTYPE::CUBE))
			Desc.eModelType = EFFECTMODELTYPE::CUBE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Circle", Desc.eModelType == EFFECTMODELTYPE::CIRCLE))
			Desc.eModelType = EFFECTMODELTYPE::CIRCLE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Slash", Desc.eModelType == EFFECTMODELTYPE::SLASH))
			Desc.eModelType = EFFECTMODELTYPE::SLASH;
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf0", Desc.eModelType == EFFECTMODELTYPE::LEAF0))
			Desc.eModelType = EFFECTMODELTYPE::LEAF0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf1", Desc.eModelType == EFFECTMODELTYPE::LEAF1))
			Desc.eModelType = EFFECTMODELTYPE::LEAF1;

		if (ImGui::RadioButton("Blade", Desc.eModelType == EFFECTMODELTYPE::BLADE_SLASH))
			Desc.eModelType = EFFECTMODELTYPE::BLADE_SLASH;
		ImGui::SameLine();
		if (ImGui::RadioButton("Blade_Long", Desc.eModelType == EFFECTMODELTYPE::BLADE_SLASH_LONG))
			Desc.eModelType = EFFECTMODELTYPE::BLADE_SLASH_LONG;
		ImGui::SameLine();
		if (ImGui::RadioButton("Grass", Desc.eModelType == EFFECTMODELTYPE::GRASS))
			Desc.eModelType = EFFECTMODELTYPE::GRASS;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock1", Desc.eModelType == EFFECTMODELTYPE::ROCK0))
			Desc.eModelType = EFFECTMODELTYPE::ROCK0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock2", Desc.eModelType == EFFECTMODELTYPE::ROCK1))
			Desc.eModelType = EFFECTMODELTYPE::ROCK1;

		if (ImGui::RadioButton("Needle", Desc.eModelType == EFFECTMODELTYPE::NEEDLE))
			Desc.eModelType = EFFECTMODELTYPE::NEEDLE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Bubble", Desc.eModelType == EFFECTMODELTYPE::BUBBLE))
			Desc.eModelType = EFFECTMODELTYPE::BUBBLE;
	}
#pragma endregion MODELTYPE
#pragma region DEFAULTVALUE
	if (ImGui::CollapsingHeader("DefaultValue"))
	{
		ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
		ImGui::Checkbox("Alpha", &Desc.bValues[0]);
		ImGui::Checkbox("ColorMap", &Desc.bValues[1]);
		if (Desc.bValues[1] == true)
		{
			ImGui::ColorEdit3("StartColor", reinterpret_cast<float*>(&Desc.vStartColor));
			ImGui::ColorEdit3("EndColor", reinterpret_cast<float*>(&Desc.vEndColor));
		}
		ImGui::Checkbox("Bloom", &Desc.bValues[2]);
		if (Desc.bValues[2] == true)
		{
			ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.vBloomColor));
			ImGui::InputFloat("BloomPower", &Desc.fBloomPower);
		}
		ImGui::Checkbox("Desolve", &Desc.bValues[3]);
		if (Desc.bValues[3] == true)
		{
			ImGui::ColorEdit3("DesolveColor", reinterpret_cast<float*>(&Desc.vDesolveColor));
			ImGui::InputFloat("DesolveLength", &Desc.fDesolveLength);
			ImGui::InputInt("NumDesolve", &Desc.NumDesolve);
		}
	}
#pragma endregion DEFAULTVALUE
#pragma region PhysX
	if (ImGui::CollapsingHeader("PhysX"))
	{
		ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &Desc.PhysXDesc.iNumInstance, NULL, NULL, "%u");

		ImGui::InputFloat3("Range", reinterpret_cast<float*>(&Desc.PhysXDesc.Range));
		ImGui::InputFloat3("Pivot", reinterpret_cast<float*>(&Desc.PhysXDesc.Pivot));
		ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.PhysXDesc.Offset));
		ImGui::InputFloat2("Size", reinterpret_cast<float*>(&Desc.PhysXDesc.Size));
		ImGui::InputFloat2("Velocity", reinterpret_cast<float*>(&Desc.PhysXDesc.Velocity));
		ImGui::InputFloat2("LifeTime", reinterpret_cast<float*>(&Desc.PhysXDesc.LifeTime));
		static _bool Detail = false;
		ImGui::Checkbox("Detail", &Detail);
		if (Detail)
		{
			ImGui::InputFloat("Energy", &Desc.PhysXDesc.Energy);
			ImGui::InputFloat("AirDrag", &Desc.PhysXDesc.AirDrag);
			ImGui::InputFloat("Threshold", &Desc.PhysXDesc.Threshold);
			ImGui::InputFloat("BubbleDrag", &Desc.PhysXDesc.BubbleDrag);
			ImGui::InputFloat("Buoyancy", &Desc.PhysXDesc.Buoyancy);
			ImGui::InputFloat("PressureWeight", &Desc.PhysXDesc.PressureWeight);
			ImGui::InputFloat("DivergenceWeight", &Desc.PhysXDesc.DivergenceWeight);
			ImGui::InputFloat("Viscosity", &Desc.PhysXDesc.Viscosity);
			ImGui::InputFloat("SurfaceTension", &Desc.PhysXDesc.SurfaceTension , 0.0f,0.0f, "%.5f");
			ImGui::InputFloat("Cohesion", &Desc.PhysXDesc.Cohesion, 0.0f,0.0f, "%.5f");
			ImGui::InputFloat("VorticityConfinement", &Desc.PhysXDesc.VorticityConfinement);
			ImGui::InputFloat("Friction", &Desc.PhysXDesc.Friction);
			ImGui::InputFloat("ContactOffset", &Desc.PhysXDesc.ContactOffset);
			ImGui::InputFloat("Damping", &Desc.PhysXDesc.Damping);

		}
	}
#pragma endregion PhysX

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_PhsyX"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}


	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_PhysX(effectname, Desc);
		}
	}



	ImGui::End();
}

HRESULT CImguiMgr::Store_PhysX(char* Name, CParticle_PhysX::PARTICLE_PHYSXDESC desc)
{
	string sName = Name;
	shared_ptr<CParticle_PhysX::PARTICLE_PHYSXDESC> StockValue = make_shared<CParticle_PhysX::PARTICLE_PHYSXDESC>(desc);
	m_PhysX.emplace_back(StockValue);
	PhysXNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::PhysX_ListBox(CParticle_PhysX::PARTICLE_PHYSXDESC* PhysX)
{
#pragma region exception
	if (m_PhysX.size() < 1)
		return;

	if (m_PhysX.size() != PhysXNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("PhysX_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception
#pragma region LISTBOX
	if (ImGui::BeginListBox("PhysX_List", list_box_size))
	{
		for (int i = 0; i < PhysXNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(PhysXNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX
	if (current_item >= 0 && current_item < PhysXNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CParticle_PhysX::PARTICLE_PHYSXDESC* Desc = m_PhysX[current_item].get();
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_PhsyX"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*PhysX = *m_PhysX[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_PhysX[current_item] = make_shared<CParticle_PhysX::PARTICLE_PHYSXDESC>(*PhysX);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_PhysX[current_item].reset();
			m_PhysX.erase(m_PhysX.begin() + current_item);
			PhysXNames.erase(PhysXNames.begin() + current_item);

			if (current_item >= m_PhysX.size())
				current_item = m_PhysX.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_PhysX)
				iter.reset();
			m_PhysX.clear();
			PhysXNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

void CImguiMgr::Hedgehog_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Hedgehog_Tool", Open);

	static CNeedleSpawner::NEEDLESPAWNER Desc{};

	if (ImGui::CollapsingHeader("Spawner"))
	{
		ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
		ImGui::InputFloat3("MinSize", reinterpret_cast<float*>(&Desc.vMinSize));
		ImGui::InputFloat3("MaxSize", reinterpret_cast<float*>(&Desc.vMaxSize));
		ImGui::InputFloat("LifeTime", &Desc.fMaxLifeTime);
		ImGui::InputFloat2("ThreadRatio", reinterpret_cast<float*>(&Desc.fThreadRatio));
		ImGui::InputFloat("SlowStrength", &Desc.fSlowStrength);
		ImGui::InputFloat("SpawnTiming", &Desc.SpawnTiming);
		ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.BloomColor));
		ImGui::InputFloat("BloomPower", &Desc.fBloomPower);
	}

	if (ImGui::CollapsingHeader("HedgeHog"))
	{
		ImGui::InputFloat4("H_StartPos", reinterpret_cast<float*>(&Desc.ChildDesc.vStartPos));
		ImGui::InputFloat3("H_Offset", reinterpret_cast<float*>(&Desc.ChildDesc.vOffset));
		ImGui::InputFloat3("H_Size", reinterpret_cast<float*>(&Desc.ChildDesc.vSize));
		ImGui::InputFloat3("H_MaxSize", reinterpret_cast<float*>(&Desc.ChildDesc.vMaxSize));
		ImGui::ColorEdit3("H_Color", reinterpret_cast<float*>(&Desc.ChildDesc.fColor));
		ImGui::ColorEdit3("H_BloomColor", reinterpret_cast<float*>(&Desc.ChildDesc.BloomColor));
		ImGui::InputFloat("H_BloomPower", &Desc.ChildDesc.fBloomPower);
		ImGui::InputFloat("H_LifeTime", &Desc.ChildDesc.fMaxLifeTime);
		ImGui::InputFloat2("H_Thread", reinterpret_cast<float*>(&Desc.ChildDesc.fThreadRatio));
		ImGui::InputInt("NumDesolve", &Desc.ChildDesc.NumDesolve);
	}

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_NeedleSpawner"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Hedgehog(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Hedgehog(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::End();
}

HRESULT CImguiMgr::Save_Hedgehog(CNeedleSpawner::NEEDLESPAWNER* pHedgehog)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HedgeHog.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pHedgehog, sizeof(CNeedleSpawner::NEEDLESPAWNER));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Hedgehog(CNeedleSpawner::NEEDLESPAWNER* pHedgehog)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HedgeHog.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pHedgehog, sizeof(CNeedleSpawner::NEEDLESPAWNER));
	inFile.close();
	return S_OK;
}

void CImguiMgr::GroundSlash_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("GroundSlash_Tool", Open);
	static CGroundSlash::GROUNDSLASH Desc{};
	//ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	//ImGui::InputFloat4("Direction", reinterpret_cast<float*>(&Desc.vDirection));
	ImGui::InputFloat("Speed", &Desc.fSpeed);
	ImGui::InputFloat3("StartSize", reinterpret_cast<float*>(&Desc.vStartSize));
	ImGui::InputFloat3("EndSize", reinterpret_cast<float*>(&Desc.vEndSize));
	ImGui::InputFloat("LifeTime", &Desc.fMaxLifeTime);
	ImGui::InputFloat("ThreadRatio", &Desc.fThreadRatio);
	ImGui::InputFloat("SlowStrength", &Desc.fSlowStrength);

	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.BloomColor));
	ImGui::InputFloat("BloomPower", &Desc.fBloomPower);


	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TrailMat == nullptr)
			MSG_BOX("행렬을 대입하세요");
		else
		{
			XMStoreFloat4(&Desc.vDirection, XMVector3Normalize(XMVectorSet(TrailMat->_31, TrailMat->_32, TrailMat->_33, 0.f)));
			XMStoreFloat4(&Desc.vStartPos, XMVectorSet(TrailMat->_41, TrailMat->_42, TrailMat->_43, 1.f));
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GroundSlash"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_GroundSlash(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_GroundSlash(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_GroundSlash(CGroundSlash::GROUNDSLASH* pGroundSlash)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/GroundSlash.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pGroundSlash, sizeof(CGroundSlash::GROUNDSLASH));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_GroundSlash(CGroundSlash::GROUNDSLASH* pGroundSlash)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/GroundSlash.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pGroundSlash, sizeof(CGroundSlash::GROUNDSLASH));
	inFile.close();
	return S_OK;
}

void CImguiMgr::HammerSpawn_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("HammerSpawn", Open);
	static CHammerSpawn::HAMMERSPAWN Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));

	if (ImGui::CollapsingHeader("Aspiration"))
	{
		ImGui::InputFloat3("A_Size", reinterpret_cast<float*>(&Desc.RibbonDesc.vSize));
		ImGui::ColorEdit3("A_Color", reinterpret_cast<float*>(&Desc.RibbonDesc.fColor));
		ImGui::ColorEdit3("A_BloomColor", reinterpret_cast<float*>(&Desc.RibbonDesc.BloomColor));
		ImGui::InputFloat("A_BloomPower", &Desc.RibbonDesc.fBloomPower);
		ImGui::InputFloat("A_LifeTime", &Desc.RibbonDesc.fMaxLifeTime);
	}
	if (ImGui::CollapsingHeader("Vane"))
	{
		ImGui::InputFloat3("V_Size", reinterpret_cast<float*>(&Desc.VaneDesc.vSize));
		ImGui::InputFloat3("V_Offset", reinterpret_cast<float*>(&Desc.VaneDesc.vOffset));
		ImGui::ColorEdit3("V_Color", reinterpret_cast<float*>(&Desc.VaneDesc.fColor));
		ImGui::ColorEdit3("V_BloomColor", reinterpret_cast<float*>(&Desc.VaneDesc.BloomColor));
		ImGui::InputFloat("V_BloomPower", &Desc.VaneDesc.fBloomPower);
		ImGui::InputFloat("V_LifeTime", &Desc.VaneDesc.fMaxLifeTime);
		ImGui::InputFloat2("ThreadRatio", reinterpret_cast<float*>(&Desc.VaneDesc.fThreadRatio));
		ImGui::InputFloat("SlowStrength", &Desc.VaneDesc.fSlowStrength);
	}

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_HammerSpawn"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_HammerSpawn(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_HammerSpawn(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_HammerSpawn(CHammerSpawn::HAMMERSPAWN* pHammer)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HammerSpawn.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pHammer, sizeof(CHammerSpawn::HAMMERSPAWN));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_HammerSpawn(CHammerSpawn::HAMMERSPAWN* pHammer)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HammerSpawn.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pHammer, sizeof(CHammerSpawn::HAMMERSPAWN));
	inFile.close();
	return S_OK;
}

void CImguiMgr::Shield_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Shield_Editor", Open);
	static CHexaShield::HEXASHIELD Desc{};

	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&Desc.vSize));
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));
	ImGui::InputFloat("UVSpeed", &Desc.fUVSpeed);
	ImGui::InputFloat("RotationSpeed", &Desc.RotationSpeed);
	ImGui::InputFloat("OpacitySpeed", &Desc.fOpacitySpeed);
	ImGui::InputFloat("OpacityPower", &Desc.OpacityPower);
	ImGui::InputFloat("LoopInterval", &Desc.LoopInterval);
	ImGui::InputFloat("DesolveTime", &Desc.DesolveTime);
	ImGui::InputFloat("HitTime", &Desc.HitTime);

	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.fBloomColor));
	ImGui::ColorEdit3("HitBloomColor", reinterpret_cast<float*>(&Desc.fBloomColor2));
	ImGui::InputFloat("BloomPower", &Desc.fBloomPower);

	if (ImGui::CollapsingHeader("Hit_Desc"))
	{
		ImGui::InputFloat3("Hit_Start_Size", reinterpret_cast<float*>(&Desc.HitDesc.vSize));
		ImGui::InputFloat3("Hit_End_Size", reinterpret_cast<float*>(&Desc.HitDesc.vMaxSize));
		ImGui::InputFloat3("Hit_Offset", reinterpret_cast<float*>(&Desc.HitDesc.vOffset));
		ImGui::InputFloat("Hit_LifeTime", &Desc.HitDesc.fLifeTime);
	}
#pragma region BUTTONS
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TrailMat == nullptr)
			MSG_BOX("행렬을 대입하세요");
		else
		{
			Desc.ParentMatrix = TrailMat;
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Shield"), TEXT("Prototype_GameObject_HexaShield"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Shield"));
	}

	if (ImGui::Button("Hit", ButtonSize))
	{
		CGameObject* Shield = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Shield"));
		if (Shield == nullptr)
			MSG_BOX("먼저 생성을 해주세요");
		else
		{
			static_cast<CHexaShield*>(Shield)->Set_Shield_Hit();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Desolve", ButtonSize))
	{
		CGameObject* Shield = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Shield"));
		if (Shield == nullptr)
			MSG_BOX("먼저 생성을 해주세요");
		else
		{
			static_cast<CHexaShield*>(Shield)->Set_Delete();
		}
	}


	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Shield(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Shield(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
#pragma endregion BUTTONS
	ImGui::End();
}

HRESULT CImguiMgr::Save_Shield(CHexaShield::HEXASHIELD* pShield)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HexaShield.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pShield, sizeof(CHexaShield::HEXASHIELD));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Shield(CHexaShield::HEXASHIELD* pShield)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/HexaShield.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pShield, sizeof(CHexaShield::HEXASHIELD));
	inFile.close();
	return S_OK;
}

void CImguiMgr::FireFly_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("FireFlyEditor", Open);
	static CFireFlyCube::FIREFLYCUBE Desc{};

	ImGui::InputFloat3("MinSize", reinterpret_cast<float*>(&Desc.fMinSize));
	ImGui::InputFloat3("MaxSize", reinterpret_cast<float*>(&Desc.fMaxSize));
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));

	ImGui::InputFloat("RotationSpeed", &Desc.fRotSpeed);
	ImGui::InputFloat("RotAngle", &Desc.fStartRotAngle);
	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat("StartDelay", &Desc.fStartdelay);
	ImGui::InputFloat("BloomInterval", &Desc.fBloomInterval);
	ImGui::InputFloat("BloomStartRatio", &Desc.fBloomIntervalStartRatio);
	ImGui::InputFloat("ChaseSpeed", &Desc.fChaseSpeed);
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.fBloomColor));

#pragma region BUTTONS
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TrailMat == nullptr)
			MSG_BOX("행렬을 대입하세요");
		else
		{
			Desc.ParentMatrix = TrailMat;
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_FireFly"), TEXT("Prototype_GameObject_FireFly"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FireFly"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_FireFly(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_FireFly(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
#pragma endregion BUTTONS
	ImGui::End();
}

HRESULT CImguiMgr::Save_FireFly(CFireFlyCube::FIREFLYCUBE* pFireFly)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/FireFly.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)pFireFly, sizeof(CFireFlyCube::FIREFLYCUBE));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_FireFly(CFireFlyCube::FIREFLYCUBE* pFireFly)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/FireFly.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)pFireFly, sizeof(CFireFlyCube::FIREFLYCUBE));
	inFile.close();
	return S_OK;
}

void CImguiMgr::BlackHole_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("BlackHole_Editor", Open);
	static CBlackHole::BLACKHOLE Desc{};

	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	ImGui::InputFloat("StartDelay", &Desc.vStartDelay);
	if (ImGui::CollapsingHeader("Sphere"))
	{
		ImGui::InputFloat3("Sphere_MinSize", reinterpret_cast<float*>(&Desc.SphereDesc.fMinSize));
		ImGui::InputFloat3("Sphere_MaxSize", reinterpret_cast<float*>(&Desc.SphereDesc.fMaxSize));
		ImGui::ColorEdit3("Sphere_Color", reinterpret_cast<float*>(&Desc.SphereDesc.fColor));
	}

	if (ImGui::CollapsingHeader("Ring"))
	{
		ImGui::InputFloat3("Ring_MinSize", reinterpret_cast<float*>(&Desc.RingDesc.vMinSize));
		ImGui::InputFloat3("Ring_MaxSize", reinterpret_cast<float*>(&Desc.RingDesc.vMaxSize));
		ImGui::InputFloat("UV_Speed", &Desc.RingDesc.fUVSpeed);
		ImGui::ColorEdit3("Ring_Color", reinterpret_cast<float*>(&Desc.RingDesc.vColor));
		ImGui::ColorEdit3("Ring_BloomColor", reinterpret_cast<float*>(&Desc.RingDesc.vBloomColor));
		ImGui::InputFloat("Ring_BloomPower", &Desc.RingDesc.fBloomPower);
		ImGui::InputFloat("Ring_RotAngle", &Desc.RingDesc.fRotAngle);
	}

	if (ImGui::CollapsingHeader("Ring_Billboard"))
	{
		ImGui::InputFloat3("Ring_Bill_MinSize", reinterpret_cast<float*>(&Desc.RingBill.vMinSize));
		ImGui::InputFloat3("Ring_Bill_MaxSize", reinterpret_cast<float*>(&Desc.RingBill.vMaxSize));
	}
	if (ImGui::CollapsingHeader("Horizon"))
	{
		ImGui::InputFloat3("Horizon_MinSize", reinterpret_cast<float*>(&Desc.HorizonDesc.vMinSize));
		ImGui::InputFloat3("Horizon_MaxSize", reinterpret_cast<float*>(&Desc.HorizonDesc.vMaxSize));
		ImGui::ColorEdit3("Horizon_Color", reinterpret_cast<float*>(&Desc.HorizonDesc.vColor));
		ImGui::ColorEdit3("Horizon_BloomColor", reinterpret_cast<float*>(&Desc.HorizonDesc.vBloomColor));
		ImGui::InputFloat("Horizon_BloomPower", &Desc.HorizonDesc.fBloomPower);
	}
#pragma region BUTTONS
	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_BlackHole"), TEXT("Prototype_GameObject_BlackHole"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BlackHole"));
	}

	if (ImGui::Button("Extinct_BlackHole", ButtonSize))
	{
		CGameObject* BlackHole = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BlackHole"));
		if (BlackHole != nullptr)
		{
			static_cast<CBlackHole*>(BlackHole)->Set_Delete_BlackHole();
		}
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_BlackHole(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_BlackHole()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_BlackHole()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

#pragma endregion BUTTONS

	BlackHole_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_BlackHole(char* Name, CBlackHole::BLACKHOLE desc)
{
	string sName = Name;
	shared_ptr<CBlackHole::BLACKHOLE> StockValue = make_shared<CBlackHole::BLACKHOLE>(desc);
	m_BlackHole.emplace_back(StockValue);
	BlackHoleNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::BlackHole_ListBox(CBlackHole::BLACKHOLE* BlackHole)
{
#pragma region exception
	if (m_BlackHole.size() < 1)
		return;

	if (m_BlackHole.size() != BlackHoleNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("BlackHole_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception

#pragma region LISTBOX
	if (ImGui::BeginListBox("BlackHole_List", list_box_size))
	{
		for (int i = 0; i < BlackHoleNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(BlackHoleNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX

	if (current_item >= 0 && current_item < BlackHoleNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CBlackHole::BLACKHOLE* Desc = m_BlackHole[current_item].get();
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_BlackHole"), TEXT("Prototype_GameObject_BlackHole"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*BlackHole = *m_BlackHole[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_BlackHole[current_item] = make_shared<CBlackHole::BLACKHOLE>(*BlackHole);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_BlackHole[current_item].reset();
			m_BlackHole.erase(m_BlackHole.begin() + current_item);
			BlackHoleNames.erase(BlackHoleNames.begin() + current_item);

			if (current_item >= m_BlackHole.size())
				current_item = m_BlackHole.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_BlackHole)
				iter.reset();
			m_BlackHole.clear();
			BlackHoleNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_BlackHole()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/BlackHole.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_BlackHole.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_BlackHole)
	{
		file.write((char*)iter.get(), sizeof(CBlackHole::BLACKHOLE));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/BlackHoles.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : BlackHoleNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/BlackHoles.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < BlackHoleNames.size(); ++i)
	{
		NumberFile << i << ". " << BlackHoleNames[i] << std::endl;
	}
	NumberFile.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_BlackHole()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/BlackHole.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_BlackHole)
		iter.reset();
	m_BlackHole.clear();
	BlackHoleNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CBlackHole::BLACKHOLE readFile{};
		inFile.read((char*)&readFile, sizeof(CBlackHole::BLACKHOLE));
		shared_ptr<CBlackHole::BLACKHOLE> StockValue = make_shared<CBlackHole::BLACKHOLE>(readFile);
		m_BlackHole.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/BlackHoles.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		BlackHoleNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::WellCylinder_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("WellCylinder_Editor", Open);
	static CWellCylinder::WELLCYLINDER Desc{};

	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&Desc.vSize));
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
	ImGui::ColorEdit3("Bloom_Color", reinterpret_cast<float*>(&Desc.BloomColor));
	ImGui::InputFloat("Bloom_Power", &Desc.fBloomPower);

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TrailMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			Desc.ParentMatrix = TrailMat;
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WellCylinder"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_WellCylinder(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_WellCylinder(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_WellCylinder(CWellCylinder::WELLCYLINDER* WellCylinder)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/WellCylinder.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)WellCylinder, sizeof(CWellCylinder::WELLCYLINDER));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_WellCylinder(CWellCylinder::WELLCYLINDER* WellCylinder)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/WellCylinder.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)WellCylinder, sizeof(CWellCylinder::WELLCYLINDER));
	inFile.close();
	return S_OK;
}

void CImguiMgr::Magic_Cast_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Magic_Cast_Editor", Open);
	static CMagicCast::MAGIC_CAST Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fMaxLifeTime);
	ImGui::InputFloat2("ThreadRatio", reinterpret_cast<float*>(&Desc.fThreadRatio));
	ImGui::InputFloat("SlowStrength", &Desc.fSlowStrength);

	if (ImGui::CollapsingHeader("Helix_Cast"))
	{
		ImGui::Checkbox("IsHelixOn", &Desc.isHelix);
		ImGui::InputFloat3("Helix_Size", reinterpret_cast<float*>(&Desc.HelixDesc.vSize));
		ImGui::ColorEdit3("Helix_Color", reinterpret_cast<float*>(&Desc.HelixDesc.fColor));
		ImGui::ColorEdit3("Helix_BloomColor", reinterpret_cast<float*>(&Desc.HelixDesc.BloomColor));
		ImGui::InputFloat("Helix_BloomPower", &Desc.HelixDesc.fBloomPower);
	}

	if (ImGui::CollapsingHeader("Bezier_Curve"))
	{
		ImGui::Checkbox("IsBezier", &Desc.isBezierCurve);
		ImGui::InputFloat3("Bezier_Size", reinterpret_cast<float*>(&Desc.BezierDesc.vSize));
		ImGui::ColorEdit3("Bezier_Color", reinterpret_cast<float*>(&Desc.BezierDesc.fColor));
		ImGui::ColorEdit3("Bezier_BloomColor", reinterpret_cast<float*>(&Desc.BezierDesc.BloomColor));
		ImGui::InputFloat("Bezier_BloomPower", &Desc.BezierDesc.fBloomPower);
		ImGui::InputFloat("Bezier_RotSpeed", &Desc.BezierDesc.fRotSpeed);

	}

	if (ImGui::CollapsingHeader("Aspiration"))
	{
		ImGui::Checkbox("IsAspiration", &Desc.isAspiration);
		ImGui::InputFloat3("Asp_Size", reinterpret_cast<float*>(&Desc.AspDesc.vSize));
		ImGui::InputFloat3("Asp_Offset", reinterpret_cast<float*>(&Desc.AspDesc.vOffset));
		ImGui::ColorEdit3("Asp_Color", reinterpret_cast<float*>(&Desc.AspDesc.fColor));
		ImGui::ColorEdit3("Asp_BloomColor", reinterpret_cast<float*>(&Desc.AspDesc.BloomColor));
		ImGui::InputFloat("Asp_BloomPower", &Desc.AspDesc.fBloomPower);
	}

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TrailMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			Desc.ParentMatrix = TrailMat;
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Magic_Cast"), &Desc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_MagicCast(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_MagicCast()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_MagicCast()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	Magic_Cast_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_MagicCast(char* Name, CMagicCast::MAGIC_CAST desc)
{
	string sName = Name;
	shared_ptr<CMagicCast::MAGIC_CAST> StockValue = make_shared<CMagicCast::MAGIC_CAST>(desc);
	m_MagicCast.emplace_back(StockValue);
	MagicCastNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Magic_Cast_ListBox(CMagicCast::MAGIC_CAST* Magic)
{
#pragma region exception
	if (m_MagicCast.size() < 1)
		return;

	if (m_MagicCast.size() != MagicCastNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Magic_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception
#pragma region LISTBOX
	if (ImGui::BeginListBox("Magic_List", list_box_size))
	{
		for (int i = 0; i < MagicCastNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(MagicCastNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma endregion LISTBOX

	if (current_item >= 0 && current_item < MagicCastNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			if (TrailMat == nullptr)
				MSG_BOX("행렬 대입해주세요");
			else
			{
				CMagicCast::MAGIC_CAST* Desc = m_MagicCast[current_item].get();
				Desc->ParentMatrix = TrailMat;
				m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
					TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Magic_Cast"), Desc);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Magic = *m_MagicCast[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_MagicCast[current_item] = make_shared<CMagicCast::MAGIC_CAST>(*Magic);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_MagicCast[current_item].reset();
			m_MagicCast.erase(m_MagicCast.begin() + current_item);
			MagicCastNames.erase(MagicCastNames.begin() + current_item);

			if (current_item >= m_MagicCast.size())
				current_item = m_MagicCast.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_MagicCast)
				iter.reset();
			m_MagicCast.clear();
			MagicCastNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_MagicCast()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/MagicCast.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_MagicCast.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_MagicCast)
	{
		file.write((char*)iter.get(), sizeof(CMagicCast::MAGIC_CAST));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/MagicCast.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : MagicCastNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/MagicCast.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < MagicCastNames.size(); ++i)
	{
		NumberFile << i << ". " << MagicCastNames[i] << std::endl;
	}
	NumberFile.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_MagicCast()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/MagicCast.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_MagicCast)
		iter.reset();
	m_MagicCast.clear();
	MagicCastNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CMagicCast::MAGIC_CAST readFile{};
		inFile.read((char*)&readFile, sizeof(CMagicCast::MAGIC_CAST));
		shared_ptr<CMagicCast::MAGIC_CAST> StockValue = make_shared<CMagicCast::MAGIC_CAST>(readFile);
		StockValue->ParentMatrix = nullptr;
		m_MagicCast.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/MagicCast.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		MagicCastNames.emplace_back(str);
	}
	NameFile.close();
}

void CImguiMgr::Andras_CutScene_Tool(_bool* Open)
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("AndrasCutSceneEditor", Open);

	static CutSceneAndras::CUTSCENEANDRAS Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat("StartEffect", &Desc.fStartEffect);
	ImGui::InputFloat2("EyeChangeDelay", reinterpret_cast<float*>(&Desc.EyeChangeBetwin));
	ImGui::InputFloat4("Position", reinterpret_cast<float*>(&Desc.vPosition));
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.vColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.vBloomColor));

	if (ImGui::CollapsingHeader("DefaultCylinder"))
	{
		ImGui::InputFloat3("Cylinder_Size", reinterpret_cast<float*>(&Desc.Cylinder.vSize));
		ImGui::InputFloat3("Cylinder_EndSize", reinterpret_cast<float*>(&Desc.Cylinder.vEndSized));
		ImGui::InputFloat3("Cylinder_Offset", reinterpret_cast<float*>(&Desc.Cylinder.vOffset));
		ImGui::ColorEdit3("Cylinder_Color", reinterpret_cast<float*>(&Desc.Cylinder.fColor));
		ImGui::ColorEdit3("Cylinder_BColor", reinterpret_cast<float*>(&Desc.Cylinder.BloomColor));
		ImGui::InputFloat("Cylinder_BloomPower", &Desc.Cylinder.fBloomPower);

		ImGui::InputFloat("Cylinder_LifeTime", &Desc.Cylinder.fMaxLifeTime);
		ImGui::InputFloat2("Cylinder_Thread", reinterpret_cast<float*>(&Desc.Cylinder.fThreadRatio));
		ImGui::InputFloat("Cylinder_SlowStrength", &Desc.Cylinder.fSlowStrength);
	}

	if (ImGui::CollapsingHeader("AndrasPillar"))
	{
		ImGui::InputFloat3("Andras_Pillar_Size", reinterpret_cast<float*>(&Desc.AndrasPillar.vSize));
		ImGui::InputFloat3("Andras_Pillar_EndSize", reinterpret_cast<float*>(&Desc.AndrasPillar.vEndSized));
		ImGui::ColorEdit3("Andras_Color", reinterpret_cast<float*>(&Desc.AndrasPillar.fColor));
		ImGui::ColorEdit3("Andras_BColor", reinterpret_cast<float*>(&Desc.AndrasPillar.BloomColor));
		ImGui::InputFloat("Andras_BloomPower", &Desc.AndrasPillar.fBloomPower);
		ImGui::InputFloat("Andras_UVSpeed", &Desc.AndrasPillar.fUVSpeed);
	}

	if (ImGui::CollapsingHeader("AndrasSphere"))
	{
		ImGui::InputFloat("Sphere_LifeTime", &Desc.SphereDesc.fMaxLifeTime);
		ImGui::InputFloat3("Sphere_Pillar_Size", reinterpret_cast<float*>(&Desc.SphereDesc.vSize));
		ImGui::InputFloat3("Sphere_Pillar_EndSize", reinterpret_cast<float*>(&Desc.SphereDesc.vEndSized));
		ImGui::InputFloat("Sphere_BloomPower", &Desc.SphereDesc.fBloomPower);
		ImGui::InputFloat("Sphere_UVSpeed", &Desc.SphereDesc.fUVSpeed);
		ImGui::InputFloat2("Sphere_Thread", reinterpret_cast<float*>(&Desc.SphereDesc.fThreadRatio));
		ImGui::InputFloat("Sphere_SlowStrength", &Desc.SphereDesc.fSlowStrength);
		ImGui::InputFloat("Sphere_Oppacity", &Desc.SphereDesc.OppacityPower);
	}



	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_CutSceneAndras"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}


	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_CutScene(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_CutScene(&Desc)))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_CutScene(CutSceneAndras::CUTSCENEANDRAS* Cut)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/AndrasCutScene.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	file.write((char*)Cut, sizeof(CutSceneAndras::CUTSCENEANDRAS));
	file.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_CutScene(CutSceneAndras::CUTSCENEANDRAS* Cut)
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/AndrasCutScene.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	inFile.read((char*)Cut, sizeof(CutSceneAndras::CUTSCENEANDRAS));
	inFile.close();
	return S_OK;
}

void CImguiMgr::CenteredTextColored(const ImVec4& color, const char* text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text).x;
	float textIndent = (windowWidth - textWidth) / 2.0f;
	if (textIndent < 0.0f)
		textIndent = 0.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textIndent);
	ImGui::TextColored(color, "%s", text);
}

HRESULT CImguiMgr::ConvertToDDSWithMipmap(const string& inputFilePath, const string& outputFilePath)
{
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	HRESULT hr;

	// 이미지 로드
	hr = DirectX::LoadFromWICFile(utf8_to_wstring(inputFilePath).c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		MSG_BOX("Failed to load image: ");
		return hr;
	}

	// mipmap 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		MSG_BOX("Failed to generate mipmaps, Proceeding with original image. ");
		mipChain = std::move(image);
	}

	// 새로운 파일명 생성: 기존 확장자 .png 제거 후 .dds 추가
	std::filesystem::path outputPath = std::filesystem::path(outputFilePath) / std::filesystem::path(inputFilePath).filename().replace_extension(".dds");

	// DDS 파일로 저장
	hr = DirectX::SaveToDDSFile(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DirectX::DDS_FLAGS_NONE, outputPath.c_str());
	if (FAILED(hr)) {
		MSG_BOX("Failed to save DDS file: ");
		return hr;
	}

	return S_OK;
}

HRESULT CImguiMgr::ConvertToDDSWithMipmap_PathHere(const string& inputFilePath)
{
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	HRESULT hr;

	// 이미지 로드
	hr = DirectX::LoadFromWICFile(utf8_to_wstring(inputFilePath).c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		MSG_BOX("Failed to load image");
		return hr;
	}

	// mipmap 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		MSG_BOX("Failed to generate mipmaps, Proceeding with original image.");
		mipChain = std::move(image);
	}

	// 출력 경로 설정
	std::filesystem::path inputPath(inputFilePath);
	std::filesystem::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + ".dds");

	// DDS 파일로 저장
	hr = DirectX::SaveToDDSFile(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DirectX::DDS_FLAGS_NONE, outputPath.c_str());
	if (FAILED(hr)) {
		MSG_BOX("Failed to save DDS file: ");
		return hr;
	}
	return S_OK;
}

vector<string> CImguiMgr::GetFilesInDirectory(const string& path)
{
	vector<std::string> files;
	for (const auto& entry : filesystem::directory_iterator(path)) {

		files.push_back(entry.path().filename().string());
	}
	return files;
}

vector<string> CImguiMgr::GetFilesTexture(const string& path)
{
	vector<string> imageFiles;
	for (const auto& entry : filesystem::directory_iterator(path)) {
		if (entry.is_regular_file())
		{
			string filePath = entry.path().string();
			if (IsTextureFile(filePath))
			{
				imageFiles.push_back(filePath);
			}
		}
	}
	return imageFiles;
}

void CImguiMgr::Erase_Particle(_int index)
{
	if (index >= m_Types.size())
		return;
	switch (m_Types[index].first)
	{
	case PART_POINT:
		delete ((CParticle_Point::PARTICLEPOINT*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	case PART_MESH:
		delete ((CParticleMesh::PARTICLEMESH*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	case PART_RECT:
		delete ((CParticle_Rect::PARTICLERECT*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	default:
		break;
	}
}

CImguiMgr* CImguiMgr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImguiMgr* pInstance = new CImguiMgr();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed To Created : ImguiMgr");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImguiMgr::Free()
{
	for (int i = 0; i < m_Types.size(); ++i)
		Erase_Particle(i);

	m_Types.clear();

	TrailEffects.clear();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pGameInstance);
}

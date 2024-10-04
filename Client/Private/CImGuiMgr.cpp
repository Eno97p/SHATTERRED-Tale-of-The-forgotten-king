#include"stdafx.h"

#include "GameInstance.h"
#include"Object_Manager.h"
#include"CImGuiMgr.h"

#pragma region LEVEL_HEADER
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Level_GamePlay.h"
#pragma endregion LEVEL_HEADER



#include"CProfiler.h"


#ifdef _DEBUG
IMPLEMENT_SINGLETON(CImGuiMgr)



_bool CImGuiMgr::FrameLimit = true;

CImGuiMgr::CImGuiMgr()
{
}


HRESULT CImGuiMgr::Ready_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.MousePos;


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);


	

	return S_OK;
}

HRESULT CImGuiMgr::Render_ImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	Update_ImGui();

	ImGui::Render();

	return S_OK;
}

HRESULT CImGuiMgr::Update_ImGui()
{
	ImGui::Begin("Property", nullptr, ImGuiWindowFlags_HorizontalScrollbar/*| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize*/);
	ImGui::Text("Frame:%f", ImGui::GetIO().Framerate);
	ImGui::Checkbox("Frame Limit", &FrameLimit);
	Render_MainMenu();
	ImGui::End();




	ImGui::Begin("Profile", nullptr, ImGuiWindowFlags_HorizontalScrollbar/*| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize*/);
	Render_Profile();
	ImGui::End();

	ImGui::Begin("Memory", nullptr, ImGuiWindowFlags_HorizontalScrollbar/*| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize*/);
	Render_Memory();

	ImGui::End();

	
	return S_OK;
}

HRESULT CImGuiMgr::End_ImGui()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	return S_OK;
}

void CImGuiMgr::Render_MainMenu()
{
	LEVEL NowLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	//m_pGameInstance->Get_Layer(NowLevel);


	unordered_map<LEVEL, const char*> LevelName = []() {
		unordered_map<LEVEL, const char*> map;
		for (int i = 0; i < sizeof(Client::LevelNames) / sizeof(Client::LevelNames[0]); ++i)
		{
			map[static_cast<LEVEL>(i)] = Client::LevelNames[i];
		}
		return map;
		}();


	vector<const char*> level_names;
	for (const auto& level : LevelName)
	{
		level_names.push_back(level.second);
	}
	int current_level_index = 0;
	for (const auto& level : LevelName)
	{
		if (level.first == NowLevel)
		{
			break;
		}
		current_level_index++;
		m_iCurrentLevel = current_level_index;
	}
	
	

	//list<CGameObject*> ObjectLis;


	//m_pGameInstance->Get_GameObjects_Ref(current_level_index, m_LayerTags[0], &ObjectLis);
		
	
	if (ImGui::Combo("Levels", &current_level_index, level_names.data(), level_names.size())) 
	{
		
		
		auto it = LevelName.begin();
		std::advance(it, current_level_index);
		// 선택하면 해당 레벨 인덱스로 이터레이터를 이동
		LEVEL selected_level = it->first;
		if (NowLevel != selected_level)
		{
			// 레벨 변경 로직
			m_pGameInstance->Set_NextLevel(selected_level);
			switch (selected_level)
			{
			case LEVEL_STATIC:
	
				break;
			case LEVEL_LOADING:
				break;
			case LEVEL_LOGO:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_GAMEPLAY:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_ACKBAR:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ACKBAR))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_JUGGLAS:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_JUGGLAS))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_ANDRASARENA:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ANDRASARENA))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_GRASSLAND:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GRASSLAND))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
			case LEVEL_END:
				
				break;
			default:
				break;
			}
			NowLevel = selected_level;
			return;
			
		}
	}
	
	if (ImGui::Button("Reset Current Level"))
	{

	//	m_pNewLevel = CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iCurrentLevel);
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iCurrentLevel))))
		{
			MSG_BOX("IMGUI::Failed to Open Level");
			return;
		}

		
		//int temp = 0;
		return;
	}

	//bool allThreadsFinished = true;
	//for (_uint i = 0; i < USED_THREAD_COUNT; ++i)
	//{
	//	if (g_IsThreadFinish[i] == false)
	//	{
	//		allThreadsFinished = false;
	//		break;
	//	}
	//}

	//if (allThreadsFinished)
	//{
	//	if (m_pNewLevel != nullptr)
	//	{
	//		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, m_pNewLevel)))
	//		{
	//			MSG_BOX("IMGUI::Failed to Open Level");
	//			return;
	//		}
	//	}
	//	
	//	int test = 0;
	//}



	m_pGameInstance->Get_LayerTags_String(m_iCurrentLevel, &m_LayerTags);
	
	Render_Layer();





}

void CImGuiMgr::Render_Layer()
{
	list<CGameObject*> ObjectLis;
	//m_LayerTags


	ImGui::Combo("Layers", &m_iCurrentLayer, m_LayerTags.data(), m_LayerTags.size());
	//if (ImGui::Combo("Layers", &m_iCurrentLayer,m_LayerTags.data(), m_LayerTags.size()))
	{
		
		if (m_iCurrentLayer >= 0 && m_iCurrentLayer < m_LayerTags.size())
		{

		}
	}
	_uint iLayerSize = m_LayerTags.size();


	if(m_LayerTags.empty()|| iLayerSize<= m_iCurrentLayer)
		return;

		m_wstrLayerTag = Client::const_char_to_wstring(m_LayerTags[m_iCurrentLayer]);
		ObjectLis = m_pGameInstance->Get_GameObjects_Ref(m_iCurrentLevel, m_wstrLayerTag);

	Render_Object(ObjectLis);





}

void CImGuiMgr::Render_Object(list<CGameObject*>& listGameObject)
{
	size_t iObjectCount = listGameObject.size();
	auto iter = listGameObject.begin();
	
	


	if(m_LayerTags.empty())
		return;

	vector<string> ObjectNamesString;


	for (size_t i = 0; i < iObjectCount; ++i, ++iter)
	{
		const std::type_info& info = typeid((**iter));
		const char* szClassname = info.name();
		
		const char* szClassCutName= strstr(szClassname, "class");
		if (szClassCutName)
			szClassname = szClassCutName + 6;
		else
			szClassname = szClassCutName;

		string FullClassName = string(szClassname) + " :" + to_string(i);

		ObjectNamesString.push_back(FullClassName);

	}
	vector<const char*> ObjectNames;

	ObjectNames.reserve(ObjectNamesString.size());
	for (auto& ObjectName : ObjectNamesString)
	{
		ObjectNames.push_back(ObjectName.c_str());
	}


	
	//CGameObject* pGameObject = nullptr;
	ImGui::Combo("Objects", &m_iCurrentObject, ObjectNames.data(), ObjectNames.size());

	if (listGameObject.size()!=0)
	{
		iter = listGameObject.begin();

		if (m_iCurrentObject >= listGameObject.size())
		{
			m_iCurrentObject = 0;
		}


		std::advance(iter, m_iCurrentObject);
		m_pSelectedObject = *iter;




		//if()

		Render_Component(m_pSelectedObject);
	}



}

void CImGuiMgr::Render_Component(CGameObject* pGameObject)
{
	vector<const char*> ComponentNames;
	

	if (nullptr == pGameObject)
		return;

	map<const wstring, CComponent*> Components;
	Components = pGameObject->Get_Components();


	m_ComponentNames.clear();
	m_ComponentNames.reserve(Components.size());

	for (auto& Component : Components)
	{
		string ComponentName = wstring_to_string(Component.first);
		m_ComponentNames.push_back(ComponentName);
		ComponentNames.push_back(m_ComponentNames.back().c_str());

	}

	
	if (ImGui::Combo("Components", &m_iCurrentComponent, ComponentNames.data(), ComponentNames.size()))
	{

		wstring ComponentTag = Client::const_char_to_wstring(ComponentNames[m_iCurrentComponent]);
		m_bComponentPanel = true;
		//Render_Component_Properties(Components[ComponentTag]);
	}
	if(m_iCurrentComponent >= ComponentNames.size())
	{
		m_iCurrentComponent = 0;
	}

	Render_Component_Properties(Components[Client::const_char_to_wstring(ComponentNames[m_iCurrentComponent])], ComponentNames[m_iCurrentComponent]);

}

void CImGuiMgr::Render_Component_Properties(CComponent* pComponent, const char* szComponentName)
{

	//컴포넌트마다 다른 데이터를 반환하고 있음
	//각자 데이터에 맞는 UI를 그려줘야함


	if (m_bComponentPanel)
	{
		string ComponentName = string(szComponentName)+ " Properties";

		ImGui::Begin(ComponentName.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		
	auto&& tmep= pComponent->GetData();
		
	if (tmep)
	{
		if (typeid(*pComponent) == typeid(CTransform))
		{
			CTransform::Transform_Editable_Desc* tDesc = static_cast<CTransform::Transform_Editable_Desc*>(tmep);
			ImGui::Text("World Matrix:");
			//ImGui::Text("  [%f, %f, %f, %f]", tDesc->pWorldMatrix->m[0][0], tDesc->pWorldMatrix->m[0][1], tDesc->pWorldMatrix->m[0][2], tDesc->pWorldMatrix->m[0][3]);
			//ImGui::Text("  [%f, %f, %f, %f]", tDesc->pWorldMatrix->m[1][0], tDesc->pWorldMatrix->m[1][1], tDesc->pWorldMatrix->m[1][2], tDesc->pWorldMatrix->m[1][3]);
			//ImGui::Text("  [%f, %f, %f, %f]", tDesc->pWorldMatrix->m[2][0], tDesc->pWorldMatrix->m[2][1], tDesc->pWorldMatrix->m[2][2], tDesc->pWorldMatrix->m[2][3]);
			//ImGui::Text("  [%f, %f, %f, %f]", tDesc->pWorldMatrix->m[3][0], tDesc->pWorldMatrix->m[3][1], tDesc->pWorldMatrix->m[3][2], tDesc->pWorldMatrix->m[3][3]);

			ImGui::InputFloat4("Row 1", reinterpret_cast<float*>(&tDesc->pWorldMatrix->m[0]));
			ImGui::InputFloat4("Row 2", reinterpret_cast<float*>(&tDesc->pWorldMatrix->m[1]));
			ImGui::InputFloat4("Row 3", reinterpret_cast<float*>(&tDesc->pWorldMatrix->m[2]));
			ImGui::InputFloat4("Row 4", reinterpret_cast<float*>(&tDesc->pWorldMatrix->m[3]));
			//tDesc->pWorldMatrix

			
		}
		else if (typeid(*pComponent) == typeid(CPhysXComponent))
		{
			CPhysXComponent::PhysX_Editable_Desc* tDesc = static_cast<CPhysXComponent::PhysX_Editable_Desc*>(tmep);
			ImGui::Checkbox("Is On Debug Render", &tDesc->bIsOnDebugRender);
			//pComponent->Addtest()
			
			int tmep = 0;


		}
		else if (typeid(*pComponent) == typeid(CPhysXComponent_static))
		{
			CPhysXComponent_static::PhysX_static_Editable_Desc* tDesc = static_cast<CPhysXComponent_static::PhysX_static_Editable_Desc*>(tmep);
			ImGui::Checkbox("Is On Debug Render", &tDesc->bIsOnDebugRender);
			

		}
		else if (typeid(*pComponent) == typeid(CPhysXComponent_Vehicle))
		{
			CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* tDesc = static_cast<CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc*>(tmep);
			ImGui::Text("Vehicle Properties:");
			ImGui::Text("Vehicle CurSpeed: %f", tDesc->pPhysXActorVehicle->getActor()->getLinearVelocity().magnitude());

			ImGui::Text("Vehicle Scale: %f", tDesc->pPhysXActorVehicle->mBaseParams.scale.scale);
			ImGui::InputFloat("Vehicle HorsePower", &tDesc->pPhysXActorVehicle->getDirectDriveParams().directDriveThrottleResponseParams.maxResponse);
			_uint Wheels = tDesc->pPhysXActorVehicle->mBaseParams.axleDescription.getNbWheels();

			vector<const char*> VehiclePropertyNames = { "RigidBody_Property", "Trie_Property", "Wheel_Property", "Steer_Property", "Wheel Damping Rate" };


			ImGui::Combo("Vehicle_Property", &m_iCurrentComponentVehicleProperty, VehiclePropertyNames.data(), VehiclePropertyNames.size());
			
				switch (m_iCurrentComponentVehicleProperty)
				{
					case 0:
						this->RigidBody_Property(tDesc);
						break;
					case 1:
						this->Tire_Property(tDesc);
						break;
					case 2:
						this->Wheel_Property(tDesc);
						break;
					case 3:
						this->Steering_Property(tDesc);
						break;


				default:
					break;
				}


				if (ImGui::Button("Save_Property"))
				{
					//L"../../Data/Test.dat"
					Engine::Save_Data(L"../Bin/DataFiles/Vehicle.dat", true,tDesc->pPhysXActorVehicle->mBaseParams, tDesc->pPhysXActorVehicle->getDirectDriveParams());
					
				}


		}


		else if (typeid(*pComponent) == typeid(CPhysXComponent_Character))
		{
			CPhysXComponent_Character::PhysX_Character_Editable_Desc* tDesc = static_cast<CPhysXComponent_Character::PhysX_Character_Editable_Desc*>(tmep);

			ImGui::Checkbox("Is On Debug Render", &tDesc->bIsOnDebugRender);
			ImGui::InputFloat3("Position", reinterpret_cast<float*>(&tDesc->fPosition));
			tDesc->pController->setFootPosition(PxExtendedVec3(tDesc->fPosition.x, tDesc->fPosition.y, tDesc->fPosition.z));
			
			
		}
		else if (typeid(*pComponent) == typeid(CShader))
		{

			CShader::Shader_Editable_Desc* tDesc = static_cast<CShader::Shader_Editable_Desc*>(tmep);
			ImGui::Text("Shader Pass Count: %zu", tDesc->PassNames.size());
			int currentPassValue=static_cast<int>(tDesc->iCurrentPass);
			ImGui::Text("Current Pass: %d", currentPassValue);
		
			for(const auto& passName : tDesc->PassNames)
			{
				string PassNameStr= wstring_to_string(passName);
				ImGui::Text("Pass: %s", PassNameStr.c_str());
			}

		}
		else if (typeid(*pComponent) == typeid(CCollider))
		{
			CCollider::Collider_Editable_Desc* tDesc = static_cast<CCollider::Collider_Editable_Desc*>(tmep);
			ImGui::Checkbox("Is On Debug Render", &tDesc->bIsOnDebugRender);


		}
	
	}
		if (ImGui::Button("Close"))
		{
			m_bComponentPanel = false;
		}
		ImGui::End();
	}


}

void CImGuiMgr::Render_Profile()
{

	auto Result = CProfiler::GetInstance().GetProfileResults();
	for(const auto& profile : Result)
	{
		ImGui::Text("%s: %.4f ms", profile.first.c_str(), profile.second);
	}




}

void CImGuiMgr::Render_Memory()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	

	if (GlobalMemoryStatusEx(&statex))
	{
		// 전체 물리적 메모리
		ImGui::Text("Total physical memory: %.2f GB", statex.ullTotalPhys / (1024.0 * 1024.0 * 1024.0));

		// 사용 가능한 물리적 메모리
		ImGui::Text("Available physical memory: %.2f GB", statex.ullAvailPhys / (1024.0 * 1024.0 * 1024.0));

		// 전체 가상 메모리 (RAM + 페이징 파일)
		ImGui::Text("Total virtual memory: %.2f GB", statex.ullTotalPageFile / (1024.0 * 1024.0 * 1024.0));

		// 사용 가능한 가상 메모리 (RAM + 페이징 파일)
		ImGui::Text("Available virtual memory: %.2f GB", statex.ullAvailPageFile / (1024.0 * 1024.0 * 1024.0));

		// 사용 중인 메모리 백분율
		ImGui::Text("Memory in use: %lu %%", statex.dwMemoryLoad);

	}




}

void CImGuiMgr::RigidBody_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle)
{
	PxVec3& Moi = pVehicle->pPhysXActorVehicle->mBaseParams.rigidBodyParams.moi;

	float MoiValue[3] = { Moi.x, Moi.y, Moi.z };
	ImGui::InputFloat("Mass", &pVehicle->pPhysXActorVehicle->mBaseParams.rigidBodyParams.mass);
	if (ImGui::InputFloat3("Moi", MoiValue))
	{
		Moi = PxVec3(MoiValue[0], MoiValue[1], MoiValue[2]);
	}
}

void CImGuiMgr::Tire_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle)
{
	_uint Wheels = pVehicle->pPhysXActorVehicle->mBaseParams.axleDescription.getNbWheels();

	for (_uint i = 0; i < Wheels; ++i)
	{
		ImGui::PushID(i);
		ImGui::Text("Tire %d", i);
		ImGui::InputFloat("longStiff", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].longStiff);
		ImGui::InputFloat("latStiffX", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].latStiffX);
		ImGui::InputFloat("latStiffY", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].latStiffY);
		ImGui::InputFloat("camberStiff", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].camberStiff);

		//ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[0][0]);
		ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[0][1]);
		//ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[1][0]);
		ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[1][1]);
		//ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[2][0]);
		ImGui::InputFloat("frictionVsSlip", &pVehicle->pPhysXActorVehicle->mBaseParams.tireForceParams[i].frictionVsSlip[2][1]);


		ImGui::PopID();

	}




}

void CImGuiMgr::Wheel_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle)
{
	_uint Wheels = pVehicle->pPhysXActorVehicle->mBaseParams.axleDescription.getNbWheels();



	for (_uint i = 0; i < Wheels; ++i)
	{
		ImGui::PushID(i);
		ImGui::Text("Wheel %d", i);

		ImGui::InputFloat("Radius", &pVehicle->pPhysXActorVehicle->mBaseParams.wheelParams[i].radius);
		ImGui::InputFloat("Half Width", &pVehicle->pPhysXActorVehicle->mBaseParams.wheelParams[i].halfWidth);
		ImGui::InputFloat("Mass", &pVehicle->pPhysXActorVehicle->mBaseParams.wheelParams[i].mass);
		ImGui::InputFloat("Moment of Inertia", &pVehicle->pPhysXActorVehicle->mBaseParams.wheelParams[i].moi);
		ImGui::InputFloat("Damping Rate", &pVehicle->pPhysXActorVehicle->mBaseParams.wheelParams[i].dampingRate);

		ImGui::PopID();
	}

}

void CImGuiMgr::Steering_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle)
{
	_uint Wheels = pVehicle->pPhysXActorVehicle->mBaseParams.axleDescription.getNbWheels();

	for (_uint i = 0; i < Wheels; ++i)
	{
		ImGui::PushID(i);
		ImGui::Text("Wheel %d Steering", i);
		_float fAngle = XMConvertToDegrees(pVehicle->pPhysXActorVehicle->mBaseParams.steerResponseParams.maxResponse);
		ImGui::InputFloat("Steer Angle", &fAngle);
		pVehicle->pPhysXActorVehicle->mBaseParams.steerResponseParams.maxResponse = XMConvertToRadians(fAngle);
		ImGui::InputFloat("Steer Multiplier", &pVehicle->pPhysXActorVehicle->mBaseParams.steerResponseParams.wheelResponseMultipliers[i]);
		ImGui::PopID();
	}

}


void CImGuiMgr::Free()
{
	Free_LayerTags_String(&m_LayerTags);
	Free_String(&m_wstrLayerTag);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();


	ImGui::DestroyContext();



}

#endif // _DEBUG
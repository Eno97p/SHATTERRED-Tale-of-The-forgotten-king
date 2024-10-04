#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "BackGround.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "UIGroup_Loading.h"

#pragma region LEVEL_HEADER

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Ackbar.h"
#include "Level_Jugglas.h"
#include "Level_AndrasArena.h"
#include "Level_GrassLand.h"

#pragma endregion

#include "UI_FadeInOut.h"

#include "Player.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUI_Manager);
}

/* 
typedef unsigned (__stdcall* _beginthreadex_proc_type)(void*);
*/



HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	LEVEL ePrevLevel = (LEVEL)CGameInstance::GetInstance()->Get_CurrentLevelIndex();
	m_pUI_Manager->SetPrevLevel(ePrevLevel);

	list<CGameObject*> 	pList = m_pGameInstance->Get_GameObjects_Ref(ePrevLevel, TEXT("Layer_Player"));
	if (!pList.empty())
	{
		auto pPlayer = dynamic_cast<CPlayer*>(pList.front());
		if(pPlayer != nullptr)
			Engine::Save_Data(L"../Bin/DataFiles/PlayerData.bin", true, pPlayer->Get_PlayerStatusData());
		Client::g__Exit_Delete_FileList.insert(L"../Bin/DataFiles/PlayerData.bin");
	}

	m_pGameInstance->StopAll();
	m_pGameInstance->Disable_Echo();
	//m_pGameInstance->PlayBGM(TEXT("BGM_Gameplay.mp3"), 0.1f);

	m_eNextLevel = eNextLevel;
	CGameInstance::GetInstance()->Set_NextLevel(eNextLevel);


	/* 로딩씬에 뭔가를 보여주려한다면 그 보여주기위한 객체들을 생성한다. (백그라운드 + 로딩바 + 폰트) */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	/* eNextLevel을 위한 자원을 로드한다.(추가적으로 생성한 스레드) */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (LEVEL_LOGO != m_eNextLevel)
	{
		m_pUI_Manager->Render_UIGroup(true, "Loading"); // 여기 갔을 때는 왜....ㅠㅠ

	}
	m_pUI_Manager->Tick(fTimeDelta);

	if (true == m_pLoader->is_Finished())
	{
		//if (GetKeyState(VK_RETURN) & 0x8000)
		{
			m_pGameInstance->Set_NextLevel(m_eNextLevel);
			
			CLevel*		pNewLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
 				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_Logo::Create(m_pDevice, m_pContext))))
					return;
				//pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
				m_pUI_Manager->Resset_Player();
				// 여기서 Loading의 Circle이 가지고 있는 값을 분명 변경해줬는데 왜... 다음 Loading으로 가면..
				dynamic_cast<CUIGroup_Loading*>(m_pUI_Manager->Get_UIGroup("Loading"))->Setting_Data();
				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_GamePlay::Create(m_pDevice, m_pContext))))
					return;
				break;
			case LEVEL_ACKBAR:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
				m_pUI_Manager->Resset_Player();
				dynamic_cast<CUIGroup_Loading*>(m_pUI_Manager->Get_UIGroup("Loading"))->Setting_Data();
				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_Ackbar::Create(m_pDevice, m_pContext))))
					return;
				break;
			case LEVEL_JUGGLAS:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
				m_pUI_Manager->Resset_Player();
				dynamic_cast<CUIGroup_Loading*>(m_pUI_Manager->Get_UIGroup("Loading"))->Setting_Data();
				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_Jugglas::Create(m_pDevice, m_pContext))))
					return;
				break;
			case LEVEL_ANDRASARENA:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
				m_pUI_Manager->Resset_Player();
				dynamic_cast<CUIGroup_Loading*>(m_pUI_Manager->Get_UIGroup("Loading"))->Setting_Data();
				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_AndrasArena::Create(m_pDevice, m_pContext))))
					return;
				break;
			case LEVEL_GRASSLAND:
				m_pUI_Manager->Render_UIGroup(false, "Loading");
				m_pUI_Manager->Resset_Player();
				dynamic_cast<CUIGroup_Loading*>(m_pUI_Manager->Get_UIGroup("Loading"))->Setting_Data();
				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, CLevel_GrassLand::Create(m_pDevice, m_pContext))))
					return;
				break;
			default :
				break;
			}

			//if (nullptr == pNewLevel)
			//	return;

			//if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
			//	return;
		}		
	}


#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif
}

void CLevel_Loading::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	/*CBackGround::BACKGROUND_DESC		ObjectDesc{};

	ObjectDesc.fData = 10.f;
	ObjectDesc.iData = 5;
	ObjectDesc.fSpeedPerSec = 10.f;
	ObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);*/

	/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;*/

	//CUI_Manager::GetInstance()->Render_Logo(true);

	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

	pDesc.isFadeIn = true;
	pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;
	pDesc.isLevelChange = true;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel)
{
	
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
	Safe_Release(m_pUI_Manager);
}

#include "..\Default\framework.h"
#include "Level_Loading.h"

#include "Loader.h"
#include "BackGround.h"

#include "GameInstance.h"

#pragma LEVEL_HEADER
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#pragma endregion LEVEL_HEADER

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{pDevice, pContext}
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	// 로딩씬에 보여주기 위한 객체 생성
	// 백그라운드
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	// 다음 레벨을 위한 자워 로드
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->is_Finished())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_pGameInstance->Set_NextLevel(m_eNextLevel);

			CLevel* pNewLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case AnimTool::LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case AnimTool::LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return;
		}
	}

#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : Level_Loading");
		return nullptr;
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

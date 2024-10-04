#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_Logo::Initialize()
{
	m_pUI_Manager->Render_UIGroup(true, "Logo");

	//GFSDK_SSAO_Parameters params = {};
	//params.Radius = 6.0f;
	//params.Bias = 0.203f;
	//params.SmallScaleAO = 1.442f;
	//params.LargeScaleAO = 1.015f;
	//params.PowerExponent = 2.753f;
	//_int stepCount = 6;
	//params.StepCount = static_cast<GFSDK_SSAO_StepCount>(stepCount);

	//params.Blur.Enable = 1;

	//_int blurRadius = 6;
	//params.Blur.Radius = static_cast<GFSDK_SSAO_BlurRadius>(blurRadius);
	//params.Blur.Sharpness = 0.0f;

	//m_pGameInstance->Set_HBAOParams(params);

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	m_pUI_Manager->Tick(fTimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");
		
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
	if(KEY_TAP(DIK_NUMPAD1))
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
	else if (KEY_TAP(DIK_NUMPAD2))
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ACKBAR))))
			return;
	}
	else if (KEY_TAP(DIK_NUMPAD3))
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_JUGGLAS))))
			return;
	}
	else if (KEY_TAP(DIK_NUMPAD4))
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ANDRASARENA))))
			return;
	}
	else if (KEY_TAP(DIK_NUMPAD5))
	{
		m_pUI_Manager->Render_UIGroup(false, "Logo");

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GRASSLAND))))
			return;
	}





#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨임"));
#endif
}

void CLevel_Logo::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Manager);
}

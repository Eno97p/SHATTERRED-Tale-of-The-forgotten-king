#include "..\Default\framework.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"

#include "Shader.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance()}
	, m_pImgui_Manager{CImgui_Manager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImgui_Manager);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;
	if (FAILED(m_pImgui_Manager->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_NavigationCell()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/142ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pImgui_Manager->Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
}

HRESULT CMainApp::Render()
{
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);
		//Terst
		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}


	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	m_pImgui_Manager->Render();


	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	m_pGameInstance->Present();



	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;



	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if(FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Logo.png"), 1)))) // TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;




	return S_OK;
}

HRESULT CMainApp::Ready_NavigationCell()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// �ӽ÷� Navigation�� Cell���� ����� �־��
	_float3		vPoints[3] = {};

	_ulong dwByte = {};
	HANDLE hFile = CreateFile(TEXT("../../Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pImgui_Manager);
	CImgui_Manager::DestroyInstance();
	
	CGameInstance::Release_Engine();
}

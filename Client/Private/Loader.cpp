#include "stdafx.h"
#include "..\Public\Loader.h"




#include"Import_Class.h"	// <---여기에 들어가서 헤더 넣으셈
#include"Import_Object_.hpp"	// <---여기에 들어가서 오브젝트 원형  넣으셈	//원형 로드는 로고레벨에서 수행함



CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_iFinishedThreadCount{ 0 }

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	for (int i = 0; i < MAX_THREAD; ++i)
		m_bIsFinish[i] = false;

}

_uint APIENTRY Loading_Main(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_MapData(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading_Map()))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_ShaderData(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading_Shader()))
		return 1;

	CoUninitialize();

	return 0;
}







HRESULT CLoader::Initialize(LEVEL eNextLevel)
{

	m_eNextLevel = eNextLevel;

	for (_uint i = 0; i < USED_THREAD_COUNT; ++i)
		InitializeCriticalSection(&m_Critical_Section[i]);

	m_hThread[0] = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread[0])
		return E_FAIL;

	m_hThread[1] = (HANDLE)_beginthreadex(nullptr, 0, Loading_MapData, this, 0, nullptr);
	if (0 == m_hThread[1])
		return E_FAIL;

	m_hThread[2] = (HANDLE)_beginthreadex(nullptr, 0, Loading_ShaderData, this, 0, nullptr);
	if (0 == m_hThread[2])
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section[0]);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;	
	case LEVEL_ACKBAR:
		hr = Loading_For_AckbarLevel();
		break;
	case LEVEL_JUGGLAS:
		hr = Loading_For_JugglasLevel();
		break;
	case LEVEL_ANDRASARENA:
		hr = Loading_For_AndrasArenaLevel();
		break;
	case LEVEL_GRASSLAND:
		hr = Loading_For_GrassLandLevel();
		break;
	}
	LeaveCriticalSection(&m_Critical_Section[0]);

	if (FAILED(hr))
		return E_FAIL;

	Finish_Thread(0);

	return S_OK;
}

HRESULT CLoader::Loading_Map()
{
	EnterCriticalSection(&m_Critical_Section[1]);
	HRESULT			hr{};


	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		break;
	case LEVEL_GAMEPLAY:
	{

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_static_For_Well"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", TEXT("")))))
			return hr = E_FAIL;
		

		//PHYSX TUTORIAL MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Tutorial_PhysX.bin";
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


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
				return hr = E_FAIL;

		}

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PhysX_BasicDonut"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
			return hr = E_FAIL;

		CloseHandle(hFile);

#ifdef _DEBUG
		//MSG_BOX("Tutorial PhysX Data Loaded");
#endif
	}
	break;
	case LEVEL_ACKBAR: //JUGGLAS CASTLE
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx_static_For_Well"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", TEXT("")))))
			return hr = E_FAIL;


		//PHYSX JUGGLAS MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Ackbar_PhysX.bin";
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


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);


#ifdef _DEBUG
		//MSG_BOX("Ackbar PhysX Data Loaded");
#endif
	}
	break;
	case LEVEL_JUGGLAS: //JUGGLAS CASTLE
	{

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx_static_For_Well"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", TEXT("")))))
			return hr = E_FAIL;


		//PHYSX JUGGLAS MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Juggulas_PhysX.bin";
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


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Jugglas.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);

#ifdef _DEBUG
		//MSG_BOX("Jugglas PhysX Data Loaded");
#endif
	}
	break;
	case LEVEL_ANDRASARENA: //JUGGLAS CASTLE
	{


		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Physx_static_For_Well"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", TEXT("")))))
			return hr = E_FAIL;





		//PHYSX JUGGLAS MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_AndrasArena_PhysX.bin";
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


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_AndrasArena.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);

#ifdef _DEBUG
		//MSG_BOX("AndrasArena PhysX Data Loaded");
#endif
	}
	break;
	case LEVEL_GRASSLAND: //JUGGLAS CASTLE
	{

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Physx_static_For_Well"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", TEXT("")))))
			return hr = E_FAIL;

		//PHYSX TUTORIAL MAP
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


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_GrassLand.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);

#ifdef _DEBUG
		//MSG_BOX("AndrasArena PhysX Data Loaded");
#endif
	}
	break;
	}

	LeaveCriticalSection(&m_Critical_Section[1]);


	if (FAILED(hr))
		return E_FAIL;



	Finish_Thread(1);
	return S_OK;
}

HRESULT CLoader::Loading_Shader()
{
	EnterCriticalSection(&m_Critical_Section[2]);
	HRESULT			hr{};


	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel_For_Shader();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel_For_Shader();
		break;
	case LEVEL_ACKBAR:
		hr = Loading_For_AckbarLevel_For_Shader();
		break;
	case LEVEL_JUGGLAS:
		hr = Loading_For_JugglasLevel_For_Shader();
		break;
	case LEVEL_ANDRASARENA:
		hr = Loading_For_AndrasArenaLevel_For_Shader();
		break;
	case LEVEL_GRASSLAND:
		hr = Loading_For_GrassLandLevel_For_Shader();
		break;
	}
	//예시:case LEVEL_BOSS:
	//	
	//	
	//	
	//	
	//	
	//	
	//
	//	break;







	LeaveCriticalSection(&m_Critical_Section[2]);


	if (FAILED(hr))
		return E_FAIL;

	Finish_Thread(2);
	return S_OK;

}




HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxUITex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUITex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;





	if(FAILED(Load_Object(m_pGameInstance, m_pDevice, m_pContext)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));


	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel_For_Shader()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	///* Prototype_Component_Texture_Snow */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_YantariBody"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Yantari/YantariBody%d.dds"), 3))))
		return E_FAIL;


#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	//// Prototype_Component_Model_TronesT02
	PreTransformMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TronesT02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT02/TronesT02.fbx", PreTransformMatrix))))
		return E_FAIL;

	//// Prototype_Component_Model_TronesT03
	PreTransformMatrix =/* XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TronesT03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT03/TronesT03.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ANDRAS ARENA
	//ANDRAS ARENA
	//ANDRAS ARENA
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AndrasTEST"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasTEST/AndrasTEST.fbx", PreTransformMatrix))))
	//	return E_FAIL;


	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMap.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMapBridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMapBridge.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoStructure"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoStructure.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoCubes"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoCubes.fbx", PreTransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoMaze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoMaze.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;


	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   DECO ELEMENTS

#pragma region   VEGETATION
	lstrcpy(m_szLoadingText, TEXT("식물 모델 로딩 중"));

	//TREES
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreeC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/TreeC/TreeC.fbx", PreTransformMatrix))))
		return E_FAIL;

	//1
	/* For.Prototype_Component_Model_BasicTree */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BasicTree/BasicTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//2
	/* For.Prototype_Component_Model_BirchTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BirchTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BirchTree/BirchTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//3
	/* For.Prototype_Component_Model_BloomTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BloomTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BloomTree/BloomTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//4
	/* For.Prototype_Component_Model_Bush */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bush"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/Bush/Bush.fbx", PreTransformMatrix))))
		return E_FAIL;

	//5
	/* For.Prototype_Component_Model_CherryTree */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CherryTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/CherryTree/CherryTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//6
	/* For.Prototype_Component_Model_GhostTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GhostTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/GhostTree/GhostTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//7
	/* For.Prototype_Component_Model_PineTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PineTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/PineTree/PineTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//8
	/* For.Prototype_Component_Model_WillowTree */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WillowTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/WillowTree/WillowTree.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   VEGETATION


#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Elevator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Elevator/Elevator.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); 

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma region Monster
	// Andras
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Andras_0724/Andras.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Mask_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mask_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Mask/AndrasMask.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword1.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Andras2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword2.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Andras3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword3.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Andras4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword4.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Andras5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword5.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// Juggulus
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Juggulus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Juggulus.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHammer
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Hammer.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandOne
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandOne"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_1.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandTwo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandTwo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_2.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandThree
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandThree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_3.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Aspiration
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Sphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	// CircleSphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CircleSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Malkhel/Malkhel.fbx", PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Malkhel/MalkhelSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Yantari/Yantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mask_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mask_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariMask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariAxe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL



	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fragile_Rock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fragile_Rock/Fragile_Rock.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;



	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel_For_Shader()
{

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxInstance_MapElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Tree */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Tree"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Tree.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));
	
	return S_OK;
}

HRESULT CLoader::Loading_For_AckbarLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	///* Prototype_Component_Texture_Snow */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Snow"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_YantariBody"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/DDS_Storage/YantariBody%d.dds"), 3))))
		return E_FAIL;

#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;




#pragma region   VEGETATION
	lstrcpy(m_szLoadingText, TEXT("식물 모델 로딩 중"));

	//TREES
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_TreeC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/TreeC/TreeC.fbx", PreTransformMatrix))))
		return E_FAIL;

	//1
	/* For.Prototype_Component_Model_BasicTree */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BasicTree/BasicTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//2
	/* For.Prototype_Component_Model_BirchTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BirchTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BirchTree/BirchTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//3
	/* For.Prototype_Component_Model_BloomTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BloomTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BloomTree/BloomTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//4
	/* For.Prototype_Component_Model_Bush */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Bush"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/Bush/Bush.fbx", PreTransformMatrix))))
		return E_FAIL;

	//5
	/* For.Prototype_Component_Model_CherryTree */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CherryTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/CherryTree/CherryTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//6
	/* For.Prototype_Component_Model_GhostTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_GhostTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/GhostTree/GhostTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//7
	/* For.Prototype_Component_Model_PineTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_PineTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/PineTree/PineTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//8
	/* For.Prototype_Component_Model_WillowTree */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WillowTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/WillowTree/WillowTree.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   VEGETATION


#pragma region  STAGE 1 PASSIVE ELEMENTS

	//STAGE 1 PASSIVE ELEMENTS
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Well"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MergedSmallHouses23"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/MergedSmallHouses23/MergedSmallHouses23.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MergedSmallHouses65"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/MergedSmallHouses65/MergedSmallHouses65.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarCastle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarCastle/AckbarCastle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AqueducTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTower/AqueducTower.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigStairSides"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigStairSides/BigStairSides.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodStair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodStair.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodPlatform"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodPlatform.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodPlank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/WoodenPlank.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigRocks"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigRocks4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock1.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock2.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock3.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RuinsPilar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/RuinsPilar/RuinsPilar.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseLarge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarHouseLarge.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarHouseSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarSwampHouse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarSwampHouse.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofL"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofL.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofM"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofM.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofS"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofS.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AqueducTowerRoof"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTowerRoof/AqueducTowerRoof.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_UnderRoofFilling"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/UnderRoofFilling/UnderRoofFilling.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma endregion  STAGE 1 PASSIVE ELEMENTS


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WarpGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/WarpGate/WarpGate.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma region  DECO ELEMENTS
	//STAGE 1 DECO ELEMENTS
	//Box, Crate
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BoxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BoxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateB.fbx", PreTransformMatrix))))
		return E_FAIL;

	//YantraStatue
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_YantraStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/YantraStatue/YantraStatue.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Banners
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole5.fbx", PreTransformMatrix))))
		return E_FAIL;

	//DECO PROPS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarWell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/AckbarWell.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_CrenauxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_CrenauxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_HourdA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_HourdA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_WallMotifA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallMotifA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_WallRenfortA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallRenfortA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Charette
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CharetteNew"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteNew.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CharetteBroke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteBroke.fbx", PreTransformMatrix))))
		return E_FAIL;

	//FACADE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade10.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TreasureChest/TreasureChest.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   DECO ELEMENTS

#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Malkhel/Malkhel.fbx", PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Malkhel/MalkhelSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Yantari/Yantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mask_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Mask_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariMask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariAxe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MetalGrid2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/MetalGrid2/MetalGrid2.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma region DECAL


#pragma endregion DECAL


#pragma region Npc
	// Npc Rlya
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Npc_Rlya"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC_Myosis/NPC_Rlya.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Npc Valnir
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Npc_Valnir"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC_Valnir/Valnir.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion Npc



	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Fragile_Rock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fragile_Rock/Fragile_Rock.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;




	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("LEVEL ACKBAR"));


	return S_OK;
}


HRESULT CLoader::Loading_For_AckbarLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTreasureChest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxTreasureChest"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTreasureChest.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxInstance_Tree */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_Tree"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Tree.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_JugglasLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;


	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

#pragma region Npc

	// Npc Choron
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Npc_Choron"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC_Choron/Choron.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Npc

#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	//// Prototype_Component_Model_TronesT02
	PreTransformMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TronesT02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT02/TronesT02.fbx", PreTransformMatrix))))
		return E_FAIL;

	//// Prototype_Component_Model_TronesT03
	PreTransformMatrix =/* XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TronesT03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT03/TronesT03.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RuinsPilar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/RuinsPilar/RuinsPilar.fbx", PreTransformMatrix))))

		return E_FAIL;
	//JUGGLAS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMap.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialMapBridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMapBridge.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialDecoStructure"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoStructure.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialDecoCubes"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoCubes.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;





#pragma region  JUGGLAS PASSIVE ELEMENTS

	//JUGGLAS PASSIVE ELEMENTS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle4.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle5.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WarpGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/WarpGate/WarpGate.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion  JUGGLAS PASSIVE ELEMENTS

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region  DECO ELEMENTS
	//STAGE 1 DECO ELEMENTS
	//Box, Crate
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BoxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BoxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateB.fbx", PreTransformMatrix))))
		return E_FAIL;

	//YantraStatue
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_YantraStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/YantraStatue/YantraStatue.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Banners
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole5.fbx", PreTransformMatrix))))
		return E_FAIL;

	//DECO PROPS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_AckbarWell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/AckbarWell.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_CrenauxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_CrenauxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_HourdA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_HourdA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_WallMotifA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallMotifA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_WallRenfortA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallRenfortA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Charette
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CharetteNew"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteNew.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CharetteBroke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteBroke.fbx", PreTransformMatrix))))
		return E_FAIL;

	//FACADE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade10.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion   DECO ELEMENTS

#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Elevator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Elevator/Elevator.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SkeletalChain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/RasSamrahCastle/SkeletalChain/SkeletalChain.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Chaudron"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/RasSamrahCastle/Chaudron/Chaudron.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RotateGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahGate/RasSamrahGate.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BossStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahStatue/ActiveStatue.fbx", PreTransformMatrix))))
		return E_FAIL;


	//Trap
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Hachoir"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Traps/Hachoir/Hachoir.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SmashingPillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Traps/SmashingPillar/SmashingPillar.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TreasureChest/TreasureChest.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Juggulus
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Juggulus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Juggulus.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// JuggulusHammer
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Hammer.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandOne
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandOne"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_1.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandTwo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandTwo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_2.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandThree
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandThree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_3.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Aspiration
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Aspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Sphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	// CircleSphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CircleSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Yantari/Yantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mask_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Mask_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariMask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariAxe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL


	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Fragile_Rock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fragile_Rock/Fragile_Rock.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;





	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("LEVEL JUGGLAS"));

	return S_OK;
}

HRESULT CLoader::Loading_For_JugglasLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTreasureChest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxTreasureChest"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTreasureChest.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_AndrasArenaLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	///* Prototype_Component_Texture_Snow */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Snow"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;

	/* Prototype_Component_Texture_NoiseRepeat*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_NoiseRepeat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/perlinNoiseRepeat.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Grass_TT"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/T_Grassland_TallThinGrass.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Texture_Grass_TF"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/T_Grassland_TallFatGrass.dds"), 1))))
		return E_FAIL;
#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;



	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_RuinsPilar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/RuinsPilar/RuinsPilar.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma region  ANDRAS ARENA PASSIVE ELEMENTS

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_AndrasTEST"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasTEST/AndrasTEST.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion  ANDRAS ARENA PASSIVE ELEMENTS

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region  DECO ELEMENTS
	//ANDRASARENA STAGE DECO ELEMENTS
	//Banners
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BannerPole1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BannerPole2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BannerPole3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BannerPole4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_BannerPole5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole5.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_AndrasArena_Deco"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasArena/AndrasArena_Deco/AndrasArena_Deco.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma endregion   DECO ELEMENTS

#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster
	// Andras
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Andras_0724/Andras.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Mask_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Mask_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Mask/AndrasMask.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword1.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Andras2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword2.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Andras3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword3.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Andras4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword4.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Andras5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword5.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Yantari/Yantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mask_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Mask_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariMask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Weapon_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariAxe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM

#pragma region DECAL

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion DECAL




	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("LEVEL JUGGLAS"));


	return S_OK;
}


HRESULT CLoader::Loading_For_AndrasArenaLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTreasureChest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxTreasureChest"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTreasureChest.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxInstance_Grass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxInstance_Grass"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Grass.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANDRASARENA, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_GrassLandLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	// Prototype_Component_Texture_GroundGrass
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Ground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Ground/T_Ground_%d.dds"), 17))))
		return E_FAIL;

	// Prototype_Component_Texture_GroundGrass
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Ground_Roughness"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Ground/T_Ground_R_%d.dds"), 17))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;



	///* Prototype_Component_Texture_Snow */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Snow"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Grass_TT"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/T_Grassland_TallThinGrass.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Grass_TF"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/T_Grassland_TallFatGrass.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_NoiseRepeat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Grass/perlinNoiseRepeat.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture3D_Noise*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture3D_Noise"),
		CTexture3D::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Noise/Cloud/pleaseplease.dds")))))
		return E_FAIL;


	/* Prototype_Component_Texture_WindGust*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_WindGust"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Lagoon/wind_gusts.dds"), 1))))
		return E_FAIL;


	/* Prototype_Component_Texture_Foam_Intensity*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Foam_Intensity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Lagoon/foam_intensity.dds"), 1))))
		return E_FAIL;


	/* Prototype_Component_Texture_Foam_Bubbles*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Foam_Bubbles"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Lagoon/foam_bubbles.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_WaterNormal1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/waterwayNRM.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_WaterNormal2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/T_StylizedWater_03_N.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_WaterCaustic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/water5.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_FoamMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/Foam/T_RiverFoam.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Foam"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/Foam/T_Foam_D_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_FoamNormal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Lagoon/Water/Foam/T_WaterFoam_N_%d.dds"), 2))))
		return E_FAIL;


	/* Prototype_Component_Texture_Moon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Texture_Moon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/SkyBox/Moon/Moon_%d.png"), 3))))

		return E_FAIL;
	_matrix		PreTransformMatrix;

#pragma region Npc


	// Npc Yaak
	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Npc_Yaak"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC_Yaak/Yaak.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Npc
#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/WorldCreatorHM.raw"), true))))
		return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	//// Prototype_Component_Model_TronesT02
	PreTransformMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_TronesT02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT02/TronesT02.fbx", PreTransformMatrix))))
		return E_FAIL;

	//// Prototype_Component_Model_TronesT03
	PreTransformMatrix =/* XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_TronesT03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT03/TronesT03.fbx", PreTransformMatrix))))
		return E_FAIL;



	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_CloudDome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/CloudDome/CloudDome.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Lagoon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Lagoon/Lagoon.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ANDRAS ARENA
	//ANDRAS ARENA
	//ANDRAS ARENA
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_AndrasTEST"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasTEST/AndrasTEST.fbx", PreTransformMatrix))))
	//	return E_FAIL;


	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
//GrassLand PASSIVE ELEMENTS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Cathedral"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Cathedral/Cathedral.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Espadon_Station"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Espadon_Station/Espadon_Station.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_GrassLand_Ruins"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/GrassLand_Ruins/GrassLand_Ruins.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Tower_Ruins"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Tower_Ruins/Tower_Ruins.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Bastion"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Bastion/Bastion.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_MetalGrid2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/MetalGrid2/MetalGrid2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Hoverboard_Track"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Hoverboard_Track/Hoverboard_Track.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Hoverboard_Track_Pillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Hoverboard_Track_Pillar/Hoverboard_Track_Pillar.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_GrassLand_Arch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/GrassLand_Arch/GrassLand_Arch.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Meteore"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Meteore/Meteore.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mountain_Card"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Mountain_Card/Mountain_Card.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mountain_BackGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GrassLand/Mountain_BackGround/Mountain_BackGround.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_AckbarCastle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarCastle/AckbarCastle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BigRocks"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BigRocks4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock1.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Rock2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock2.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Rock3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock3.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Rock4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock4.fbx", PreTransformMatrix))))
		return E_FAIL;



	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   DECO ELEMENTS

#pragma region   VEGETATION
	lstrcpy(m_szLoadingText, TEXT("식물 모델 로딩 중"));

	//TREES
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_TreeC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/TreeC/TreeC.fbx", PreTransformMatrix))))
		return E_FAIL;

	//1
	/* For.Prototype_Component_Model_BasicTree */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BasicTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BasicTree/BasicTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//2
	/* For.Prototype_Component_Model_BirchTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BirchTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BirchTree/BirchTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//3
	/* For.Prototype_Component_Model_BloomTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BloomTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/BloomTree/BloomTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//4
	/* For.Prototype_Component_Model_Bush */
	PreTransformMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Bush"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/Bush/Bush.fbx", PreTransformMatrix))))
		return E_FAIL;

	//5
	/* For.Prototype_Component_Model_CherryTree */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_CherryTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/CherryTree/CherryTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//6
	/* For.Prototype_Component_Model_GhostTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_GhostTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/GhostTree/GhostTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//7
	/* For.Prototype_Component_Model_PineTree */
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_PineTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/PineTree/PineTree.fbx", PreTransformMatrix))))
		return E_FAIL;

	//8
	/* For.Prototype_Component_Model_WillowTree */
	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WillowTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Trees/WillowTree/WillowTree.fbx", PreTransformMatrix))))
		return E_FAIL;
	

#pragma endregion   VEGETATION


#pragma region  GRASSLAND Passive Element Models
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_AqueducTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTower/AqueducTower.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BigStairSides"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigStairSides/BigStairSides.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WoodStair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodStair.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WoodPlatform"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodPlatform.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WoodPlank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/WoodenPlank.fbx", PreTransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_RuinsPilar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/RuinsPilar/RuinsPilar.fbx", PreTransformMatrix))))
		return E_FAIL;


	//STAGE 1 DECO ELEMENTS
	//Box, Crate
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BoxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_BoxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_AckbarWell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/AckbarWell.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Remparts_CrenauxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Remparts_CrenauxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Remparts_HourdA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_HourdA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Remparts_WallMotifA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallMotifA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Remparts_WallRenfortA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallRenfortA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_AqueducTowerRoof"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTowerRoof/AqueducTowerRoof.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_CharetteBroke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteBroke.fbx", PreTransformMatrix))))
		return E_FAIL;

	//FACADE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Facade10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade10.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma endregion

#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Elevator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Elevator/Elevator.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TreasureChest/TreasureChest.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster
	// Andras
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Andras_0724/Andras.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Mask_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mask_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Mask/AndrasMask.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword1.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Andras2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword2.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Andras3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword3.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Andras4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword4.fbx", PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weapon_Andras5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Andras5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Swords/Sword5.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Aspiration
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Aspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Sphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	// CircleSphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_CircleSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Malkhel/Malkhel.fbx", PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Malkhel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Malkhel/MalkhelSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Yantari/Yantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mask_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mask_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariMask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Yantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Weapon_Yantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Yantari/YantariAxe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL


#pragma region Npc
	PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Npc_Rlya"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC_Myosis/NPC_Rlya.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion Npc



	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Fragile_Rock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fragile_Rock/Fragile_Rock.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Model_WarpGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/WarpGate/WarpGate.fbx", PreTransformMatrix))))
		return E_FAIL;



	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//TEXT("Prototype_Component_VIBuffer_Terrain")
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Physx_HeightField"),
		CPhysXComponent_HeightField::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_VIBuffer_Terrain")/*찾을 터레인의 원형*/))))
		return E_FAIL;
	


	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_GrassLandLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxLagoon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxLagoon"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxLagoon.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTreasureChest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxTreasureChest"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTreasureChest.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Grass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxInstance_Grass"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Grass.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxInstance_Tree */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxInstance_Tree"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Tree.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCloud */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_Shader_VtxCloud"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCloud.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GRASSLAND, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		if (m_hThread[i] != nullptr)
		{
			WaitForSingleObject(m_hThread[i], INFINITE);
			CloseHandle(m_hThread[i]);
			m_hThread[i] = nullptr;
		}
	}

	for (_uint i = 0; i < MAX_THREAD; ++i)
		DeleteCriticalSection(&m_Critical_Section[i]);


	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

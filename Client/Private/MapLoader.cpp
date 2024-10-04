#include "stdafx.h"
#include "..\Public\MapLoader.h"

#include "Particle_Point.h"
#include "GameInstance.h"



#include "Map_Element.h"
#include "Passive_Element.h"
#include "Active_Element.h"



CMapLoader::CMapLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY Loading_Main(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CMapLoader*		pLoader = (CMapLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_MapData(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CMapLoader*		pLoader = (CMapLoader*)pArg;

	if (FAILED(pLoader->Loading_MapData()))
		return 1;

	CoUninitialize();

	return 0;
}



HRESULT CMapLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CMapLoader::Loading()
{


	EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
	{
		
			hr = Loading_For_GamePlayLevel();
			//Load_LevelData(m_eNextLevel, TEXT("../Bin/MapData/Stage.dat"));
		
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;


	return S_OK;
	}

	return S_OK;
}

HRESULT CMapLoader::Loading_MapData()
{







	return S_OK;
}

HRESULT CMapLoader::Load_LevelData(LEVEL eLevel, const _tchar* pFilePath)
{
	HANDLE   hFile = CreateFile(pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	_tchar   wszName[MAX_PATH] = TEXT("");
	_tchar   wszLayer[MAX_PATH] = TEXT("");
	_tchar   wszModelName[MAX_PATH] = TEXT("");

	// 생성된 객체 로드
	while (true)
	{
		ZeroMemory(wszName, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszLayer, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszModelName, sizeof(_tchar) * MAX_PATH);

		ReadFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

		MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
		MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
		MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

		//wstring Temp = wszName;

		if (0 == dwByte)
			break;

		CMap_Element::MAP_ELEMENT_DESC pDesc{};

		//strcpy_s(pDesc.szName, szName);
		pDesc.mWorldMatrix = WorldMatrix;
		pDesc.wstrModelName = wszModelName;

		if (FAILED(m_pGameInstance->Add_CloneObject(eLevel, wszLayer, wszName, &pDesc))) // TEXT("Prototype_ToolObj")
			return E_FAIL;

		if (0 == dwByte)
			break;
	}

	CloseHandle(hFile);

	return S_OK;

}

HRESULT CMapLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CMapLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;
	
	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
	//	return E_FAIL;

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Rect */
//	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
//
//	InstanceDesc.iNumInstance = 300;
//	InstanceDesc.vOffsetPos = _float3(0.0f, -3.f, 0.0f);
//	InstanceDesc.vPivotPos = _float3(0.0f, 0.f, 0.0f);
//	InstanceDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
//	InstanceDesc.vSize = _float2(0.2f, 0.4f);	
//	InstanceDesc.vSpeed = _float2(1.f, 7.f);
//	InstanceDesc.vLifeTime = _float2(1.f, 3.f);
//	InstanceDesc.isLoop = false;
//
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
//		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
//		return E_FAIL;
//
//
//	/* For.Prototype_Component_VIBuffer_Instance_Point*/
//	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);
//
//	InstanceDesc.iNumInstance = 1000;
//	InstanceDesc.vOffsetPos = _float3(0.0f, 0.f, 0.0f);
//	InstanceDesc.vPivotPos = _float3(0.0f, 30.f, 0.0f);
//	InstanceDesc.vRange = _float3(500.0f, 0.5f, 500.0f);
//	InstanceDesc.vSize = _float2(0.2f, 0.4f);
//	InstanceDesc.vSpeed = _float2(1.f, 7.f);
//	InstanceDesc.vLifeTime = _float2(10.f, 15.f);
//	InstanceDesc.isLoop = true;
//
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
//		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
//		return E_FAIL;

	_matrix		PreTransformMatrix;

	/* For.Prototype_Component_Model_ForkLift */

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
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

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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


	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AndrasTEST"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasTEST/AndrasTEST.fbx", PreTransformMatrix))))
		return E_FAIL;






	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AndrasArenaRocks"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasArenaRocks/AndrasArenaRocks.fbx", PreTransformMatrix))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;







	
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

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));

	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FreeCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster ::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Distortion"),
		CDistortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Clone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Clone"),
		CClone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WhisperSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword"),
		CWhisperSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WhisperSword_Anim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword_Anim"),
		CWhisperSword_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cendres */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cendres"),
		CCendres::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CorruptedSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CorruptedSword"),
		CCorruptedSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Catharsis */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Catharsis"),
		CCatharsis::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_DurgaSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DurgaSword"),
		CDurgaSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_IceBlade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBlade"),
		CIceBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_NaruehSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NaruehSword"),
		CNaruehSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PretorianSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PretorianSword"),
		CPretorianSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RadamantheSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RadamantheSword"),
		CRadamantheSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_SitraSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SitraSword"),
		CSitraSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ValnirSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ValnirSword"),
		CValnirSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_VeilleurSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VeilleurSword"),
		CVeilleurSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Passive_Element */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Passive_Element"),
		CPassive_Element::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Active_Element */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Active_Element"),
		CActive_Element::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Particle_Rect */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rect"),
	//	CParticle_Rect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Particle_Point */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Point"),
	//	CParticle_Point::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CMapLoader * CMapLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel)
{
	CMapLoader*		pInstance = new CMapLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CMapLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

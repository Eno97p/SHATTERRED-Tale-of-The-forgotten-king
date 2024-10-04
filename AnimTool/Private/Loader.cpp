#include "..\Default\framework.h"
#include "Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Monster.h"
#include "Default_Camera.h"
#include "ToolObj.h"
#include "Bone_Sphere.h"
#include "ToolPartObj.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT	hr = {};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	// Prototype_Component_Texture_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	// Prototype_Component_Texture_Sphere_Color
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sphere_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Bone_Sphere/Color_%d.png"), 2))))
		return E_FAIL;
	

#pragma region Model Load
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	_matrix PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Wanda
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Wanda"), // "../../Client/Bin/Resources/Models/Fiona/Fiona.fbx"
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	// 병합용

	// Homonculus
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Homonculus/Homomculus.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Job Mob
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_JobMob"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Job_Mob/Job_Mob1.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Juggulus
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Juggulus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Juggulus/Juggulus.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Malkhel
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Malkhel"), // Prototype_GameObject_Malkhel
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Andras_0724/Andras.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Mantari
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Npc_Choron
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_NPC_Choron"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/NPC_Choron/Choron.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Npc_Valnir
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_NPC_Valnir"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/NPC_Valnir/Valnir.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Arrow_Jobmob
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Arrow_Jobmob"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Ghost
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// Bone Sphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma region PartObj
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// Gun
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_JobMob_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion PartObj

	// Prototype_Component_VIBuffer_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

#pragma endregion Model Load

	lstrcpy(m_szLoadingText, TEXT("내비게이션(을) 로딩 중 입니다."));
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/BossStage.dat")))))
		return E_FAIL;*/


#pragma region Shader Load
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	// Prototype_Component_Shader_VtxNorTex
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	// Prototype_Component_Shader_VtxMesh
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	// Prototype_Component_Shader_VtxAnimMesh
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

#pragma endregion Shader Load

#pragma region Object Prototype Load
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 로딩 중 입니다."));
	// Prototype_GameObject_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_DefaultCamera

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DefaultCamera"),
		CDefault_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Monster
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// ToolObj
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_ToolObj"),
		CToolObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// BoneSphere
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BoneSphere"),
		CBone_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// PartObj
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PartObj"),
		CToolPartObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion Object Prototype Load

	//// Prototype_Component_Calculator
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Calculator"),
	//	CCalculator::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

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
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

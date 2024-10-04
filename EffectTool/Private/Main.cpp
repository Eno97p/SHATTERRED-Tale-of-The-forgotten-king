
#pragma once
#include "Main.h"
#include "GameInstance.h"
#include "Shader.h"
#include "ImguiMgr.h"
#include "BackGround.h"
#include "Level_Loading.h"



CMainEffect::CMainEffect()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainEffect::Initialize()
{
	ENGINE_DESC			EngineDesc{};
	
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	/* 엔진 초기화과정을 거친다. ( 그래픽디바이스 초기화과정 + 레벨매니져를 사용할 준비를 한다. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	m_pImguiMgr = CImguiMgr::Create(m_pDevice, m_pContext);
	if (m_pImguiMgr == nullptr)
		return E_FAIL;

	return S_OK;
}

void CMainEffect::Tick(float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pImguiMgr->Tick(fTimeDelta);
	
}

HRESULT CMainEffect::Render()
{

	m_pImguiMgr->Render();

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainEffect::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainEffect::Ready_Prototype_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainEffect::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//a
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_DistortionEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_DistortionRect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FrameTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_FrameTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Fire"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Fire.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	
	//로딩텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/Illu_PNJ_Choronzon.png"), 1))))
		return E_FAIL;

	//불
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Fire/fire01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Fire/noise01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireAlpha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Fire/alpha01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBloom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Fire/CircleGradient.dds"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}


CMainEffect* CMainEffect::Create()
{
	CMainEffect* pInstance = new CMainEffect();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainEffect::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pImguiMgr);

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();
}

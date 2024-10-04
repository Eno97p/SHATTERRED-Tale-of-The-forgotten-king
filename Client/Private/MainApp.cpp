#include "stdafx.h"
#include "..\Public\MainApp.h"

//new

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "CMouse.h"

#include "Shader.h"
#include"CImGuiMgr.h"

#pragma region UI

#pragma region Logo
#include "UI_LogoBG.h"
#include "UI_LogoBanner.h"
#include "UI_LogoTitle.h"
#include "UI_LogoSelector.h"
#include "UI_LogoFlow.h"
#include "UIGroup_Logo.h"
#pragma endregion Logo

#pragma region Loading
#include "UI_LoadingBG.h"
#include "UI_LoadingCircle.h"
#include "UIGroup_Loading.h"
#pragma endregion Loading

#pragma region State
#include "UI_StateBG.h"
#include "UI_StateBar.h"
#include "UI_StateHP.h"
#include "UI_StateEnergy.h"
#include "UI_StateEther.h"
#include "UI_StateSoul.h"
#include "UIGroup_State.h"
#pragma endregion State

#pragma region WeaponSlot
#include "UI_WeaponSlotBG.h"
#include "UI_WeaponSlot.h"
#include "UI_Slot_EquipSign.h"
#include "UI_HUDEffect.h"
#include "UIGroup_WeaponSlot.h"
#pragma endregion WeaponSlot

#pragma region Menu
#include "UI_MenuBG.h"
#include "UI_MenuAlphaBG.h"
#include "UI_MenuFontaine.h"
#include "UI_MenuBtn.h"
#include "UI_Menu_SelectFrame.h"
#include "UIGroup_Menu.h"
#pragma endregion Menu

#pragma region Quick
#include "UI_QuickBG.h"
#include "UI_QuickTop.h"
#include "UI_QuickExplain.h"
#include "UI_QuickInvBG.h"
#include "UIGroup_Quick.h"
#pragma endregion Quick

#pragma region Character
#include "UI_CharacterBG.h"
#include "UI_CharacterTop.h"
#include "UIGroup_Character.h"
#pragma endregion Character

#pragma region Weapon
#include "UI_WeaponRTop.h"
#include "UI_WPEquipSlot.h"
#include "UI_WPEquipNone.h"
#include "UI_WPFontaine.h"
#include "UI_WeaponTab.h"
#include "UIGroup_Weapon.h"
#pragma endregion Weapon

#pragma region Monster
#include "UI_MonsterHP.h"
#include "UI_MonsterHPBar.h"
#include "UIGroup_MonsterHP.h"
#include "UI_BossHP.h"
#include "UI_BossHPBar.h"
#include "UI_BossShield.h"
#include "UIGroup_BossHP.h"
#pragma endregion Monster

#pragma region Item
#include "UI_ItemIcon.h"
#include "UI_DropItemBG.h"
#include "UIGroup_DropItem.h"
#include "ItemData.h"
#pragma endregion Item

#pragma region Script
#include "UI_ScriptBG_Aura.h"
#include "UI_ScriptBG_Npc.h"
#include "UI_Script_DialogBox.h"
#include "UI_Script_NameBox.h"
#include "UI_Script_BGKey.h"
#include "UIGroup_Script.h"
#pragma endregion Script

#pragma region Upgrade
#include "UI_UpgradeBG.h"
#include "UI_UpgradeForge.h"
#include "UI_UpgradeForge_Text.h"
#include "UI_UpgradeCrucible.h"
#include "UIGroup_Upgrade.h"
#include "UI_UpGPageBG.h"
#include "UI_UpGPageTop.h"
#include "UI_UpGPage_NameBox.h"
#include "UI_UpGPage_Circle.h"
#include "UI_UpGPageBtn.h"
#include "UI_UpGPageBtn_Select.h"
#include "UI_UpGPage_Slot.h"
#include "UI_UpGPage_SelectSlot.h"
#include "UI_UpGPage_ItemSlot.h"
#include "UI_UpGPage_MatSlot.h"
#include "UI_UpGPage_Value.h"
#include "UIGroup_UpGPage.h"
#include "UI_UpCompletedBG.h"
#include "UI_UpCompleted_Circle.h"
#include "UI_UpCompleted_Forge.h"
#include "UI_UpCompleted_Crucible.h"
#include "UIGroup_UP_Completed.h"
#include "UI_RedDot.h"
#pragma endregion Upgrade

#pragma region Ch_Upgrade
#include "UI_Ch_UpgradeBtn.h"
#include "UI_Ch_UpgradeBtn_Select.h"
#include "UI_Ch_Upgrade_OkBtn.h"
#include "UIGroup_Ch_Upgrade.h"
#pragma endregion Ch_Upgrade

#pragma region Map
#include "UI_MapBG.h"
#include "UI_MapArea.h"
#include "UI_MapDetail.h"
#include "UI_MapIcon.h"
#include "UI_MapUser.h"
#include "UI_MapPosIcon.h"
#include "UIGroup_Map.h"
#pragma endregion Map

#pragma region Shop
#include "UI_Shop_AnimBG.h"
#include "UI_ShopBG.h"
#include "UI_Shop_SoulBG.h"
#include "UI_ShopSelect.h"
#include "UI_Shop_RemainIcon.h"
#include "UIGroup_Shop.h"
#pragma endregion Shop

#pragma region Setting
#include "UI_Setting_Star.h"
#include "UI_Setting_Overlay.h"
#include "UI_Setting_Btn.h"
#include "UI_Setting_BackBtn.h"
#include "UI_Setting_Sound.h"
#include "UI_Setting_SoundBar.h"
#include "UIGroup_Setting.h"
#pragma endregion Setting

#pragma region QTE
#include "UI_QTE_Btn.h"
#include "UI_QTE_Ring.h"
#include "UI_QTE_Score.h"
#include "UI_QTE_Particle.h"
#include "UI_QTE_Shine.h"
#include "QTE.h"
#pragma endregion QTE

#pragma region BuffTimer
#include "UI_BuffTimer_Bar.h"
#include "UI_BuffTimer_Timer.h"
#include "UI_BuffTimer.h"
#include "UIGroup_BuffTimer.h"
#pragma endregion BuffTimer

#pragma region Level
#include "UI_LevelBG.h"
#include "UI_Level_TextBox.h"
#include "UIGroup_Level.h"
#pragma endregion Level

#pragma region Portal
#include "UI_PortalPic.h"
#include "UI_PortalText.h"
#include "UIGroup_Portal.h"
#pragma endregion Portal

#pragma region BossText
#include "UI_BossTextBG.h"
#include "UI_BossTextBox.h"
#include "UIGroup_BossText.h"
#pragma endregion BossText

#include "UI_MenuPageBG.h"
#include "UI_MenuPageTop.h"
#include "UI_MenuPage_BGAlpha.h"
#include "UI_Slot.h"
#include "UI_Slot_Frame.h"
#include "UIGroup_Inventory.h"
#include "UI_FadeInOut.h"
#include "UI_InvSub_Btn.h"
#include "UI_InvSub_BtnSelect.h"
#include "UIGroup_InvSub.h"
#include "UI_Activate.h"
#include "UI_ScreenBlood.h"
#include "UI_Broken.h"
#include "UI_Dash.h"
#include "UI_Memento.h"
#include "UI_AeonsLost.h"
#include "UI_Cinematic.h"
#include "UI_ArrowSign.h"
#include "UI_PhaseChange.h"
#pragma endregion UI

#pragma region EFFECT
#include "EffectManager.h"
#pragma endregion EFFECT

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pUI_Manager{CUI_Manager::GetInstance()}
#ifdef _DEBUG
	,m_pImGuiMgr{CImGuiMgr::GetInstance()}
#endif // _DEBUG
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUI_Manager);


#ifdef _DEBUG
	Safe_AddRef(m_pImGuiMgr);
#endif // _DEBUG

	/*D3D11_RASTERIZER_DESC		RSDesc;
	ID3D11RasterizerState*		pRSState;
	m_pDevice->CreateRasterizerState();
	m_pContext->RSSetState();

	D3D11_DEPTH_STENCIL_DESC	DSState;

	ID3D11DepthStencilState*	pDSState;
	m_pDevice->CreateDepthStencilState();
	m_pContext->OMSetDepthStencilState();

	D3D11_BLEND_DESC*			pBState;
	ID3D11BlendState*			pBlendState;
	m_pContext->OMSetBlendState();*/

	


	//D3DLIGHT9;
	//m_pDevice->SetLight();

	//D3DMATERIAL9;
	//m_pDevice->SetMaterial();

	//m_pDevice->LightEnble(0, true);

}

HRESULT CMainApp::Initialize()
{

	//LPCWSTR applicationName = L"C:\\Program Files (x86)\\NVIDIA Corporation\\PhysX Visual Debugger 3\\PVD3.exe";
	//CreatePhysXProcess(applicationName);

	
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	/* 엔진 초기화과정을 거친다. ( 그래픽디바이스 초기화과정 + 레벨매니져를 사용할 준비를 한다. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;	

	// Font
	/*if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/NotoSansR.spritefont"))))
		return E_FAIL;*/

	if (FAILED(Ready_Fonts()))
		return E_FAIL;


	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_For_Effects()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;


	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->Ready_ImGui(m_pDevice, m_pContext, m_pGameInstance)))return E_FAIL;);
		//if (FAILED(m_pImGuiMgr->Ready_ImGui(m_pDevice, m_pContext, m_pGameInstance)))
		//	return E_FAIL;


	
	//종료시 파일 삭제
	std::atexit(Client::Delete_File);
	

	return S_OK;
}

void CMainApp::Tick(float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;

	
	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		//wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);
		//Terst
		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}


	
	
	//IMGUI_EXEC(if (FAILED(m_pImGuiMgr->Render_ImGui())) return E_FAIL;);

	/* 그린다. */
	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;


	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->Render_ImGui())) return E_FAIL;);		//프로파일링 하기 위해 Draw다음으로 옮김

//#ifdef _DEBUG
	//if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight15"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f))))
	//	return E_FAIL;
//#endif // _DEBUG

	

	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->End_ImGui())) return E_FAIL;);


	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 142.spritefont
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/142ex.spritefont"))))
		return E_FAIL;

	/*_float3		vPoints[3] = {};

	_ulong		dwByte = {};
	HANDLE		hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
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
	vPoints[2] = _float3(10.f, 0.f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	ID3D11Texture2D*		pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint*			pPixel = new _uint[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	D3D11_SUBRESOURCE_DATA		InitialData{};

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = 256 * 4;
	
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	for (size_t i = 0; i < 256; i++)
	{
		for (size_t j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;
			
			if(j < 10)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	pPixel[0] = D3DCOLOR_ARGB(255, 0, 0, 255);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pPixel, sizeof(_uint) * 256 * 256);

	m_pContext->Unmap(pTexture2D, 0);

	SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"));

	Safe_Delete_Array(pPixel);

	Safe_Release(pTexture2D);*/

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_StartMenu_BG.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Effects()
{
#pragma region SHADER
	// FrameTexture
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FrameTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_FrameTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstanceRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstanceMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Mesh.hlsl"), VTXPARTICLE_MESH::Elements, VTXPARTICLE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstancePoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_NewPoint.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	//DistortionEffect
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_DistortionEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DistortionRect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	//MeshEffect
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;


#pragma endregion SHADER
#pragma region TEXTURE
	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.dds"), 46))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SwordTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Trail/T_SwordTrail01.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SwordTrail_Rotated"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Trail/SwordTrail_Rotated.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve39"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 39))))
		return E_FAIL;

	//불
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Fire/fire01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Fire/noise01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireAlpha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Fire/alpha01.dds"), 1))))
		return E_FAIL;

	//퍼린 노이즈
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PerlinNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Noise/Perlin%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Fire"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Fire.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AndrasRain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Andras_0724/AndrasLazer/MyRainTexture.dds"), 1))))
		return E_FAIL;

	//HorizonTexture
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GradiantTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Opacity/CircleGradient.dds"), 1))))
		return E_FAIL;

#pragma endregion TEXTURE
#pragma region Component
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sword_Trail"),
		CVIBuffer_SwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* SwordTrailShader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Sword_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_SwordTrail.hlsl"), SwordTrailVertex::Elements, SwordTrailVertex::iNumElements))))
		return E_FAIL;
#pragma endregion Component
#pragma region MODEL
	_matrix		PreTransformMatrix;
	//Slash
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Slash.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Cube
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/WhiteCube.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Circle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/ParticleCircle.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Leaf0
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Leaf0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/GinkgoLeaf.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Leaf1
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Leaf1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Leaf.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Grass
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GrassParticle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/GrassParticle.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Rock
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RockParticle1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/RockParticle0.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Rock2
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RockParticle2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/RockParticle1.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Bubble
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bubble_Mesh"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Bubble_Mesh.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LightningProp
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_InstanceLightning"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Jugglus_Lightning.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Blade
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blade_Slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Blade.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Blade2
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blade_Slash_Long"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/NewBlade.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Needle
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Needle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/Needle.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Tornado
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TornadoWind"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TornadoEffect/New/NewTornado_Wind_Core.fbx", PreTransformMatrix))))
		return E_FAIL;
	//2
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TornadoWind2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TornadoEffect/New/NewTornado_Wind_Second.fbx", PreTransformMatrix))))
		return E_FAIL;
	//3
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TornadoWind3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TornadoEffect/New/NewTornado_Wind_Third.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Ring
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TornadoRing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TornadoEffect/New/NewTornado_Ring.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TornadoBottom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TornadoEffect/TornadoRoot.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Heal Effect
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ribbon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spiral_Thick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HealingEffect/Spiral_Thick.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SmoothLine"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HealingEffect/SmoothLine.fbx", PreTransformMatrix))))
		return E_FAIL;


	//AndrasLazer
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Lazer_Base"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/AndrasLazer/AndrasLazerQuarter.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Lazer_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/AndrasLazer/New_LazerCylinder.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_HelixCast"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/HelixCast/NewHelixCast.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BezierCurve"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/HelixCast/BezierCurve.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_NewAspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/HelixCast/NewAspriation.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_LazerCast"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/HelixCast/NewLazerCast.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lazer_Lightning"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/AndrasLazerLightning.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ShieldSphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShieldShphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShieldSphere/ShieldSphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Screw"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/AndrasLazer/AndrasScrew.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//Swing Spiral
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Swing_Spiral"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SwingEffect/Swing_Spiral.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Vane
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Vane"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HealingEffect/Vane.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Large_Ribbon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Large_Ribbon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HealingEffect/Large_Ribbon.fbx", PreTransformMatrix))))
		return E_FAIL;

	//DefaultCylinder
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DefaultCylinder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InstanceModel/DefaultCylinder.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Meteor
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteor_Core"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Meteor/Meteor.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteor_Crater1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Meteor/Rock_Impact/R_Impact0.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteor_Crater2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Meteor/Rock_Impact/R_Impact1.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteor_Wind"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Meteor/MeteorWind.fbx", PreTransformMatrix))))
		return E_FAIL;


	//FirePillar
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirePillar1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/FirePillar/FirePillar.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirePillar2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/FirePillar/FirePillar2.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirePillar4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/FirePillar/FirePillar_Core.fbx", PreTransformMatrix))))
		return E_FAIL;
	//FirePillarBottom
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirePillar_Bottom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/FirePillar/FirePillar_Bottom.fbx", PreTransformMatrix))))
		return E_FAIL;
	//FirePillarCharge
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirePillar_Charge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HealingEffect/PillarBottom.fbx", PreTransformMatrix))))
		return E_FAIL;
	//RockGround
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_Ground"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Meteor/Rock_Impact/R_Impact2.fbx", PreTransformMatrix))))
		return E_FAIL;

	//가시 패턴
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hedgehog"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hedgehog/Hedgehog.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_NeedleSpawner"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hedgehog/NeedleSpawner.fbx", PreTransformMatrix))))
		return E_FAIL;

	//검기
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f)) * XMMatrixRotationZ(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GroundSlash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GroundSlash/GroundSlash2.fbx", PreTransformMatrix))))
		return E_FAIL;

	//쉴드
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HexaShield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HexaShield/HexaShield.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HexaShield_MK2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HexaShield/HexaShield_Mk3.fbx", PreTransformMatrix))))
		return E_FAIL;

	//블랙홀
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BlackHoleRing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BlackHole/BlackHole_Ring.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BlackHoleRing2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/BlackHole/BlackHole_Ring2.fbx", PreTransformMatrix))))
		return E_FAIL;

	//안드라스 컷신
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Cut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Andras_0724/AndrasCutScene.fbx", PreTransformMatrix))))
		return E_FAIL;

	//AndrasCut
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AndrasPillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Pillar/AndrasPillar.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Andras_0724/Pillar/AndrasSphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion MODEL

	if (FAILED(EFFECTMGR->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_UI()
{
	// UI Texture
	if (FAILED(Ready_Texture_UI()))
		return E_FAIL;

	// UI 원형
	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_Mouse"), TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	// UI 생성
	if (FAILED(m_pUI_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(CInventory::GetInstance()->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Texture_UI()
{
#pragma region UI_Texture

#pragma region Logo
	/* Prototype_Component_Texture_LogoBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_StartMenu_BG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_Banner */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_Banner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Logo_Banner.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_Selector */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_Selector"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Logo_Selector.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Title.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LogoFlow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_PixelEffect1.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LogoFlowMask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoFlowMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_PixelEffectMask.dds"), 1))))
		return E_FAIL;

#pragma endregion Logo

#pragma region Loading
	/* Prototype_Component_Texture_LoadingBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/fond_loading2.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LoadingCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/T_Circle_loading%d.dds"), 2))))
		return E_FAIL;

#pragma endregion Loading

#pragma region HUD
#pragma region State
	/* Prototype_Component_Texture_HUD_StateBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_StateBG.dds"), 1)))) // dds 이슈?
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_Bar.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_HP.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateEnergy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateEnergy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_Energy.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateEther */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateEther"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_Ether.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateSoul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateSoul"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_Soul.dds"), 1))))
		return E_FAIL;
#pragma endregion State

#pragma region WeaponSlot
	/* Prototype_Component_Texture_HUD_WeaponSlotBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_WeaponSlotBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/WeaponSlot/HUD_WeaponSlotBG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_HUDEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HUDEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/WeaponSlot/HUDEffect.png"), 1))))
		return E_FAIL;
#pragma endregion WeaponSlot
#pragma endregion HUD

#pragma region Icon
#pragma region Weapon
	/* Prototype_Component_Texture_Icon_Durgas_Claymore */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Durgas_Claymore"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Durgas_Claymore.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Elish */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Elish"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Elish.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Pretorian*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Pretorian"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Pretorian.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Radamanthes*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Radamanthes"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Radamanthes_Dagger.dds"), 1))))
		return E_FAIL;
#pragma endregion Weapon

#pragma region Skill
	/* Prototype_Component_Texture_Icon_Oph */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Oph"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Oph.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Aksha */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Aksha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Aksha.dds"), 1))))
		return E_FAIL;
#pragma endregion Skill

#pragma region Item

#pragma region DropItem
	/* Prototype_Component_Texture_Icon_Item_Buff0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Buff0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Buff0.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Buff1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Buff1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Buff1.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Buff2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Buff2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Buff2.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Buff3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Buff3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Buff3.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Essence */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Essence"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Essence.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Ether */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Ether"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Ether.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Upgrade0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Upgrade0.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Item_Upgrade1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Item_Upgrade1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Upgrade1.dds"), 1))))
		return E_FAIL;

#pragma endregion DropItem

	/* Prototype_Component_Texture_Icon_Whisperer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Whisperer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Whisperer.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Firefly */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Firefly"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Firefly.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_HoverBoard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_HoverBoard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_HoverBoard.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_Catalyst */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Catalyst"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_AncientCatalyst.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Item.png"), 1))))
		return E_FAIL;
#pragma endregion Item

	/* Prototype_Component_Texture_DropItemBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DropItemBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/DropItem.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_ItemIcon_None */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon_None"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/ItemIcon_None.dds"), 1))))
		return E_FAIL;

#pragma endregion Icon

#pragma region Menu

	/* Prototype_Component_Texture_MenuBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/T_IGGOUI_BG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuFontaine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuFontaine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/T_IGGOUI_Fontaine.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuAlphaBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuAlphaBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Unknown/T_SmallCloud.png"), 1))))
		return E_FAIL;

#pragma region Btn
	/* Prototype_Component_Texture_MenuBtn_Ch */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Ch"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Ch_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Cod */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Cod"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Cod_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Inv */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Inv"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Inv_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Map"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Map_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Set */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Set"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Set_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_WP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_WP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_WP_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Menu_SelectFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_SelectFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Menu_SelectFrame.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Menu_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Menu_Mask.dds"), 1))))
		return E_FAIL;
#pragma endregion Btn

#pragma endregion Menu

#pragma region Quick
	/* Prototype_Component_Texture_UI_QuickBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/T_Weapons_Overlay.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/T_Character_TopImage.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickInvBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickInvBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/QuickInvBG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickExplain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickExplain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/QuickExplain.dds"), 1))))
		return E_FAIL;
#pragma endregion Quick

#pragma region Inventory
	/* Prototype_Component_Texture_UI_InvSub_Btn_None */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InvSub_Btn_None"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InvSub_Btn_None.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_InvSub_Btn_Select */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InvSub_Btn_Select"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InvSub_Btn_Select.dds"), 1))))
		return E_FAIL;
#pragma endregion Inventory

#pragma region Character
	/* Prototype_Component_Texture_UI_CharacterBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/T_background_character_sheet.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_CharacterTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/Ch_Top.dds"), 1))))
		return E_FAIL;
#pragma endregion Character

#pragma region Slot
	/* Prototype_Component_Texture_UI_Slot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Slot.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Slot_SelectFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot_SelectFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Slot_SelectFrame.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_EquipSign */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_EquipSign"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Inventory_InQANotif.dds"), 1))))
		return E_FAIL;
#pragma endregion Slot

#pragma region MenuPage
	/* Prototype_Component_Texture_UI_MenuPageBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MenuPageBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Options_BG.dds"), 1))))
		return E_FAIL;
#pragma endregion MenuPage

#pragma region Weapon
	/* Prototype_Component_Texture_UI_WeaponRTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponRTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/RightTop.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WeaponEquipSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponEquipSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/EquipSlot.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WPFontaine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WPFontaine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/Fontaine_%d.dds"), 3))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WPEquipNone */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WPEquipNone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/EquipSlot_None.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WeaponTab */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponTab"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/Tab.dds"), 1))))
		return E_FAIL;
#pragma endregion Weapon

#pragma region Monster
	/* Prototype_Component_Texture_UI_MonsterHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MonsterHP.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_MonsterHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MonsterHPBar.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BossHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossHP.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BossHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossHPBar.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BossShield */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossShield"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossShield.png"), 1)))) // dds 변환 필요
		return E_FAIL;
#pragma endregion Monster

#pragma region Script
	/* Prototype_Component_Texture_Script_NameBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_NameBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/T_Dialog_NameBox.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_DialogBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_DialogBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/T_Dialog_Text_Box_Whisperer.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Aura */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Aura"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/T_NPC_NaiAura%d.dds"), 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Npc_Rlya */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_Rlya"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/T_NPC_Rlya.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Npc_Valnir */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_Valnir"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/T_NPC_Valnir.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Npc_ChoronBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_ChoronBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/Choron_BG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Npc_ChoronKey */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_ChoronKey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/Choron_Key_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Script_Npc_YaakBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Npc_YaakBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Script/YaakBG.dds"), 1))))
		return E_FAIL;
#pragma endregion Script

#pragma region Upgrade
	/* Prototype_Component_Texture_Upgrade_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/CRAFT_background_home.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Upgrade_Forge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Forge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Forge.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Upgrade_Forge_Text */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Forge_Text"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Forge_Text.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Upgrade_Crucible */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Crucible"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Upgrade_Crucible.dds"), 1))))
		return E_FAIL;

#pragma region UpGPage
	/* Prototype_Component_Texture_UpGPage_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/CRAFT_background_forge__1_.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/CRAFT_background_forge_circle.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_Top */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Top"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Upgrade_Top.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_Slot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Upgrade_Slot.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_SlotSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_SlotSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Upgrade_SlotSelect.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_value */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_value"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Upgrade_value.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_Btn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Btn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/UpgradeBtn.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_BtnSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_BtnSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/BtnSelect.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UpGPage_MatSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_MatSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/UpgradeMat_Slot.dds"), 1))))
		return E_FAIL;
#pragma endregion UpGPage

#pragma region Up_Completed
	/* Prototype_Component_Texture_UI_UpCompleted_Circle_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Circle_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/T_circle_cinematic.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_UpCompleted_Circle_2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Circle_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/T_circle_cinematic_small.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_UpCompleted_Forge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Forge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/CRAFT_background_forge_circle2.dds"), 1))))
		return E_FAIL;
#pragma endregion Up_Completed

#pragma region Ch_Upgrade
	/* Prototype_Component_Texture_Ch_Upgrade_Btn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ch_Upgrade_Btn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/T_Weapons_Slot.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Ch_Upgrade_BtnSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ch_Upgrade_BtnSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/T_LevleUp_Select.dds"), 1))))
		return E_FAIL;
#pragma endregion Ch_Upgrade

#pragma endregion Upgrade

#pragma region Map
	/* Prototype_Component_Texture_MapBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/T_Plaines_Map.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Map_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/Map_Icon_%d.dds"), 6))))
		return E_FAIL;

	/* Prototype_Component_Texture_Map_Area */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Area"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/Map_Area.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Map_Detail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Detail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/Map_Detail.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Map_PosIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_PosIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/Map_PosIcon.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Map_User */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_User"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Map/Map_User.dds"), 1))))
		return E_FAIL;
#pragma endregion Map

#pragma region Shop
	/* Prototype_Component_Texture_UI_ShopBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ShopBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopBG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_ShopSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ShopSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopSelect.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_SoulBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SoulBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/SoulBG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_RemainIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_RemainIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Shop_RemainIcon.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_ValnirBG_Anim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ValnirBG_Anim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/AnimBG.dds"), 1))))
		return E_FAIL;

#pragma endregion Shop

#pragma region Setting
	/* Prototype_Component_Texture_UI_Setting_Star */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/star.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Setting_OnOffBtn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_OnOffBtn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/OnOffBtn.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Setting_Overlay */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Overlay"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/T_Options_Overlay.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Setting_BackBtn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_BackBtn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/Setting_BackBtn.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Setting_Sound */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Sound"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/SoundBtn.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Setting_SoundBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_SoundBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Setting/SoundBar.dds"), 1))))
		return E_FAIL;
#pragma endregion Setting

#pragma region QTE
	/* Prototype_Component_Texture_UI_QTE_Btn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Btn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Score_Bad */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Score_Bad"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Bad.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Score_Good */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Score_Good"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Good.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Score_Perfect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Score_Perfect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Perfect.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Ring_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Particle.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QTE_Shine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Shine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QTE/Qte_Shine.dds"), 1))))
		return E_FAIL;
#pragma endregion QTE

#pragma region ETC
	/* Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ShatteredCursor.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_FadeInOut */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FadeInOut"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FadeInOut.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BG_Alpha */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BG_Alpha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BG_Alpha.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Activate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Activate"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Activate.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_ScreenBlood */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ScreenBlood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_ScreenBlood.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Broken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Broken.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dash_%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BuffTimer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BuffTimer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BuffTimer.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Memento */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Memento"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Memento.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_AeonsLost*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_AeonsLost"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/AeonsLost.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_RedDot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_RedDot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/RedDot.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Level_BG*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Level_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Level_BG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Level_TextBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Level_TextBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Level_TextBox.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Cinematic*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Cinematic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_front_Plan_cinematic.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_ArrowSign */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ArrowSign"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ArrowSign.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_PortalPic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PortalPic/PortalPic_%d.png"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_PortalText */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PortalPic/Portal_Text.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_PortalPic_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Noise/T_SpiralErode.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_PortalPic_Opacity */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic_Opacity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Noise/T_ErodeClouds.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_PhaseChange */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PhaseChange"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PhaseChange.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BossTextBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossTextBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossTextBG.png"), 1))))
		return E_FAIL;
#pragma endregion ETC

#pragma endregion UI_Texture

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_UI()
{

	/* For.Prototype_Component_Shader_VtxUITex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUITex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

#pragma region UI_Obj
	// UI 객체 원형 생성

	// Mouse
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"), CMouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Logo
	/* For.Prototype_GameObject_UI_LogoBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoBG"),
		CUI_LogoBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LogoBanner*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoBanner"),
		CUI_LogoBanner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LogoTitle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoTitle"),
		CUI_LogoTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LogoSelector*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoSelector"),
		CUI_LogoSelector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LogoFlow*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoFlow"),
		CUI_LogoFlow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Logo*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Logo"),
		CUIGroup_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion Logo

#pragma region Loading
	/* For.Prototype_GameObject_UI_LoadingBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingBG"),
		CUI_LoadingBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingCircle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingCircle"),
		CUI_LoadingCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UIGroup_Loading*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Loading"),
		CUIGroup_Loading::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Loading

#pragma region HUD

#pragma region State
	/* For.Prototype_GameObject_UI_StateBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateBG"),
		CUI_StateBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateBar"),
		CUI_StateBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateHP"),
		CUI_StateHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateEnergy*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateEnergy"),
		CUI_StateEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateEther*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateEther"),
		CUI_StateEther::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateSoul*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateSoul"),
		CUI_StateSoul::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_State*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_State"),
		CUIGroup_State::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion State

#pragma region WeaponSlot
	/* For.Prototype_GameObject_UI_WeaponSlotBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponSlotBG"),
		CUI_WeaponSlotBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WeaponSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponSlot"),
		CUI_WeaponSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_HUDEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HUDEffect"),
		CUI_HUDEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_WeaponSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_WeaponSlot"),
		CUIGroup_WeaponSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion WeaponSlot

#pragma endregion HUD

#pragma region Menu
	/* For.Prototype_GameObject_UI_MenuBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuBG"),
		CUI_MenuBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuAlphaBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuAlphaBG"),
		CUI_MenuAlphaBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuFontaine*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuFontaine"),
		CUI_MenuFontaine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuBtn"),
		CUI_MenuBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Menu_SelectFrame*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Menu_SelectFrame"),
		CUI_Menu_SelectFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UIGroup_Menu*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Menu"),
		CUIGroup_Menu::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Menu

#pragma region Quick
	/* For.Prototype_GameObject_UI_QuickBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickBG"),
		CUI_QuickBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickTop"),
		CUI_QuickTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickExplain*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickExplain"),
		CUI_QuickExplain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickInvBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickInvBG"),
		CUI_QuickInvBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Quick*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Quick"),
		CUIGroup_Quick::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Quick

#pragma region Character
	/* For.Prototype_GameObject_UIGroup_CharacterBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_CharacterBG"),
		CUI_CharacterBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_CharacterTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_CharacterTop"),
		CUI_CharacterTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Character*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Character"),
		CUIGroup_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Character

#pragma region Inventory
	/* For.Prototype_GameObject_UI_InvSub_Btn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InvSub_Btn"),
		CUI_InvSub_Btn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_InvSub_BtnSelect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InvSub_BtnSelect"),
		CUI_InvSub_BtnSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Inventory

#pragma region Slot
	/* For.Prototype_GameObject_UI_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Slot"),
		CUI_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Slot_Frame*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Slot_Frame"),
		CUI_Slot_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Slot_EquipSign*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Slot_EquipSign"),
		CUI_Slot_EquipSign::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Slot

#pragma region MenuPage
	/* For.Prototype_GameObject_UI_MenuPageBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuPageBG"),
		CUI_MenuPageBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuPageTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuPageTop"),
		CUI_MenuPageTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuPage_BGAlpha*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuPage_BGAlpha"),
		CUI_MenuPage_BGAlpha::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion MenuPage

#pragma region Weapon
	/* For.Prototype_GameObject_UI_WeaponRTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponRTop"),
		CUI_WeaponRTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPEquipSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPEquipSlot"),
		CUI_WPEquipSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPEquipNone*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPEquipNone"),
		CUI_WPEquipNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPFontaine*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPFontaine"),
		CUI_WPFontaine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WeaponTab*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponTab"),
		CUI_WeaponTab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Weapon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Weapon"),
		CUIGroup_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Weapon

#pragma region Monster
	/* For.Prototype_GameObject_UI_MonsterHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHP"),
		CUI_MonsterHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MonsterHPBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHPBar"),
		CUI_MonsterHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_MonsterHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_MonsterHP"),
		CUIGroup_MonsterHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHP"),
		CUI_BossHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossHPBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHPBar"),
		CUI_BossHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossShield*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossShield"),
		CUI_BossShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_BossHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_BossHP"),
		CUIGroup_BossHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Monster

#pragma region Item
	/* For.Prototype_GameObject_UI_ItemIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemIcon"),
		CUI_ItemIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_DropItemBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DropItemBG"),
		CUI_DropItemBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_DropItem*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_DropItem"),
		CUIGroup_DropItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Item

#pragma region Script
	/* For.Prototype_GameObject_UI_ScriptBG_Aura*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ScriptBG_Aura"),
		CUI_ScriptBG_Aura::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ScriptBG_Npc*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ScriptBG_Npc"),
		CUI_ScriptBG_Npc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Script_DialogBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Script_DialogBox"),
		CUI_Script_DialogBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ScriptBG_NameBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ScriptBG_NameBox"),
		CUI_Script_NameBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Script_BGKey*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Script_BGKey"),
		CUI_Script_BGKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Script*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Script"),
		CUIGroup_Script::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Script

#pragma region Upgrade
	/* For.Prototype_GameObject_UIGroup_UpgradeBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_UpgradeBG"),
		CUI_UpgradeBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Upgrade_Forge*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Upgrade_Forge"),
		CUI_UpgradeForge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Upgrade_Forge_Text*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Upgrade_Forge_Text"),
		CUI_UpgradeForge_Text::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Upgrade_Crucible*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Upgrade_Crucible"),
		CUI_UpgradeCrucible::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Upgrade*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Upgrade"),
		CUIGroup_Upgrade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region UpGPage
	/* For.Prototype_GameObject_UIGroup_UpGPageBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPageBG"),
		CUI_UpGPageBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPageTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPageTop"),
		CUI_UpGPageTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_NameBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_NameBox"),
		CUI_UpGPage_NameBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_Circle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_Circle"),
		CUI_UpGPage_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPageBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPageBtn"),
		CUI_UpGPageBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPageBtn_Select*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPageBtn_Select"),
		CUI_UpGPageBtn_Select::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_Slot"),
		CUI_UpGPage_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_SelectSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_SelectSlot"),
		CUI_UpGPage_SelectSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_ItemSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_ItemSlot"),
		CUI_UpGPage_ItemSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_MatSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_MatSlot"),
		CUI_UpGPage_MatSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UpGPage_Value*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage_Value"),
		CUI_UpGPage_Value::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UIGroup_UpGPage*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UpGPage"),
		CUIGroup_UpGPage::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion UpGPage

#pragma region UP_Completed
	/* For.Prototype_GameObject_UI_UpCompletedBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_UpCompletedBG"),
		CUI_UpCompletedBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_UpCompleted_Circle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_UpCompleted_Circle"),
		CUI_UpCompleted_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_UpCompleted_Forge*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_UpCompleted_Forge"),
		CUI_UpCompleted_Forge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_UpCompleted_Crucible*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_UpCompleted_Crucible"),
		CUI_UpCompleted_Crucible::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_UP_Completed*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_UP_Completed"),
		CUIGroup_UP_Completed::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion UP_Completed

#pragma endregion Upgrade

#pragma region Ch_Upgrade
	/* For.Prototype_GameObject_UIGroup_Ch_Upgrade_Btn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_Btn"),
		CUI_Ch_UpgradeBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Ch_Upgrade_BtnSelect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_BtnSelect"),
		CUI_Ch_UpgradeBtn_Select::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Ch_Upgrade_OKBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_OKBtn"),
		CUI_Ch_Upgrade_OkBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Ch_Upgrade*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade"),
		CUIGroup_Ch_Upgrade::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Ch_Upgrade

#pragma region Map
	/* For.Prototype_GameObject_UI_MapBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapBG"),
		CUI_MapBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MapArea*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapArea"),
		CUI_MapArea::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MapDetail*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapDetail"),
		CUI_MapDetail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MapIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapIcon"),
		CUI_MapIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MapUser*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapUser"),
		CUI_MapUser::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MapPosIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MapPosIcon"),
		CUI_MapPosIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Map*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Map"),
		CUIGroup_Map::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Map

#pragma region Shop
	/* For.Prototype_GameObject_UI_Shop_AnimBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Shop_AnimBG"),
		CUI_Shop_AnimBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ShopBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ShopBG"),
		CUI_ShopBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Shop_SoulBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Shop_SoulBG"),
		CUI_Shop_SoulBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ShopSelect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ShopSelect"),
		CUI_ShopSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Shop_RemainIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Shop_RemainIcon"),
		CUI_Shop_RemainIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Shop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Shop"),
		CUIGroup_Shop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Shop

#pragma region Setting
	/* For.Prototype_GameObject_UI_Setting_Star*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_Star"),
		CUI_Setting_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Setting_Overlay*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_Overlay"),
		CUI_Setting_Overlay::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Setting_Btn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_Btn"),
		CUI_Setting_Btn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Setting_BackBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_BackBtn"),
		CUI_Setting_BackBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Setting_Sound*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_Sound"),
		CUI_Setting_Sound::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Setting_SoundBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Setting_SoundBar"),
		CUI_Setting_SoundBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Setting*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Setting"),
		CUIGroup_Setting::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Setting

#pragma region QTE
	/* For.Prototype_GameObject_UI_QTE_Btn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QTE_Btn"),
		CUI_QTE_Btn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QTE_Ring*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QTE_Ring"),
		CUI_QTE_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QTE_Score*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QTE_Score"),
		CUI_QTE_Score::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QTE_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QTE_Particle"),
		CUI_QTE_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QTE_Shine*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QTE_Shine"),
		CUI_QTE_Shine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_QTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE"),
		CQTE::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion QTE

#pragma region BuffTimer
	/* For.Prototype_GameObject_UI_BuffTimer_Bar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BuffTimer_Bar"),
		CUI_BuffTimer_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BuffTimer_Timer*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BuffTimer_Timer"),
		CUI_BuffTimer_Timer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BuffTimer*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BuffTimer"),
		CUI_BuffTimer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_BuffTimer*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_BuffTimer"),
		CUIGroup_BuffTimer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion BuffTimer

#pragma region Level
	/* For.Prototype_GameObject_UI_LevelBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LevelBG"),
		CUI_LevelBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Level_TextBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Level_TextBox"),
		CUI_Level_TextBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Level*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Level"),
		CUIGroup_Level::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Level

#pragma region Portal
	/* For.Prototype_GameObject_UI_PortalPic*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PortalPic"),
		CUI_PortalPic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PortalText*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PortalText"),
		CUI_PortalText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Portal*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Portal"),
		CUIGroup_Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Portal

#pragma region BossText
	/* For.Prototype_GameObject_UI_BossTextBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossTextBG"),
		CUI_BossTextBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_BossTextBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossTextBox"),
		CUI_BossTextBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_BossText*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_BossText"),
		CUIGroup_BossText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion BossText

#pragma region ETC
	/* For.Prototype_GameObject_UIGroup_Inventory*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Inventory"),
		CUIGroup_Inventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_FadeInOut*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FadeInOut"),
		CUI_FadeInOut::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_InvSub*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_InvSub"),
		CUIGroup_InvSub::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ItemData*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemData"),
		CItemData::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Activate*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Activate"),
		CUI_Activate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ScreenBlood*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ScreenBlood"),
		CUI_ScreenBlood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Broken*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Broken"),
		CUI_Broken::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Dash*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Dash"),
		CUI_Dash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Memento*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Memento"),
		CUI_Memento::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AeonsLost*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AeonsLost"),
		CUI_AeonsLost::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_RedDot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_RedDot"),
		CUI_RedDot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Cinematic*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Cinematic"),
		CUI_Cinematic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ArrowSign*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ArrowSign"),
		CUI_ArrowSign::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PhaseChange*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PhaseChange"),
		CUI_PhaseChange::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion ETC

#pragma endregion UI_Obj

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_12.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo13"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_13.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo15"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_15.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo17"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_17.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo23"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_23.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo24"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_24.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo25"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_25.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo27"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_27.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo35"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_35.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_HeirofLight12"), TEXT("../Bin/Resources/Fonts/HeirofLight12.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_HeirofLight13"), TEXT("../Bin/Resources/Fonts/HeirofLight13.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_HeirofLight15"), TEXT("../Bin/Resources/Fonts/HeirofLight15.spritefont"))))
		return E_FAIL;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	

	CInventory::GetInstance()->DestroyInstance();

	Safe_Release(m_pUI_Manager);
	CUI_Manager::GetInstance()->DestroyInstance();
	//CUI_Manager::DestroyInstance();
	/* 레퍼런스 카운트를 0으로만든다. */





#ifdef _DEBUG


	m_pImGuiMgr->DestroyInstance();
	CImGuiMgr::DestroyInstance();
#endif // _DEBUG

	EFFECTMGR->DestroyInstance();
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();



}


#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

#include "QuarterCamera.h"
#include "ImguiMgr.h"
#include "GameObject.h"
#include "Level_Loading.h"
#include "Map_Flat.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
	//ㅁ
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	return S_OK;
}



void CLevel_GamePlay::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
#endif
	
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{

}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CQuarterCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0, 10.0f, -20.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);

	m_pGameInstance->Add_Light(LightDesc);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CFlatMap::MAP_DEC desc{};
	desc.Pos = _float4(0.f, 0.f, 0.f, 1.f);
	desc.Scale = _float3(1.f, 1.f, 1.f);

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("LayerFlat"), TEXT("Prototype_GameObject_FlatMap"), &desc);

	CGameObject* Camera = m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
	CGameObject* Map = m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("LayerFlat"));

	static_cast<CQuarterCamera*>(Camera)->Set_Target(Map);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("LayerDummy"), TEXT("Prototype_GameObject_PlayerDummy"));


	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}

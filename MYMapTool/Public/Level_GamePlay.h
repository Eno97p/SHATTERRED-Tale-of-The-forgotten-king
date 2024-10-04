#pragma once

#include "Level.h"
#include "MYMapTool_Defines.h"
#include "LandObject.h"

BEGIN(MYMapTool)

class CLevel_GamePlay final : public CLevel
{
private:
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT	Initialize() override;
	virtual void	Tick(_float fTimeDelta) override;

private:
	HRESULT	Ready_Light();
	HRESULT	Ready_Layer_Terrain(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_EnvEffects(const wstring& strLayerTag);	//환경 이펙트 

	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc);

	// Load 함수와 Save 함수 필요
	HRESULT	Save_Data();
	HRESULT	Load_Data();
	HRESULT Save_Data_PhysX();
	HRESULT Load_Data_PhysX();

	HRESULT	Save_Data_Effects();
	HRESULT	Load_Data_Effects();

	HRESULT	Save_Data_Decals();
	HRESULT	Load_Data_Decals();

	const wchar_t* Setting_FileName();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;


};

END
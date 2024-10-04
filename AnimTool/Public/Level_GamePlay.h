#pragma once

#include "Level.h"
#include "AnimTool_Defines.h"

BEGIN(AnimTool)

class CLevel_GamePlay final : public CLevel
{
private:
	enum STAGE_NAME{STAGE_HOME, STAGE_ONE, STAGE_TWO, STAGE_THREE, STAGE_BOSS, STAGE_END};
	enum MODEL_NAME { MODEL_PLAYAER, MODEL_END };

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

	// Load 함수와 Save 함수 필요
	HRESULT	Save_Data();
	HRESULT	Load_Data();

	const wchar_t* Setting_FileName();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;
};

END
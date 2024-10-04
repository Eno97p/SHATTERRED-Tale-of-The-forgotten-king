#pragma once

#include "AnimTool_Defines.h"
#include "Base.h"
#include "Imgui_Manager.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(AnimTool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	void	Tick(_float fTimeDelta);
	HRESULT	Render();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };

	CImgui_Manager*			m_pImgui_Manager = { nullptr };

	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = {};
	_uint					m_iNumRender = {};

public:
	HRESULT	Open_Level(LEVEL eLevelID);
	HRESULT	Ready_Prototype_GameObject();
	HRESULT	Ready_Prototype_Component();

	HRESULT	Ready_NavigationCell(); // Navigation 복습을 위해 임의로 만든 함수

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

END
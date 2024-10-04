#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUI_Manager;
class CImGuiMgr;
class CInventory;

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };
	CImGuiMgr*				m_pImGuiMgr = { nullptr };
	CUI_Manager*			m_pUI_Manager = { nullptr };
	

private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = {};
	_uint					m_iNumRender = {};

public:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Gara();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_For_Effects();
	HRESULT	Ready_UI();
	HRESULT	Ready_Texture_UI();
	HRESULT	Ready_Prototype_UI();

	HRESULT	Ready_Fonts();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END
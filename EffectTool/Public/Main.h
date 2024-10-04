#pragma once
#include "Base.h"
#include "Effect_Define.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Effect)
class CMainEffect :
    public CBase
{
private:
	CMainEffect();
	virtual ~CMainEffect() = default;

public:
	HRESULT Initialize();
	void Tick(float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };
	class CImguiMgr* m_pImguiMgr = { nullptr };
public:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();


public:
	static CMainEffect* Create();
	virtual void Free() override;

};

END
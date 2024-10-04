#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 화면에 로딩 씬을 보여준다 + 다음 레벨을 위한 리소스를 준비하낟. */

BEGIN(Client)
class CUI_Manager;

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;


private:
	LEVEL				m_eNextLevel = { LEVEL_END };

	class CLoader*		m_pLoader = { nullptr };

	CUI_Manager*		m_pUI_Manager = nullptr;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END
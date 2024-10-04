#pragma once

#include "Level.h"
#include "AnimTool_Defines.h"

BEGIN(AnimTool)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT	Initialize(LEVEL eNextLevel);
	virtual void	Tick(_float fTimeDelta) override;

private:
	LEVEL			m_eNextLevel = { LEVEL_END };
	class CLoader*	m_pLoader = { nullptr };

public:
	HRESULT	Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	static CLevel_Loading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void			Free() override;
};

END
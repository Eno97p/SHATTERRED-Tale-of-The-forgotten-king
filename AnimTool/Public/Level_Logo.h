#pragma once

#include "Level.h"
#include "AnimTool_Defines.h"

BEGIN(AnimTool)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT	Initialize() override;
	virtual void	Tick(_float fTimeDelta) override;

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void	Free() override;
};

END
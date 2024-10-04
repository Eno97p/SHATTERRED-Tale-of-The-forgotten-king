#pragma once
#include "C:\asdf\EngineSDK\Inc\Base.h"
#include "AnimTool_Defines.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(AnimTool)
class CParticleManager final : public CBase
{
	DECLARE_SINGLETON(CParticleManager)
public:
	CParticleManager();
	virtual ~CParticleManager() = default;
public:
	HRESULT Initialize();
	HRESULT Create_Particle(_uint iIndex, _vector vStart, _vector vDir = XMVectorZero());
	HRESULT Create_Particle(_uint iIndex, _vector vStart, CGameObject* pTarget = nullptr);
	HRESULT Create_Particle(_uint iIndex, _vector vStart, _vector vAxis, _float fRadian);

public:
	virtual void Free() override;
};

END
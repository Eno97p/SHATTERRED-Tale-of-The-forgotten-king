#include "ParticleManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
}

HRESULT CParticleManager::Initialize()
{
    return E_NOTIMPL;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, _vector vDir)
{
    return E_NOTIMPL;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, CGameObject* pTarget)
{
    return E_NOTIMPL;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, _vector vAxis, _float fRadian)
{
    return E_NOTIMPL;
}

void CParticleManager::Free()
{
}

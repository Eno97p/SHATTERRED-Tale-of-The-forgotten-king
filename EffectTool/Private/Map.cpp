#include "Map.h"

#include "GameInstance.h"

CMap::CMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMap::CMap(const CMap & rhs)
	: CGameObject{ rhs }
{
}



void CMap::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

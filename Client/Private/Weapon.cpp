#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"
#include "EffectManager.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CPartObject{ rhs }
{
}

void CWeapon::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

void CWeapon::Generate_Trail(_int iIndex)
{
	if (m_GenerateTrail == false)
		return;
	else
	{
		CEffectManager::GetInstance()->Generate_SwordTrail(iIndex, &m_WorldMatrix);
		m_GenerateTrail = false;
	}
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

#include "..\Public\Weapon.h"

#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CPartObject{ rhs }
{
}

void CWeapon::Set_Weapon_Offset(WEAPON_DESC* pDesc, CTransform* TransCom)
{
	TransCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vOffset), 1.f));
}

void CWeapon::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

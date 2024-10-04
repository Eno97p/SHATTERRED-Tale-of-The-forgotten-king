#include "Particle.h"
#include "GameInstance.h"
#include "stdafx.h"
CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CParticle::CParticle(const CParticle& rhs)
	:CBlendObject(rhs)
{
	
}

HRESULT CParticle::Initialize(void* pArg)
{
	__super::Initialize(nullptr);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((PARTICLEDESC*)pArg)->vStartPos));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DesolveTexture"), reinterpret_cast<CComponent**>(&m_pDesolveTexture))))
		return E_FAIL;

	//a
	return S_OK;
}

void CParticle::Set_Target(CGameObject* Target)
{
	if (Target == nullptr)
		m_pTarget = nullptr;
	else
		m_pTarget = (CTransform*)Target->Get_Component(TEXT("Com_Transform"));
}

void CParticle::Set_Rotation(_float Radian, _vector Axis)
{
	m_pTransformCom->Rotation(Axis, XMConvertToRadians(Radian));
}

void CParticle::AdJustLook(_vector vLook)
{
	m_pTransformCom->Set_LookingAt(vLook);
}

void CParticle::Free()
{
	__super::Free();
	Safe_Release(m_pDesolveTexture);
	Safe_Release(m_pShaderCom);
	
}

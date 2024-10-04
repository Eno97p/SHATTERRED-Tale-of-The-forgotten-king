#include "BlackSphere.h"
#include "GameInstance.h"
CBlackSphere::CBlackSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CBlackSphere::CBlackSphere(const CBlackSphere& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CBlackSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackSphere::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_unique<BLACKSPHERE>(*((BLACKSPHERE*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix ParentMat;
	ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_Scale(m_OwnDesc->fMinSize.x, m_OwnDesc->fMinSize.y, m_OwnDesc->fMinSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
	return S_OK;
}

void CBlackSphere::Priority_Tick(_float fTimeDelta)
{

}

void CBlackSphere::Tick(_float fTimeDelta)
{
	if (Erase)
		return;
	m_AccTime += fTimeDelta;

	if (m_AccTime <= m_OwnDesc->fStartdelay && !m_SetDead)			//라이프타임 다하면 삭제
	{
		_float t = m_AccTime / m_OwnDesc->fStartdelay;
		t = max(0.f, min(t, 1.f));
		_vector Minsize = XMLoadFloat3(&m_OwnDesc->fMinSize);
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->fMaxSize);
		_vector ResultSize = XMVectorLerp(Minsize, Maxsize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}

	if (m_SetDead)
	{
		if (m_AccTime > m_DeadDelay)
			Erase = true;

		_float t = m_AccTime / m_DeadDelay;
		t = max(0.f, min(t, 1.f));
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->fMaxSize);
		_vector Minsize = XMVectorZero();
		_vector ResultSize = XMVectorLerp(Maxsize, Minsize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}

}

void CBlackSphere::Late_Tick(_float fTimeDelta)
{
	if (Erase)
		return;
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CBlackSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(25);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

void CBlackSphere::Set_BlackHole_Dead()
{
	m_DeadDelay = m_AccTime + m_OwnDesc->fStartdelay;
	m_SetDead = true;
}


HRESULT CBlackSphere::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Circle"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlackSphere::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}


CBlackSphere* CBlackSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlackSphere* pInstance = new CBlackSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlackSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBlackSphere::Clone(void* pArg)
{
	CBlackSphere* pInstance = new CBlackSphere(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlackSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlackSphere::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

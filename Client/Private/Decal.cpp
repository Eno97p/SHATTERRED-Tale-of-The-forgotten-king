#include "stdafx.h"
#include "..\Public\Decal.h"

#include "GameInstance.h"
#include "ComputeShader_Buffer.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DECAL_DESC* gameobjDesc = (DECAL_DESC*)pArg;
	m_iDecalIdx = gameobjDesc->iDecalIdx;
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&gameobjDesc->mWorldMatrix));

	// 스케일 추출
	_float3 vScaleX = _float3(gameobjDesc->mWorldMatrix._11, gameobjDesc->mWorldMatrix._12, gameobjDesc->mWorldMatrix._13);
	_float3 vScaleY = _float3(gameobjDesc->mWorldMatrix._21, gameobjDesc->mWorldMatrix._22, gameobjDesc->mWorldMatrix._23);
	_float3 vScaleZ = _float3(gameobjDesc->mWorldMatrix._31, gameobjDesc->mWorldMatrix._32, gameobjDesc->mWorldMatrix._33);

	float fScaleX = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vScaleX)));
	float fScaleY = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vScaleY)));
	float fScaleZ = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vScaleZ)));

	m_vPosition = { gameobjDesc->mWorldMatrix._41, gameobjDesc->mWorldMatrix._42, gameobjDesc->mWorldMatrix._43, 1.f };
	m_fRadius = max(fScaleX, max(fScaleY, fScaleZ));

	return S_OK;
}

void CDecal::Priority_Tick(_float fTimeDelta)
{
}

void CDecal::Tick(_float fTimeDelta)
{
}

void CDecal::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_vPosition, m_fRadius))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DECAL, this);
	}
}

pair<_uint, _matrix> CDecal::Render_Decal()
{
	// first : 텍스쳐 번호, second : 월드 역행렬(건들지 말것)
	return pair(m_iDecalIdx, m_pTransformCom->Get_WorldMatrix_Inverse());
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();
}

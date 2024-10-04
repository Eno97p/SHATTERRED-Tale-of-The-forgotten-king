#include "..\Public\Collider.h"

#include "Bounding_AABB.h"
//#include "Bounding_OBB.h"
//#include "Bounding_Sphere.h"

#include "GameInstance.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent{ rhs }
#ifdef _DEBUG	
	, m_pBatch{ rhs.m_pBatch }
	, m_pShader{ rhs.m_pShader }
	, m_pInputLayout{ rhs.m_pInputLayout }
	, m_OutDesc{ rhs.m_OutDesc }
#endif
{
#ifdef _DEBUG	
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pShader = new BasicEffect(m_pDevice);

	m_pShader->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pShader->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout);
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDER_DESC*		pDesc = static_cast<COLLIDER_DESC*>(pArg);

	m_eType = pDesc->eType;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pArg);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pArg);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(pArg);
		break;
	}

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	

	m_pBounding->Tick(WorldMatrix);
}

CCollider::COLLTYPE CCollider::Intersect(CCollider * pTargetCollider)
{
	_bool bCurColl = m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);

	// 충돌O
	if (bCurColl)
	{
		// 닿아있는 상태
		if (m_isColl)
		{
			return COLL_CONTINUE;
		}
		// 처음 닿았을 때
		else
		{
			m_isColl = true;
			return COLL_START;
		}
	}
	// 충돌X
	else
	{
		// 처음 벗어났을 떄
		if (m_isColl)
		{
			m_isColl = false;
			return COLL_FINISH;
		}
		// 닿아있지 않음
		else
		{
			return COLL_NOCOLL;
		}
	}
	return COLL_NOCOLL;
}

void CCollider::Reset()
{
	m_isColl = false;
}

_bool CCollider::IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance)
{
	return m_pBounding->IntersectRay(vRayOrigin, vRayDirection, fDistance);
}

_float3 CCollider::Get_Center()
{
	return m_pBounding->Get_Center();
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (m_OutDesc.bIsOnDebugRender)
	{
		m_pContext->GSSetShader(nullptr, nullptr, 0);
		/*
			m_pContext->OMSetDepthStencilState(nullptr, 0);
			_float		fTemp = {};
			m_pContext->OMSetBlendState(nullptr, &fTemp, 0);
			m_pContext->RSSetState(nullptr);*/

		m_pShader->SetWorld(XMMatrixIdentity());
		m_pShader->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		m_pShader->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));



		m_pContext->IASetInputLayout(m_pInputLayout);

		m_pShader->Apply(m_pContext);

		m_pBatch->Begin();

		m_pBounding->Render(m_pBatch, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

		m_pBatch->End();
	}

	return S_OK;
}

#endif

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{	
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pShader);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);
}

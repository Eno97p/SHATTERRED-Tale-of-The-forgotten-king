#include "..\Public\PipeLine.h"

CPipeLine::CPipeLine()
{

}

const _float4x4 * CPipeLine::Get_Transform_float4x4(D3DTRANSFORMSTATE eState)
{
	return &m_TransformStateMatrix[eState];
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrix[eState]);
}

const _float4x4 * CPipeLine::Get_Transform_float4x4_Inverse(D3DTRANSFORMSTATE eState)
{
	return &m_TransformStateMatrixInverse[eState];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrixInverse[eState]);
}

const _float4 * CPipeLine::Get_CamPosition_float4()
{
	return &m_vCamPosition;	
}

_vector CPipeLine::Get_CamPosition()
{
	return XMLoadFloat4(&m_vCamPosition);
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformStateMatrix[i], XMMatrixIdentity());
		m_TransformStateMatrixInverse[i] = m_TransformStateMatrix[i];
	}

	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformStateMatrixInverse[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(D3DTRANSFORMSTATE(i))));
	}

	memcpy(&m_vCamPosition, &m_TransformStateMatrixInverse[D3DTS_VIEW].m[3], sizeof(_float4));
}

CPipeLine * CPipeLine::Create()
{
	CPipeLine*		pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
}

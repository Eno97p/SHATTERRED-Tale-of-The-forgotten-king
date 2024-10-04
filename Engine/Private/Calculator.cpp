#include "Calculator.h"
#include "Transform.h"
CCalculator::CCalculator()
	
{
}

void CCalculator::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, CTransform* pTransform)
{
	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrixInverse();

	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv));
	XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv));
}

void CCalculator::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse)
{
	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldInverse));
	XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldInverse));
}



HRESULT CCalculator::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND m_hWnd)
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	UINT numViewports = 1;
	pContext->RSGetViewports(&numViewports, &viewport);
	mm_hWnd = m_hWnd;
	return S_OK;
}


void CCalculator::Store_MouseRay(XMMATRIX _mat_projInverse, XMMATRIX _mat_viewInverse)
{
	GetCursorPos(&m_MousePoint);
	ScreenToClient(mm_hWnd, &m_MousePoint);

	_float3 vMousePos;
	vMousePos.x = m_MousePoint.x / (viewport.Width * 0.5) - 1.f;
	vMousePos.y = m_MousePoint.y / -(viewport.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_vector vecMousePos = XMLoadFloat3(&vMousePos);
	vecMousePos = XMVectorSetW(vecMousePos, 1.f);

	vecMousePos = XMVector3TransformCoord(vecMousePos, _mat_projInverse);
	_vector vecRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vecRayDir = vecMousePos - vecRayPos;

	vecRayPos = XMVector3TransformCoord(vecRayPos, _mat_viewInverse);
	vecRayDir = XMVector3TransformNormal(vecRayDir, _mat_viewInverse);
	vecRayDir = XMVector3Normalize(vecRayDir);

	XMStoreFloat3(&m_vRayPos, vecRayPos);
	XMStoreFloat3(&m_vRayDir, vecRayDir);
}


CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND m_hWnd)
{
	CCalculator* pInstance = new CCalculator();

	if (FAILED(pInstance->Initialize(pDevice, pContext, m_hWnd)))
	{
		MSG_BOX("Failed To Created : Calculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCalculator::Free()
{
	
}

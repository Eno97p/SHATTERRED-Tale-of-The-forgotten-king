#pragma once
#include "Base.h"

BEGIN(Engine)
class CPipeLine;
class CTransform;
END

BEGIN(Engine)
class ENGINE_DLL CCalculator final : public CBase
{
private:
	CCalculator();
	virtual ~CCalculator() = default;

public:
	POINT Get_Mouse_Point() { return m_MousePoint; }
	void Store_MouseRay(XMMATRIX _mat_projInverse, XMMATRIX _mat_viewInverse);

public:
	_vector Get_RayPos() { return XMLoadFloat3(&m_vRayPos_Local); }
	_vector Get_RayDir() { return XMLoadFloat3(&m_vRayDir_Local); }
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, CTransform* pTransform);
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse);


private:
	D3D11_VIEWPORT viewport;
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND m_hWnd);

	_float3					m_vRayDir, m_vRayPos;
	_float3					m_vRayDir_Local, m_vRayPos_Local;
	POINT					m_MousePoint;
	HWND					mm_hWnd;
public:
	static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND m_hWnd);
	virtual void Free() override;
};

END
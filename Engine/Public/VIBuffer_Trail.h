#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct TRAILDESC
	{
		_uint			iNumInstance = { 0 };
		_float3			vPivotPos;
		_float3			vSize;
		_float			vSpeed;
		_float			fLifeTime;
		const _float4x4*  ParentMat;
	};


private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

	_bool Check_Instance_Dead() { return m_bInstanceDead; }

public:
	void ExtinctTrail(_float fDelta);
	void EternalTrail(_float fDelta);
	void CatMullRomTrail(_float fDelta);

private:
	XMVECTOR CalculateSplinePoint(float t);
private:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	TRAILDESC					m_TrailDescription;

private:
	_float* m_pSpeeds = { nullptr };
	_float* m_pOriginalSpeeds = { nullptr };
	_float3* m_pSize = { nullptr };
	_float3* m_pOriginalSize = { nullptr };
	_float3 m_pPivotPos = {0.f,0.f,0.f};

	_bool						m_bInstanceDead = false;

	deque<XMVECTOR>				m_bSpline;

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
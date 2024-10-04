#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_SwordTrail final : public CVIBuffer
{
public:
	typedef struct SwordTrailDesc
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float			vSize;
		_float			fLifeTime;
		_float			fMaxTime;
		const _float4x4* ParentMat = nullptr;
	};
private:
	CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs);
	virtual ~CVIBuffer_SwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
	_bool isDead() { return m_bInstanceDead; }

public:
	void Tick(_float fDelta);
	void Tick_AI(_float fDelta);
	void Tick_AI_UPgrade(_float fDelta);
private:
	XMVECTOR CatmullRom(XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR v3, float t);

private:
	ID3D11Buffer*				 m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

private:
	deque<pair<XMVECTOR, XMVECTOR>>		m_pTrailVertex;

	shared_ptr<SwordTrailDesc>	m_pDesc;
	vector<XMFLOAT3> m_TopPoints;
	vector<XMFLOAT3> m_BottomPoints;

	_bool m_bInstanceDead = false;
public:
	static CVIBuffer_SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
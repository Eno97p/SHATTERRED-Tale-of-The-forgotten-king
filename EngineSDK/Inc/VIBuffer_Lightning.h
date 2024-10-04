#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Lightning final : public CVIBuffer
{
public:
	typedef struct LIGHTNINGDESC
	{
		_uint			iNumInstance = { 0 };
		XMFLOAT3		vStartpos = {0.f,0.f,0.f};
		XMFLOAT3		vEndpos = {0.f,0.f,0.f};
		_float2			fThickness = {0.f,0.f};
		_float2			fLifeTime = {0.f,0.f};
		_int			NumSegments[2] = { 0,0 };
		_float2			Amplitude = { 0.f,0.f };
	};

private:
	CVIBuffer_Lightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Lightning(const CVIBuffer_Lightning& rhs);
	virtual ~CVIBuffer_Lightning() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

	void Tick(_float fTimeDelta);
	_bool isDead() { return m_bInstanceDead; }

private:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

private:
	shared_ptr<LIGHTNINGDESC>	m_pDesc;
	_bool m_bInstanceDead = false;
public:
	static CVIBuffer_Lightning* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
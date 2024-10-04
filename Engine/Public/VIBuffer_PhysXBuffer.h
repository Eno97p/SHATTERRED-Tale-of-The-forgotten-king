#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PhysXBuffer final : public CVIBuffer
{
private:
	CVIBuffer_PhysXBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_PhysXBuffer(const CVIBuffer_PhysXBuffer& rhs);
	virtual ~CVIBuffer_PhysXBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype(istream& is);
	virtual HRESULT Initialize_Prototype(const PxRenderBuffer& rRenderBuffer);
	virtual HRESULT Initialize_Prototype(const vector<_float3> vecVertices);
	virtual HRESULT Initialize_Prototype(const vector<_float3>& vecVertices, const std::vector<_uint>& vecIndices);
	virtual HRESULT Initialize(void* pArg) override;


public:
	_uint Get_NumVertex() { return m_iNumVertices; }
	VTXPHYSX* Get_Vertices() { return m_pVertices; }
	_uint Get_NumIndices() { return m_iNumIndices; }
	_uint* Get_Indices() { return m_pIndices; }




private:
	HRESULT Load_Buffer(istream& is);

	//정점 정보들
private:
	VTXPHYSX* m_pVertices = nullptr;
	_uint* m_pIndices = nullptr;


	string m_strFilePath;
public:
	static CVIBuffer_PhysXBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PxRenderBuffer& rRenderBuffer);
	static CVIBuffer_PhysXBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_float3> vecVertices);
	static CVIBuffer_PhysXBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& is);
	static CVIBuffer_PhysXBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_float3>& vecVertices, const std::vector<_uint>& vecIndices);


	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
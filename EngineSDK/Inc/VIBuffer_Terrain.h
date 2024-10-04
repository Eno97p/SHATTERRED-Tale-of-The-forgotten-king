#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath, _bool realmap);
	virtual HRESULT Initialize(void* pArg) override;
	virtual _float Compute_Height(const _float3& vLocalPos) override;
	_float3 Compute_Normal(const _float3& vWorldPos);

public:
	void Culling(_fmatrix WorldMatrixInv);

	void Update_Height(float fDeltaTime);


	void AdjustHeight(const _float4& vBrushPos, float fBrushSize, float fBrushStrength, float fMaxHeight, bool bRaise);

	HRESULT UpdateHeightMap(ID3D11Texture2D* pHeightMapTexture);

	HRESULT UpdateVertexBuffer(_ushort* pHeightMapData, int iWidth, int iHeight, float terrainScale);

	//HRESULT UpdateVertexBuffer(BYTE* pHeightMapData, int iWidth, int iHeight, float terrainScale);


	void SplatBrushOnHeightMap(ID3D11Texture2D* pHeightMapTexture, const _float4& vBrushPos, float fBrushSize, float fBrushStrength, CTexture* pBrushTexture);

	void Recalculate_Normals();

	//void Update_Height(const _float4& vBrushPos, float fBrushSize, float fBrushStrength, CTexture* pBrushTexture);

	_uint GetNumVerticesX() { return m_iNumVerticesX; }
	_uint GetNumVerticesZ() { return m_iNumVerticesZ; }

public:
	ID3D11Buffer* Get_VertexBuffer() { return m_pVB; }
	_float4* Get_VertexPositions() { return m_pVertexPositions; }

public:
	const vector<_float3>& GetValidGrassPositions() const { return m_validGrassPositions; }

private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };

	class CQuadTree*	m_pQuadTree = { nullptr };
	class COctTree*		m_pOctTree = { nullptr };
	PxActor* m_pTerrainActor = { nullptr };
private:
	vector<_float3> m_validGrassPositions;
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath, _bool realmap);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
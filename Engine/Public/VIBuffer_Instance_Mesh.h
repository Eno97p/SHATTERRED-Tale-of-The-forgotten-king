#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CInstance_Mesh final : public CVIBuffer_Instance
{

private:
	CInstance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Mesh(const CInstance_Mesh& rhs);
	virtual ~CInstance_Mesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	HRESULT Initialize_ProtoType(class CMesh* m_Meshes, const INSTANCE_DESC& InstanceDesc);
	HRESULT Initialize_ProtoType_ForMapElements(class CMesh* m_Meshes, const INSTANCE_MAP_DESC& InstanceDesc);
	HRESULT Initialize(void* pArg);

	_uint* Get_Mesh_Indices() { return pNewIndices; }

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	vector<_float4x4>	m_OffsetMatrices;
	D3D11_BUFFER_DESC	m_MeshBufferdesc = {};
	VTXMESH* m_nonAnimpVertices;
	_uint* pNewIndices;



public:
	static CInstance_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CMesh* m_Meshes, const INSTANCE_DESC& InstanceDesc);
	static CInstance_Mesh* Create_ForMapElements(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CMesh* m_Meshes, const INSTANCE_MAP_DESC& InstanceDesc);

	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
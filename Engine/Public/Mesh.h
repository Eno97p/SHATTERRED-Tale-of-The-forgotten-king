#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	_uint Get_NumVertex() { return m_iNumVertices; }
	VTXMESH* Get_Vertices() { return m_nonAnimpVertices; }
	_uint Get_NumIndices() { return m_iNumIndices; }

public:
	HRESULT Initialize(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	void Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices);
	_uint* Get_Mesh_Indices() { return pNewIndices; }


private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 }; /* 이 메시에 영향을 주는 뼈의 갯수 */
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;
	D3D11_BUFFER_DESC	m_MeshBufferdesc = {};
	VTXMESH* m_nonAnimpVertices;
	VTXANIMMESH* m_AnimVertices;
	_uint* pNewIndices;
	CModel::MODELTYPE	m_ModelType;

private:
	HRESULT Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);

public:
	HRESULT Save_Mesh(ostream& os);
	HRESULT Load_Mesh(istream& is);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones);
	static CMesh* LoadCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& is);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
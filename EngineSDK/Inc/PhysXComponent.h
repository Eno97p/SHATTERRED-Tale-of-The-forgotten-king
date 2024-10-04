#pragma once

#include "Component.h"
#include"VehicleDefault.h"



BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent : public CComponent
{
public:
	typedef struct  PHYSX_DESC
	{

		_float4x4 fWorldMatrix = {};
		_float4x4 fOffsetMatrix = {};
		_float3 fMatterial = { 0.f, 0.f, 0.f };
		_float3 fBoxProperty = { 0.f, 0.f, 0.f };
		_float2 fCapsuleProperty = { 0.f, 0.f };
		_float fMass = 0.0f;
		PxFilterData filterData = {};
		PxGeometryType::Enum eGeometryType = PxGeometryType::eINVALID;
		CComponent* pComponent = nullptr;
		CMesh* pMesh = nullptr;
		const char* pName = nullptr;

	}PHYSX_DESC;

#ifdef _DEBUG
	typedef struct  PhysX_Editable_Desc : CComponent::ComponentDesc
	{
		//수정 가능하게 할 항목만 넣기
		PxMaterial* pMaterial = nullptr;
		_bool bIsOnDebugRender = true;

	}PhysX_Editable_Desc;

#endif // _DEBUG

	

protected:
	CPhysXComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXComponent(const CPhysXComponent& rhs);
	virtual ~CPhysXComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;


#ifdef _DEBUG
	virtual HRESULT  Init_Buffer();
	virtual HRESULT Render();
	virtual PhysX_Editable_Desc* GetData() override { return &m_OutDesc; }
#endif

public:
	virtual void Tick(const _float4x4* pWorldMatrix);
	virtual void Late_Tick(_float4x4* pWorldMatrix);
	virtual void SetFilterData(PxFilterData  filterData);

protected:
	virtual tuple<vector<PxVec3>, vector<PxU32>> CreateTriangleMeshDesc(void* pvoid);

public:
	vector<_float3> CreateCapsuleVertices(float radius, float halfHeight, int segments = 32, int rings = 32);
	tuple<vector<_float3>, vector<_uint>> CreateBoxVertices(const PxVec3& halfExtents);
	HRESULT CreateActor(PxGeometryType::Enum eGeometryType, const PxTransform& pxTrans, const PxTransform& pxOffsetTrans = PxTransform());
	PxActor* Get_Actor() { return m_pActor; }
	_bool RayCast(_vector vYourPos, _vector vYourDir, PxReal distance,const PxQueryFilterData& filterData = PxQueryFilterData(), PxQueryFilterCallback* filterCall = NULL);
private:
	void MakeFilterData(PxFilterData& filterData);











protected:
#ifdef _DEBUG
	class CShader*								m_pShader = { nullptr };	
#endif
	vector <class CVIBuffer_PhysXBuffer*>		m_pBuffer = {};
	PxMaterial* m_pMaterial = { nullptr };
	PxRigidActor* m_pActor = { nullptr };
	PxShape* m_pShape = { nullptr };
	_float4x4 m_WorldMatrix = {};
private:
	class CModel* m_pModelCom = { nullptr };
	class CMesh* m_pMeshCom = { nullptr };
	vector<_float3> m_vecVertices;
	_float m_Mass = 0.0f;

	_float3 m_fBoxProperty = { 0.f, 0.f, 0.f };
	_float2 m_fCapsuleProperty = { 0.f, 0.f };
	_float3 m_fScale = { 1.f, 1.f, 1.f };
#ifdef _DEBUG

	PhysX_Editable_Desc m_OutDesc;
#endif // _DEBUG

public:
	static CPhysXComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();



public:
	static PxTransform Convert_DxMat_To_PxTrans(const _float4x4& pWorldMatrix, _float3* fSclae =nullptr);
	static XMMATRIX Convert_PxTrans_To_DxMat(const PxTransform& pTrans);
	static PxVec4 Convert_XMVec4_To_PxVec4(const _vector pXMVec4);
	static XMVECTOR Convert_PxVec4_To_XMVec4(const PxVec4& pPxVec4);
};

END
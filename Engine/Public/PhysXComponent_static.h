#pragma once

#include "PhysXComponent.h"




//해당 클래스는 static 오브젝트에 대한 사전 쿠킹을 위한 클래스

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_static final : public CPhysXComponent
{
public:
#ifdef _DEBUG
	typedef struct  PhysX_static_Editable_Desc : CPhysXComponent::PhysX_Editable_Desc
	{
		//수정 가능하게 할 항목만 넣기
		int i = 20;


	}PhysX_static_Editable_Desc;
#endif // _DEBUG

	
private:
	explicit CPhysXComponent_static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_static(const CPhysXComponent_static& rhs);
	virtual ~CPhysXComponent_static() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, const wstring& FilePath);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual PhysX_static_Editable_Desc* GetData() override { return &m_OutDesc; }
	virtual HRESULT Render()override;
#endif



private:
	HRESULT Create_PhysX_TriAngleMesh();
	HRESULT Load_Buffer();



private:
	virtual tuple<vector<PxVec3>, vector<PxU32>> CreateTriangleMeshDesc(void* pvoid) override;
private:
	string m_strFilePath;
	vector<PxTriangleMesh*> m_pTriangleMesh;
#ifdef _DEBUG
	PhysX_static_Editable_Desc m_OutDesc;
#endif
public:
	static CPhysXComponent_static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const wstring& FilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END
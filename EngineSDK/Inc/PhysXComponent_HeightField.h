#pragma once

#include "PhysXComponent.h"




//해당 클래스는 static 오브젝트에 대한 사전 쿠킹을 위한 클래스

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_HeightField final : public CPhysXComponent
{
public:
#ifdef _DEBUG
	typedef struct  PhysX_HeightFiel_Editable_Desc : CPhysXComponent::PhysX_Editable_Desc
	{
	
	}PhysX_HeightFiel_Editable_Desc;
#endif // _DEBUG

	
private:
	explicit CPhysXComponent_HeightField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_HeightField(const CPhysXComponent_HeightField& rhs);
	virtual ~CPhysXComponent_HeightField() = default;

public:
	HRESULT Initialize_Prototype(const wstring& strPrototypeTag);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual PhysX_HeightFiel_Editable_Desc* GetData() override { return &m_OutDesc; }
	virtual HRESULT Render()override;
#endif

private:
	HRESULT Create_HeightField();



#ifdef _DEBUG
private:
	PhysX_HeightFiel_Editable_Desc m_OutDesc;
#endif
	PxReal m_fMinHeight = PX_MAX_F32;
	PxReal m_fMaxHeight = -PX_MAX_F32;
	PxHeightField* m_pHeightField = nullptr;
	_float m_fHeightScale = 0.0f;

	_uint m_iNumVerticeX = 0;
	_uint m_iNumVerticeZ = 0;
public:
	static CPhysXComponent_HeightField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END
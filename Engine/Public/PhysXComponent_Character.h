#pragma once

#include "PhysXComponent.h"
#include "CCCTFilterCallBack.h"



BEGIN(Engine)
class CGameObject;
class CTransform;
class CHitReport;

class ENGINE_DLL CPhysXComponent_Character final : public CPhysXComponent
{
public:
	typedef	struct ControllerDesc :public PxCapsuleControllerDesc
	{
		CCCTFilterCallBack::FilterCallBack pFilterCallBack = nullptr;
		PxFilterData filterData = {};
		_float3 fMatterial;
		_float fJumpSpeed = 0.f;
		const char* pName = nullptr;
		CGameObject* pGameObject = nullptr;
		CTransform* pTransform = nullptr;
	}ControllerDesc;


#ifdef _DEBUG

public:
	typedef struct  PhysX_Character_Editable_Desc : CPhysXComponent::PhysX_Editable_Desc
	{
		_float3 fPosition = { 0.f, 0.f, 0.f };
		PxController* pController = nullptr;
		//수정 가능하게 할 항목만 넣기


	}PhysX_Character_Editable_Desc;
#endif // _DEBUG


private:
	explicit CPhysXComponent_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_Character(const CPhysXComponent_Character& rhs);
	virtual ~CPhysXComponent_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


public:
	virtual void SetFilterData(PxFilterData  filterData)override;


#ifdef _DEBUG
	virtual HRESULT  Init_Buffer() override;
	virtual HRESULT Render() override;
	virtual PhysX_Character_Editable_Desc* GetData() override { return &m_OutDesc; }
#endif

public:
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

	HRESULT Go_Up(_float fTimeDelta);
	HRESULT Go_Straight(_float fTimeDelta);
	HRESULT Go_BackWard(_float fTimeDelta);
	HRESULT Go_Left(_float fTimeDelta);
	HRESULT Go_Right(_float fTimeDelta);
	HRESULT Go_LeftFront(_float fTimeDelta);
	HRESULT Go_RightFront(_float fTimeDelta);
	HRESULT Go_OrbitCW(_float fTimeDelta, CTransform* pTargetTransform);
	HRESULT Go_OrbitCCW(_float fTimeDelta, CTransform* pTargetTransform);
	HRESULT Go_Jump(_float fTimeDelta, _float fJumpSpeed = 0.f);
	void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	void Set_Position(_vector vPos);
	void Speed_Scaling(_float fScale) { m_fSpeed *= fScale; }
	void Set_JumpSpeed(_float fSpeed) { m_fJumpSpeed = fSpeed; }
	_bool Get_IsJump() { return m_bIsJump; }
	_float Get_CurrentJumpSpeed() { return m_fCurrentY_Velocity; }
	_float Get_LengthFromGround();
	void Set_Gravity(_bool bGravity = true) { m_bGravity = bGravity; }
	_bool Get_Gravity() { return m_bGravity; }


	PxController* Get_Controller() { return m_pController; }

private:
	//해제할 필요 없음 컨트롤러 매니저가 한 번에 지워줌
	PxController* m_pController = { nullptr };
	CHitReport* m_pHitReport = { nullptr };
	CTransform* m_pTransform = { nullptr };
	CCCTFilterCallBack* m_pCCTFilerCallBack = { nullptr };

	PxVec3 m_vGravity = { 0.f, 0.0f, 0.f };
	_bool m_bIsJump = false;
	_float m_fJumpSpeed = 0.f;
	_float m_fCurrentY_Velocity = 0.f;
	const _float m_fGravity = -30.f;
	_bool m_bGravity = true;
	_float m_fSpeed = 3.f;
#ifdef _DEBUG
	PhysX_Character_Editable_Desc m_OutDesc;
#endif // _DEBUG
	
public:
	static CPhysXComponent_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END
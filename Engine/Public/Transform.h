#pragma once

#include "Component.h"

/* 객체의 월드 변환 행렬을 보관한다. */
/* 월드 변환 상태를 제어하기위한 기능. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
private:
	enum MATRIXTYPE { MAT_VIEW, MAT_PROJ, MAT_VIEWINV, MAT_PROJINV , MATTYPE_END};
public:
	typedef struct TRANSFORM_DESC
	{
		_float4x4 mWorldMatrix = 
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		_float		fInitialDegree = 0.f;
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

	typedef struct Transform_Editable_Desc : CComponent::ComponentDesc
	{
		//수정 가능하게 할 항목만 넣기
		_float4x4* pWorldMatrix = nullptr;

	}Transform_Editable_Desc;

	virtual Transform_Editable_Desc*  GetData() override { return &m_OutDesc; }

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrixInverse() { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }

	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() {
		return _float3(XMVector3Length(Get_State(STATE_RIGHT)).m128_f32[0], 
			XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
			XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]);
	}

	const _float4x4* Get_WorldFloat4x4() {
		return &m_WorldMatrix;
	}
	_float4x4* Get_WorldFloat4x4Ref() {
		return &m_WorldMatrix;
	}
	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_WorldMatrix(const _matrix matWorld) { XMStoreFloat4x4(&m_WorldMatrix, matWorld); }

public:
	void Set_State(STATE eState,_fvector vState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	


public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);
	HRESULT Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	HRESULT Go_Backward(_float fTimeDelta);
	HRESULT Go_Up(_float fTimeDelta);
	HRESULT Go_Left(_float fTimeDelta);
	HRESULT Go_Right(_float fTimeDelta);
	HRESULT LookAt(_fvector vTargetPosition);
	HRESULT LookAt_For_LandObject(_fvector vTargetPosition);
	HRESULT Turn(_fvector vAxis, _float fTimeDelta);
	HRESULT Rotation(_fvector vAxis, _float fRadian);
	void TurnToTarget(_float fTimeDelta, _fvector vTargetPosition);
	void Speed_Scaling(_float fRatio) { m_fSpeedPerSec *= fRatio; }
	void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fSpeed; }
	void BillBoard();			//빌보드 함수 추가 - 박은호
	void BillBoard_X();
	void BillBoard_Y();		
	void BillBoard_Z();
	void Set_LookingAt(_fvector vLook);

public:
	HRESULT Floating_Y(_float fTimeDelta, _float fFloatSpeed, _float fFloatRange, _vector vOriginPos); //무중력 떠다니기

private:
	//void Update_WorldMatrix();


private:
	/* row major */
	_float4x4				m_WorldMatrix;

	const XMFLOAT4X4*				m_Matrix[MATTYPE_END];		//뷰,투영,뷰행렬역행렬,투영행렬역행렬 들어있음
	//_float4x4* m_ViewInverse = nullptr;
	//_float4x4* m_ProjInverse = nullptr;
	//_float4x4* m_Proj = nullptr;
	//_float4x4* m_View = nullptr;



	_float4x4               m_QuternionMatrix;
	_float					m_fSpeedPerSec = { 0.0f };
	_float					m_fRotationPerSec = { 0.0f };

	Transform_Editable_Desc			m_OutDesc;

	_float					m_fFloatTime = 0.f;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
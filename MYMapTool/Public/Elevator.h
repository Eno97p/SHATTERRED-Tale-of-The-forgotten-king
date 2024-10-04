#pragma once

//#include "MYMapTool_Defines.h"
#include "ToolObj.h"


BEGIN(MYMapTool)

class CElevator final : public CToolObj
{
public:
	enum ELEVATOR_STATE { ELEVATOR_IDLE, ELEVATOR_DESCEND, ELEVATOR_ASCEND, ELEVATOR_REBOUND, ELEVATOR_END };
private:
	CElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElevator(const CElevator& rhs);
	virtual ~CElevator() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

public:
	void Ascend(_vector vTargetPos) { m_eElevatorState = ELEVATOR_ASCEND; m_vTargetPos = vTargetPos; }
	void Descend(_vector vTargetPos) { m_eElevatorState = ELEVATOR_DESCEND; m_vTargetPos = vTargetPos; }
	void Idle(_vector vTargetPos) { m_eElevatorState = ELEVATOR_IDLE; m_vTargetPos = {0.f, 0.f, 0.f, 1.f}; }

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fReboundTimer = 0.f;

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	//	_bool m_bElevate = false;
	ELEVATOR_STATE m_eElevatorState = ELEVATOR_END;
	_float m_fElevateTimer= 0.f;
	_uint m_iTest = 0;


	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CElevator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
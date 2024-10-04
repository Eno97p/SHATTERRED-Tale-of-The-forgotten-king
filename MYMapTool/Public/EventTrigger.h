#pragma once

#include "ToolObj.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
class CCollider;
END

BEGIN(MYMapTool)

class CEventTrigger final : public CToolObj
{
public:
	enum TRIGGER_TYPE {
		TRIG_TUTORIAL_BOSSENCOUNTER,
		TRIG_JUGGLAS_SPAWNSECONDROOM, TRIG_JUGGLAS_SPAWNTHIRDROOM, TRIG_VIEWCHANGE_TTOS, TRIG_VIEWCHANGE_STOT, TRIG_ASCEND_ELEVATOR, TRIG_DESCEND_ELEVATOR,
		TRIG_SCENE_CHANGE, TRIG_VIEWCHANGE_TTOBS, TRIG_SCENE_CHANGE_FOR_ACKBAR, TRIG_SCENE_CHANGE_FOR_JUGGLAS, TRIG_SCENE_CHANGE_FOR_ANDRASARENA,
		 TRIG_CUTSCENE_GRASSLAND, TRIG_CUTSCENE_MALKHEL, TRIG_CUTSCENE_JUGGULUS, TRIG_CUTSCENE_ANDRAS,
		TRIG_END
	};
private:
	CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventTrigger(const CEventTrigger& rhs);
	virtual ~CEventTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };

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
	TRIGGER_TYPE m_eTRIGState = TRIG_END;

private:
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f;

	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CEventTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
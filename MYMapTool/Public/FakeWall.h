#pragma once

#include "ToolObj.h"
//#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(MYMapTool)

class CFakeWall final : public CToolObj
{
public:
	enum DISSOLVE_STATE { DS_FADEIN, DS_FADEOUT, DS_END };
	/*typedef struct TutorialMapBridge_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4			vStartPos;
		_float3			vTopCol = { 1.f, 0.f, 0.f };
		_float3			vBotCol = {0.f, 0.f, 0.f};
	}TutorialMapBridge_DESC;*/
private:
	CFakeWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFakeWall(const CFakeWall& rhs);
	virtual ~CFakeWall() = default;

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
	DISSOLVE_STATE eDSState = DS_END;

private:
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f;

	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CFakeWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Active_Element.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(Client)

class CTutorialMapBridge final : public CActive_Element
{
public:
	enum DISSOLVE_STATE {DS_FADEIN, DS_FADEOUT, DS_END};
	/*typedef struct TutorialMapBridge_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4			vStartPos;
		_float3			vTopCol = { 1.f, 0.f, 0.f };
		_float3			vBotCol = {0.f, 0.f, 0.f};
	}TutorialMapBridge_DESC;*/
private:
	CTutorialMapBridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorialMapBridge(const CTutorialMapBridge& rhs);
	virtual ~CTutorialMapBridge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pNoiseCom = { nullptr };

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
	DISSOLVE_STATE m_eDSState = DS_END;
	_bool			m_bPhysxOff = false;
	CPhysXComponent* m_pPhysXCom = { nullptr };

private:
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f; 

	_float3 m_vPivotPos = {0.f, 0.f, 0.f};
	_float4x4 m_World;

public:
	static CTutorialMapBridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
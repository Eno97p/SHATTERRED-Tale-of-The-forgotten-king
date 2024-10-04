#pragma once

//#include "MYMapTool_Defines.h"
#include "ToolObj.h"


BEGIN(MYMapTool)

class CLagoon final : public CToolObj
{
private:
	enum TEXS { TEX_NORMAL1, TEX_NORMAL2, TEX_CAUSTIC, TEX_FOAMMASK, TEX_FOAM, TEX_FOAMNORMAL, TEX_END };
private:
	CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLagoon(const CLagoon& rhs);
	virtual ~CLagoon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Mirror() override;

public:
	_float4 m_vLightPosition = { 0.f, 100.f, 0.f, 1.f };
	_float m_fLightRange = 500.f; // 포인트 라이트의 범위

	_float4 m_vLightDir = { -1.f, -1.f, -1.f, 0.f };
	_float4 m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vLightAmbient = { 0.4f, 0.4f, 0.4f, 1.f };
	_float4 m_vLightSpecular = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

	_float m_fFlowSpeed = 0.1f;
	_float m_fFresnelStrength = 0.5f;
	_float3 m_vSkyColor = { 0.7f, 0.9f, 1.0f };
	_bool m_bDiffuse = true;
	_bool m_bNormal = true;
	_bool m_bSpecular = true;
	_float m_fRoughness = 0.01f;
	_float m_fNormalStrength0 = 0.33f; // 첫 번째 노멀 맵의 강도
	_float m_fNormalStrength1 = 0.33f; // 두 번째 노멀 맵의 강도
	_float m_fNormalStrength2 = 0.33f; // 세 번째 노멀 맵의 강도
	_float m_fCausticStrength = 0.3f; // Caustic 노이즈의 강도

	_float m_fWaterAlpha = 0.8f;    // 물의 기본 알파값
	_float m_fWaterDepth = 2.0f;    // 물의 깊이 (미터 단위)


	_float m_fBloomIntensity = 1.f;    // 물의 기본 알파값
	_float m_fBloomThreshold = 0.8f;    // 물의 깊이 (미터 단위)

    _float m_fFoamWaveFrequency = 1.0f;		// 거품 파도의 주파수
    _float m_fFoamWaveAmplitude = 0.05f;	// 거품 파도의 진폭
    _float m_fFoamMaskScale = 10.0f;		// 거품 마스크의 스케일
    _float m_fFoamMaskSpeed = 0.01f;		// 거품 마스크의 이동 속도
    _float m_fFoamBlendStrength = 0.5f;		// 거품 블렌딩 강도
    _float m_fFoamFresnelStrength = 0.5f;	// 거품에 적용되는 프레넬 효과 강도private:
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CTexture* m_pTexture[TEX_END] = {nullptr};
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fAccTime = 0.f;

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
	_float m_fElevateTimer= 0.f;
	_uint m_iTest = 0;


	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CLagoon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
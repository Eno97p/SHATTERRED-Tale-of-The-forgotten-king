#pragma once

#include "Map_Element.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CLagoon final : public CMap_Element
{
public:
	typedef struct LAGOON_DESC
	{
		_float4 vLightPosition = { -389.486f, 485.690f, 35.512f, 1.f };
		_float fLightRange = 1000.f; 

		_float4 vLightDir = { -1.f, -1.f, -1.f, 0.f };
		_float4 vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
		_float4 vLightAmbient = { 0.574f, 0.812f, 1.f, 1.f };
		_float4 vLightSpecular = { 1.f, 1.f, 1.f, 1.f };
		_float4 vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

		_float fBloomIntensity = 0.65f;    
		_float fBloomThreshold = 0.524752f;  

		_float fFlowSpeed = 0.3f;
		_float fFresnelStrength = 0.399f;
		_float3 vSkyColor = { 0.7f, 0.9f, 1.0f };
		_float fRoughness = 0.22f;
		_float fNormalStrength0 = 0.663f; 
		_float fNormalStrength1 = 0.756f; 
		_float fNormalStrength2 = 0.330f; 
		_float fCausticStrength = 1.f; 

		_float fWaterAlpha = 0.95f;   
		_float fWaterDepth = 2.0f;    
		
	}LAGOON_DESC;

private:
	enum TEXS { TEX_NORMAL1, TEX_NORMAL2, TEX_CAUSTIC, TEX_FOAMMASK, TEX_FOAM, TEX_FOAMNORMAL,TEX_END };
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
	void CLagoon::Set_LagoonDesc(LAGOON_DESC desc)
	{
		m_vLightPosition = desc.vLightPosition;
		m_fLightRange = desc.fLightRange;
		m_vLightDir = desc.vLightDir;
		m_vLightDiffuse = desc.vLightDiffuse;
		m_vLightAmbient = desc.vLightAmbient;
		m_vLightSpecular = desc.vLightSpecular;
		m_vMtrlSpecular = desc.vMtrlSpecular;
		m_fBloomIntensity = desc.fBloomIntensity;
		m_fBloomThreshold = desc.fBloomThreshold;
		m_fFlowSpeed = desc.fFlowSpeed;
		m_fFresnelStrength = desc.fFresnelStrength;
		m_vSkyColor = desc.vSkyColor;
		m_fRoughness = desc.fRoughness;
		m_fNormalStrength0 = desc.fNormalStrength0;
		m_fNormalStrength1 = desc.fNormalStrength1;
		m_fNormalStrength2 = desc.fNormalStrength2;
		m_fCausticStrength = desc.fCausticStrength;
		m_fWaterAlpha = desc.fWaterAlpha;
		m_fWaterDepth = desc.fWaterDepth;

		// 필요한 경우 추가적인 처리를 여기에 구현할 수 있습니다.
		// 예: 셰이더 상수 버퍼 업데이트
		// UpdateShaderVariables();
	}

private:
	_float4 m_vLightPosition = { -389.486f, 485.690f, 35.512f, 1.f };
	_float m_fLightRange = 1000.f; // 포인트 라이트의 범위

	_float4 m_vLightDir = { -1.f, -1.f, -1.f, 0.f };
	_float4 m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vLightAmbient = { 0.574f, 0.812f, 1.f, 1.f };
	_float4 m_vLightSpecular = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

	_float m_fBloomIntensity = 0.65f;    // 물의 기본 알파값
	_float m_fBloomThreshold = 0.524752f;    // 물의 깊이 (미터 단위)

	_float m_fFlowSpeed = 0.3f;
	_float m_fFresnelStrength = 0.399f;
	_float3 m_vSkyColor = { 0.7f, 0.9f, 1.0f };
	_bool m_bDiffuse = true;
	_bool m_bNormal = true;
	_bool m_bSpecular = true;
	_float m_fRoughness = 0.27f;
	_float m_fNormalStrength0 = 0.663f; // 첫 번째 노멀 맵의 강도
	_float m_fNormalStrength1 = 0.756f; // 두 번째 노멀 맵의 강도
	_float m_fNormalStrength2 = 0.330f; // 세 번째 노멀 맵의 강도
	_float m_fCausticStrength = 1.f; // Caustic 노이즈의 강도

	_float m_fWaterAlpha = 0.6f;    // 물의 기본 알파값
	_float m_fWaterDepth = 3.7f;    // 물의 깊이 (미터 단위)



	_float m_fFoamWaveFrequency = 0.85f;		// 거품 파도의 주파수
	_float m_fFoamWaveAmplitude = 0.01f;	// 거품 파도의 진폭
	_float m_fFoamMaskScale = 0.215686f;		// 거품 마스크의 스케일
	_float m_fFoamMaskSpeed = 0.01f;		// 거품 마스크의 이동 속도
	_float m_fFoamBlendStrength = 0.2f;		// 거품 블렌딩 강도
	_float m_fFoamFresnelStrength = 0.937f;	// 거품에 적용되는 프레넬 효과 강도private:


private:
	CTexture* m_pNoiseCom = { nullptr };
	CTexture* m_pTexture[TEX_END] = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fAccTime = 0.f;


	PxRigidStatic* m_pgroundPlane = nullptr;
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
	_float m_fElevateTimer = 0.f;
	_uint m_iTest = 0;


	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CLagoon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
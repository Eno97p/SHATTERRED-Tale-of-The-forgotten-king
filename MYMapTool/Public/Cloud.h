#pragma once

#include "ToolObj.h"

#include "../Texture3D.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTexture3D;
class CVIBuffer_Instance_Point;
class CCollider;
END

BEGIN(MYMapTool)

class CCloud final : public CToolObj
{
public:
private:
	CCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCloud(const CCloud& rhs);
	virtual ~CCloud() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection() override;


public:
	_uint Get_ShaderPath() { return m_iShaderPath; }
	void Set_ShaderPath(_uint iIdx) {  m_iShaderPath = iIdx; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTexture3D* m_pNoise3DCom = { nullptr };

	CPhysXComponent_static* m_pPhysXCom = { nullptr };
	CComputeShader_Texture* m_pCloudCS = { nullptr };
	ID3D11Texture3D* m_pNoiseTexture = nullptr;
	ID3D11ShaderResourceView* m_pNoiseSRV = nullptr;
	ID3D11Texture3D* m_pDensityTexture = nullptr;
	ID3D11ShaderResourceView* m_pDensitySRV = nullptr;
private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f;

	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

	_uint m_iShaderPath = 0;
public:
	// 구름 관련 변수들
	float m_fCloudDensity = 1.007605f;
	float m_fCloudScale = 0.000381f;
	float m_fCloudSpeed = 0.017529f;
	float m_fCloudHeight = 99.049f;
	_float3 m_vCloudColor = _float3(0.6f, 0.6f, 0.6f);

	// 조명 관련 변수
	_float4 m_vLightDiffuse = _float4(0.76f, 0.8f, 0.95f, 1.0f);
	_float4 m_vLightAmbient = _float4(0.2f, 0.2f, 0.3f, 1.0f);
	_float4 m_vLightSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	_float4 m_vLightDir = _float4(-1.0f, -1.0f, -1.0f, 0.0f);
	_float4 m_vLightPosition = {-761.797f, 3494.479f, 856.610, 1.f };
	float m_fLightRange = 5000.0f;

	// Sphere Tracing 관련 변수
	float m_fSphereTracingThreshold = 0.01f;
	float m_fMaxRayDistance = 1000.0f;
	int m_iMaxSteps = 128;

	// 레이마칭 관련 변수
	float m_fStepSize = 0.5f;

	// 노이즈 관련 변수
	int m_iPerlinOctaves = 2;
	float m_fPerlinFrequency = 0.630f;
	float m_fWorleyFrequency = 1.0f;

	// Perlin Noise 관련 새 변수들
	float m_fPerlinPersistence = 0.381f;
	float m_fPerlinLacunarity = 1.0f;

	// Worley Noise 관련 새 변수
	float m_fWorleyJitter = 0.0f;

	// Noise 혼합 관련 새 변수들
	float m_fPerlinWorleyMix = 0.186f;
	float m_fNoiseRemapLower = 0.650f;
	float m_fNoiseRemapUpper = 1.000f;

	//Reflection 최적화
	_float m_fReflectionQuality = 0.5f; // 0.1 (낮음) ~ 1.0 (높음)
	_float m_fReflectionOpacity = 0.5f; // 0.0 (완전 투명) ~ 1.0 (불투명)
	_float m_fReflectionDensityScale = 0.5f; // 0.1 (엷음) ~ 1.0 (진함)
	_float4 m_vBaseSkyColor = { 0.6f, 1.f, 1.f, 1.f };
public:
	float m_fCoarseStepSize = 10.0f;
	float m_fFineStepSize = 1.f;
	int m_iMaxCoarseSteps = 10;
	int m_iMaxFineSteps = 64;
	float m_fDensityThreshold = 0.1f;
	float m_fAlphaThreshold = 0.944f;


	void Set_CloudVolumeSize(float size) { m_fCloudVolumeSize = size; }
	void Set_CloudVolumeOffset(const XMFLOAT3& offset) { m_vCloudVolumeOffset = offset; }

private:
	float m_fCloudVolumeSize = 100.0f;
	XMFLOAT3 m_vCloudVolumeOffset = XMFLOAT3(0.0f, 50.0f, 0.0f);

public:
	static CCloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
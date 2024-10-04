#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Effect)
class CFireEffect final : public CBlendObject
{
private:
	enum TEXTURETYPE { FIRE_DIFFUSE, FIRE_NOISE, FIRE_ALPHA,FIRE_END};

public:
	typedef struct FIREEFFECTDESC
	{
		_float3		ScrollSpeeds = { 1.3f, 2.1f, 2.3f };
		_float3		Scales = { 1.0f, 2.0f, 3.0f };
		_float2		distortion1 = { 0.1f, 0.2f };
		_float2		distortion2 = { 0.1f, 0.3f };
		_float2		distortion3 = { 0.1f, 0.1f };
		_float		distortionScale = 0.8f;
		_float		distortionBias = 0.5f;
		_float4		vStartPos = { 0.f,0.f,0.f,1.f };
		_float3		vOffsetPos = { 0.f,0.f,0.f };
		_float2		vStartScale = { 1.f,1.f };
	};

private:
	CFireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireEffect(const CFireEffect& rhs);
	virtual ~CFireEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


private:		//components
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEXTURETYPE::FIRE_END] = {nullptr, nullptr, nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:		//Menber Method
	_float m_fAcctime = 0.f;
	shared_ptr<FIREEFFECTDESC> m_OwnDesc;

public:
	static CFireEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Effect)
class CParticle abstract : public CBlendObject
{
public:
	typedef struct PUBLIC_PARTICLEDESC
	{
		PARTICLETYPE						particleType = PART_POINT;
	};

	typedef struct PARTICLEDESC
	{
		CVIBuffer_Instance::INSTANCE_DESC	InstanceDesc;
		_bool								IsBloom = false;
		_bool								IsBlur = false;
		_bool								Desolve = false;
		_bool								IsColor = false;
		_bool								IsAlpha = false;
		_int								DesolveNum = 0;
		XMFLOAT3						    vStartColor{1.f,1.f,1.f};
		XMFLOAT3						    vEndColor{1.f,1.f,1.f};
		XMFLOAT3							vBloomColor{ 1.f,1.f,1.f };
		XMFLOAT3							vDesolveColor{ 1.f,1.f,1.f };
		EFFECTTYPE						    eType;
		_float4								vStartPos{0.f,0.f,0.f,1.f};
		_float								fDesolveLength = 0.f;
		_float								fBlurPower = 0.f;
	};

protected:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;
protected:
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Set_Target(CGameObject* Target);
	void Set_Rotation(_float Radian, _vector Axis);
protected:
	CTexture* m_pDesolveTexture = { nullptr };
	CShader*  m_pShaderCom = { nullptr };
	CTransform* m_pTarget = { nullptr };
	D3D11_BLEND_DESC blendStateDescription;
public:
	virtual void Free() override;
};

END
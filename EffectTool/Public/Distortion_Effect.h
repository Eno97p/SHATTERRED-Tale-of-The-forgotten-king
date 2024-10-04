#pragma once
#include "Effect_Define.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Effect)

class CDistortionEffect final : public CBlendObject
{
public:
	typedef struct DISTORTIONEFFECT
	{
		_bool								bFuncType = false;	//false는 커지게 true는 작아지게
		_bool								bDisolve = false;
		_int								iDesolveNum = 0;
		_float2								vStartScale = { 1.f,1.f };
		_float4								vStartpos = { 0.f,0.f,0.f,1.f };
		_float								fSpeed = 0.f;		//스케일이 커지는, 작아지는 속도
		_float								fLifeTime = 0.f;
		wstring								Texture = TEXT("");
		wstring								TexturePath = TEXT("");
	};

private:
	CDistortionEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDistortionEffect(const CDistortionEffect& rhs);
	virtual ~CDistortionEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render_Distortion() override;

private:
	shared_ptr<DISTORTIONEFFECT> m_OwnDesc;
	_float						 m_fLifetime = 0.f;
	_float						 m_fRatio = 0.f;

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_DesolveTexture = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDistortionEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
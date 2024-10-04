#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"
#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CParticle_Trail final : public CBlendObject
{
public:
	typedef struct TRAIL_DESC
	{
		CVIBuffer_Trail::TRAILDESC			traildesc;
		XMFLOAT3						    vStartColor{ 1.f,1.f,1.f };
		XMFLOAT3						    vEndColor{ 1.f,1.f,1.f };
		XMFLOAT3							vBloomColor{ 1.f,1.f,1.f };
		_float								fBloompower = 0.f;
		_bool								Desolve = false;
		_bool								Blur = false;
		_bool								Bloom = false;
		TRAILFUNCTYPE						eFuncType = TRAIL_EXTINCT;
		_int								DesolveNum = 0;
		wstring								Texture = TEXT("");
		wstring								TexturePath = TEXT("");
	};

private:
	CParticle_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Trail(const CParticle_Trail& rhs);
	virtual ~CParticle_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Blur() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };	
	CTexture*						m_DesolveTexture = { nullptr };
	CVIBuffer_Trail*				m_pVIBufferCom = { nullptr };
	TRAIL_DESC*						m_pTrailDesc = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
public:
	static CParticle_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
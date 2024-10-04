#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Effect)
class CBlackHorizon final : public CBlendObject
{
public:
	typedef struct HORIZON_DESC
	{
		_float3 vMinSize = { 0.01f,0.01f,0.01f };
		_float3 vMaxSize = { 4.f,4.f,4.f };
		_float fBloomPower = 1.f;
		_float3 vColor = { 1.f,1.f,1.f };
		_float3 vBloomColor = { 1.f,1.f,1.f };
		_float fStartdelay = 0.2f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CBlackHorizon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlackHorizon(const CBlackHorizon& rhs);
	virtual ~CBlackHorizon() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	void Set_BlackHole_Dead();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	unique_ptr<HORIZON_DESC>				m_OwnDesc;
	_float						m_AccTime = 0.f;
	_float						m_DeadDelay = 0.f;
	_bool						m_SetDead = false;
	_bool						Erase = false;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

public:
	static CBlackHorizon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
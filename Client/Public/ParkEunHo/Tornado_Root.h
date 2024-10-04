#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CTornado_Root final : public CBlendObject
{
public:
	typedef struct TORNADO_ROOT
	{
		_float StartYScale = 0.f;
		_float3 vMaxSize = { 0.f,0.f,0.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float fRotationSpeed = 0.f;
		_float fBloomPower = 0.f;
		_float fMaxLifeTime = 0.f;
		_float fUVSpeed = 0.f;
		_float fGrowSpeed = 1.f;
		_int NumDesolve = 0;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CTornado_Root(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTornado_Root(const CTornado_Root& rhs);
	virtual ~CTornado_Root() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	shared_ptr<TORNADO_ROOT>	m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

private:
	_bool						EffectDead = false;

public:
	static CTornado_Root* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
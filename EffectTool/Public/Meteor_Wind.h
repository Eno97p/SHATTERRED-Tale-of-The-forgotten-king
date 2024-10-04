#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CMeteor_Wind final : public CBlendObject
{
public:
	typedef struct METEOR_WIND
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 0.f,1.f,0.f };
		_float fBloomPower = 0.f;
		_float fMaxLifeTime = 0.5f; 
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CMeteor_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeteor_Wind(const CMeteor_Wind& rhs);
	virtual ~CMeteor_Wind() = default;

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
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	shared_ptr<METEOR_WIND>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;

private:
	_bool						EffectDead = false;

public:
	static CMeteor_Wind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
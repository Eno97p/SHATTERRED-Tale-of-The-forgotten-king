#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CMeteor_Core final : public CGameObject
{
public:
	typedef struct METEOR_CORE
	{
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float fMaxLifeTime = 1.f;
		_float fRotationSpeed = 0.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CMeteor_Core(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeteor_Core(const CMeteor_Core& rhs);
	virtual ~CMeteor_Core() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	shared_ptr<METEOR_CORE>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;

private:
	_bool						EffectDead = false;

public:
	static CMeteor_Core* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "Weapon.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CTornado_Wind final : public CWeapon
{
#define HITCOOLTIME 1.f

	enum TornadoModel { Tor_1, Tor_2, Tor_3, Tor_End };
public:
	typedef struct TORNADO_WIND
	{
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fColor2 = { 1.f,1.f,1.f };
		_float fBloomPower = 0.f;
		_float fRotationSpeed = 360.f;
		_float fMaxLifeTime = 10.f;
		_float fUVSpeed = 1.f;
		_float RadicalStrength = 1.f;
		_float fGrowSpeed = 3.f;
		_bool Opacity = false;
		_bool IsDistortion = false;
		_float OpacityPower = 0.5f;
		_int NumDesolve = 0;
		_int NumModels = Tor_1;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CTornado_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTornado_Wind(const CTornado_Wind& rhs);
	virtual ~CTornado_Wind() = default;

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
	shared_ptr<TORNADO_WIND>	m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

private:
	wstring						m_ModelProtoName = TEXT("");

	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	_float m_fHitCoolTime = HITCOOLTIME;

private:
	_bool						EffectDead = false;

public:
	static CTornado_Wind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
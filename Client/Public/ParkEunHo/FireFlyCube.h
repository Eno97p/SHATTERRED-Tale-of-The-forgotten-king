#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CFireFlyCube : public CBlendObject
{
public:
	typedef struct FIREFLYCUBE
	{ 
		_float3 fMinSize = { 0.01f,0.01f,0.01f };
		_float3 fMaxSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float fRotSpeed = 30.f;
		_float fStartRotAngle = 30.f;
		_float fLifeTime = 10.f;
		_float fStartdelay = 1.f;
		_float fBloomInterval = 0.5f;
		_float fBloomIntervalStartRatio = 0.8f;
		_float fChaseSpeed = 0.f;
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fBloomColor = { 1.f,1.f,1.f };
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CFireFlyCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireFlyCube(const CFireFlyCube& rhs);
	virtual ~CFireFlyCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

	void Set_Delete() { m_FireFlyDead = true; }
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	unique_ptr<FIREFLYCUBE>		m_OwnDesc;
	vector<CGameObject*>		m_Particles;
	_bool						m_FireFlyDead = false;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float						m_BloomTick = 0.f;
	_bool						m_BloomChange = false;
	_float						m_BloomPower = 1.f;
public:
	static CFireFlyCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
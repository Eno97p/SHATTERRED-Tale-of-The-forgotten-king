#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)
class CBlackHole_Ring final : public CBlendObject
{
public:
	typedef struct BLACKHOLE_RING
	{
		_float3 vMinSize = { 0.01f,0.01f,0.01f };
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fUVSpeed = 1.f;
		_float3 vColor = { 1.f,1.f,1.f };
		_float3 vBloomColor = { 1.f,1.f,1.f };
		_float fStartdelay = 1.f;
		_float fRotAngle = 30.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CBlackHole_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlackHole_Ring(const CBlackHole_Ring& rhs);
	virtual ~CBlackHole_Ring() = default;
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
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	unique_ptr<BLACKHOLE_RING>				m_OwnDesc;

private:
	_float						m_AccTime = 0.f;
	_float						m_DeadDelay = 0.f;
	_bool						m_SetDead = false;
	_bool						Erase = false;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

public:
	static CBlackHole_Ring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
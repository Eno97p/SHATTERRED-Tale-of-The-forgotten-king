#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CHedgehog final : public CBlendObject
{
public:
	typedef struct HEDGEHOG
	{
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float3 vOffset = { 0.f,-1.f,0.f };
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vMaxSize = { 2.f,2.f,2.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.5f;
		_float2 fThreadRatio = { 0.7f,1.f };
		_int	NumDesolve = 0;
	};

private:
	CHedgehog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHedgehog(const CHedgehog& rhs);
	virtual ~CHedgehog() = default;

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
	CTexture* m_pDesolveTexture = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

private:
	unique_ptr<HEDGEHOG>			m_OwnDesc;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;
	_vector							vStart;
	_bool							m_ParticleSpawn = false;
public:
	static CHedgehog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
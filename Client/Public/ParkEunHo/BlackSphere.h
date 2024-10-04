#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBlackSphere : public CBlendObject
{
public:
	typedef struct BLACKSPHERE
	{ 
		_float3 fMinSize = { 0.01f,0.01f,0.01f };
		_float3 fMaxSize = { 1.f,1.f,1.f };
		_float fStartdelay = 1.f;
		_float3 fColor = { 0.f,0.f,0.f };
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CBlackSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlackSphere(const CBlackSphere& rhs);
	virtual ~CBlackSphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_BlackHole_Dead();
	

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	unique_ptr<BLACKSPHERE>		m_OwnDesc;
	_float						m_AccTime = 0.f;
	_float						m_DeadDelay = 0.f;
	_bool						m_SetDead = false;
	_bool						Erase = false;

public:
	static CBlackSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
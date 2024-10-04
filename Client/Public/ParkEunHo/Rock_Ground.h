#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CRock_Ground final : public CGameObject
{
public:
	typedef struct ROCK_GROUND
	{
		XMFLOAT3 vMinSize = { 0.1f,0.1f,0.1f };
		XMFLOAT3 vMaxSize = { 1.f,1.f,1.f };
		_float fMaxLifeTime = 1.f;
		_float MinRatio = 0.f;
		_float MaxRatio = 0.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CRock_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRock_Ground(const CRock_Ground& rhs);
	virtual ~CRock_Ground() = default;

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
	shared_ptr<ROCK_GROUND>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_bool						Size[2] = { false,false };
	_float3						m_CurSize = { 0.f,0.f,0.f };
public:
	static CRock_Ground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CElectricCylinder final : public CBlendObject
{
public:
	typedef struct ANDRAS_ELECTRIC_DESC
	{
		_float3 vMaxSize = { 13.f,13.f,30.f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fBloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.3f;
		_float fUVSpeed = 4.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CElectricCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectricCylinder(const CElectricCylinder& rhs);
	virtual ~CElectricCylinder() = default;

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
	shared_ptr<ANDRAS_ELECTRIC_DESC> m_OwnDesc;

private:
	_float				m_fCurLifeTime = 0.f;
	_float				m_fRatio = 0.f;

public:
	static CElectricCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
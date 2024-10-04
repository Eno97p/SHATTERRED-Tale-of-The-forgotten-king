#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CWellCylinder final : public CBlendObject
{
public:
	typedef struct WELLCYLINDER
	{
		_float3 vSize = { 0.5f,0.5f,0.5f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 0.f,1.f,0.f };
		_float fBloomPower = 1.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CWellCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWellCylinder(const CWellCylinder& rhs);
	virtual ~CWellCylinder() = default;

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
	shared_ptr<WELLCYLINDER>			m_OwnDesc;

public:
	static CWellCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
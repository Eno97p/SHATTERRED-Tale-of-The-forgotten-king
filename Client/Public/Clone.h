#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CClone final : public CBlendObject
{
public:
	typedef struct CLONE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		const _uint* pState;
		_float fAnimDelay;
	}CLONE_DESC;

private:
	CClone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClone(const CClone& rhs);
	virtual ~CClone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_float m_fAnimDelay = 0.f;
	_float m_fLifeTime = 1.f;
	_float4x4 m_vParentMatrix;
	_float4x4				m_WorldMatrix;
	const _float4x4* m_pParentMatrix = { nullptr };
	const _uint* m_pState = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CClone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
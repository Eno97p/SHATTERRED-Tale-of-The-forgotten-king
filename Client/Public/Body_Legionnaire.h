#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CBody_Legionnaire final : public CPartObject
{
private:
	CBody_Legionnaire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Legionnaire(const CBody_Legionnaire& rhs);
	virtual ~CBody_Legionnaire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	virtual bool Get_AnimFinished() { return m_bAnimFinished; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bAnimFinished = false;
	_uint m_iPastAnimIndex = 0;
	_float m_fDistortionValue = 0.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Legionnaire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "Client_Defines.h"
#include "Particle.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CParticleMesh : public CParticle
{
public:
	typedef struct PARTICLEMESH  : public PUBLIC_PARTICLEDESC
	{
		EFFECTMODELTYPE						eModelType;
		PARTICLEDESC						SuperDesc;
	};
private:
	CParticleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleMesh(const CParticleMesh& rhs);
	virtual ~CParticleMesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Blur() override;

private:
	CModel*	 m_InstModelCom = { nullptr };
private:
	HRESULT Add_Components(const wstring& strModelPrototype);
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();

	const wchar_t* m_ModelPrototypeTag = nullptr;

private:
	shared_ptr<PARTICLEMESH> OwnDesc;

public:
	static CParticleMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
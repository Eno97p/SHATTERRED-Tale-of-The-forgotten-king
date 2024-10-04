#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CHexaHit final : public CBlendObject
{
public:
	typedef struct HEXASHIELDHIT
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float fLifeTime = 1.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CHexaHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHexaHit(const CHexaHit& rhs);
	virtual ~CHexaHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render_Distortion() override;

private: 
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	shared_ptr<HEXASHIELDHIT>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

public:
	static CHexaHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
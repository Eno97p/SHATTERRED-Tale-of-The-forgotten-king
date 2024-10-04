#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)
class CElectronic final : public CBlendObject
{
public:
	typedef struct ELECTRONICDESC
	{
		_float3 vSize = { 8.f,8.f,8.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float fMaxLifeTime = 0.3f;
		_float fBloomPower = 1.f;
		_float fUVSpeed = 4.f;
		_float3 vColor = { 1.f,1.f,1.f };
		_float3 vBloomColor = { 1.f,1.f,1.f };
		_int	ParticleIndex = 0;
	};

private:
	CElectronic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectronic(const CElectronic& rhs);
	virtual ~CElectronic() = default;
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CTexture*			m_pNoiseTex = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	shared_ptr<ELECTRONICDESC>				OwnDesc;

	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

private:
	_float				m_fCurLifeTime = 0.f;
	_float				m_fRatio = 0.f;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

public:
	static CElectronic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
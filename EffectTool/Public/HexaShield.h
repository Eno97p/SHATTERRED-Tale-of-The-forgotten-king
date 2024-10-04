#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
#include "HexaHit.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CHexaShield final : public CBlendObject
{
public:
	typedef struct HEXASHIELD
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fBloomColor = { 1.f,1.f,1.f };
		_float3 fBloomColor2 = { 1.f,1.f,1.f };
		_float fBloomPower = 0.5f;
		_float fUVSpeed = 1.f;
		_float fOpacitySpeed = 0.1f;
		_float OpacityPower = 0.5f;
		_float LoopInterval = 0.5f;
		_float DesolveTime = 1.f;
		_float HitTime = 0.2f;
		_float RotationSpeed = 90.f;
		CHexaHit::HEXASHIELDHIT HitDesc{};
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CHexaShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHexaShield(const CHexaShield& rhs);
	virtual ~CHexaShield() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;

	void Set_Delete() { m_DesolveStart = true; }
	void Set_Shield_Hit();
private: 
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();
	HRESULT Generate_HexaHit();
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	shared_ptr<HEXASHIELD>		m_OwnDesc;
	_float3						m_CurrentSize{};

private:
	_float						m_Interval = 0.f;
	_float						m_AccTime = 0.f; 
	_float						m_DesolveThreadhold = 0.f;
	_float						m_DesolveRatio = 0.f;
	_bool						m_DesolveStart = false;
	_bool						m_IntervalChange = false;
	_float						m_HitInterval = 0.f;
	_float						m_HitRatio = 0.f;
	_bool						IsHit = false;

public:
	static CHexaShield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CFirePillar_Bottom final : public CBlendObject
{
public:
	typedef struct FIREPILLAR_BOTTOM
	{
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fColor2 = { 1.f,1.f,1.f };
		_float fBloomPower = 0.5f;
		_float fMaxLifeTime = 10.f;
		_float fUVSpeed = 1.f;
		_float RadicalStrength = 0.f;
		_float fGrowSpeed = 1.f;
		_bool Opacity = true;
		_bool IsDistortion = false;
		_float OpacityPower = 0.5f;
		_int NumDesolve = 33;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CFirePillar_Bottom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFirePillar_Bottom(const CFirePillar_Bottom& rhs);
	virtual ~CFirePillar_Bottom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	shared_ptr<FIREPILLAR_BOTTOM>	m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

private:
	_bool						SizeUpDone = { false };
public:
	static CFirePillar_Bottom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Effect)
class CFirePillarEffect final : public CBlendObject
{
public:
	enum FIREPILLARMODELNUM { F_1, F_2, F_3, F_4, F_END};
public:
	typedef struct FIREPILLAREFFECTDESC
	{
		_float3 vMaxSize = { 0.5f,3.f,0.5f };
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
		_int NumModels = F_1;
		_float SizeInterval = 0.1f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CFirePillarEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFirePillarEffect(const CFirePillarEffect& rhs);
	virtual ~CFirePillarEffect() = default;

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
	shared_ptr<FIREPILLAREFFECTDESC>	m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

private:
	wstring						m_ModelProtoName = TEXT("");

private:
	_float						m_Interval = 0.f;
	_bool						SizeDirectionChange = false; //false 일때 작아지게
	_bool						SizeUpDone[2] = {false,false};

public:
	static CFirePillarEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
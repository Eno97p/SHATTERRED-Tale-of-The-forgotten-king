#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include "VIBuffer_SwordTrail.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CSTrail final : public CBlendObject
{
public:
	typedef struct STRAIL_DESC
	{
		CVIBuffer_SwordTrail::SwordTrailDesc			traildesc;
		_bool								isBloom = false;
		_bool								isDestortion = false;
		_int								iDesolveNum = 0;
		_float3								vColor = { 1.f,1.f,1.f };
		_float3								vBloomColor = { 1.f,1.f,1.f };
		_float								fBloomPower = 1.f;
		wstring								Texture = TEXT("");
		wstring								TexturePath = TEXT("");
	};

private:
	CSTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSTrail(const CSTrail& rhs);
	virtual ~CSTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Distortion() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };	
	CTexture*						m_DesolveTexture = { nullptr };
	CVIBuffer_SwordTrail*			m_pVIBufferCom = { nullptr };
	shared_ptr<STRAIL_DESC>			m_pTrailDesc = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
public:
	static CSTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
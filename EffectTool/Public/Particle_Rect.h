#pragma once

#include "Effect_Define.h"
#include "Particle.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Rect;
END

BEGIN(Effect)

class CParticle_Rect final : public CParticle
{
public:
	typedef struct PARTICLERECT : public PUBLIC_PARTICLEDESC
	{
		~PARTICLERECT()
		{
			Texture.clear();
			TexturePath.clear();
		}
		wstring									 Texture = TEXT("");
		wstring									 TexturePath = TEXT("");
		PARTICLEDESC							 SuperDesc;
	};
private:
	CParticle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Rect(const CParticle_Rect& rhs);
	virtual ~CParticle_Rect() = default;

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
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Instance_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();

private:
	PARTICLERECT*		OwnDesc = nullptr;

public:
	static CParticle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
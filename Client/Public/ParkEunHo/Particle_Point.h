#pragma once
#include "Client_Defines.h"
#include "Particle.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(Client)

class CParticle_Point final : public CParticle
{
public:
	typedef struct PARTICLEPOINT : public PUBLIC_PARTICLEDESC
	{
		~PARTICLEPOINT(){
			Texture.clear();
			TexturePath.clear();
		}
		wstring									 Texture = TEXT("");
		wstring									 TexturePath = TEXT("");
		PARTICLEDESC							 SuperDesc;
	};
private:
	CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Point(const CParticle_Point& rhs);
	virtual ~CParticle_Point() = default;

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
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Instance_Point*		m_pVIBufferCom = { nullptr };	
	PARTICLEPOINT*					OwnDesc = nullptr;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
public:
	static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
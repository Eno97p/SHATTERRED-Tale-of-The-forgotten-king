#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Effect)
class CTextureFrame final : public CBlendObject
{
public:
	enum TXFRAMESTATE { TX_ISCOLOR, TX_DESOLVE,TX_BLUR,TX_BILL,TX_END };
	enum TX_FRAMEFLOAT { MAXFRAME, FRAMESPEED, BLURPOWER, FRAMEFLOAT_END};
	typedef struct TEXFRAMEDESC : public CGameObject::GAMEOBJECT_DESC
	{
		_int									 iDivision = 0;
		wstring									 Texture = L"";
		wstring									 TexturePath = L"";
		_bool									 State[TXFRAMESTATE::TX_END] = { false,false,false,false };
		_float									 Frame[TX_FRAMEFLOAT::FRAMEFLOAT_END] = {0.f,0.f,0.f};
		_float3									 Color[3] = {_float3(0.f,0.f,0.f), _float3(0.f,0.f,0.f), _float3(0.f,0.f,0.f)};
		_float4									 vStartPos = {0.f,0.f,0.f,1.f};
		_float2									 vSize = { 1.f,1.f};
		_int									 iNumDesolve = 0;
	};

private:
	CTextureFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextureFrame(const CTextureFrame& rhs);
	virtual ~CTextureFrame() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pDesolveTexture = { nullptr };
private:
	shared_ptr<TEXFRAMEDESC> m_OwnDesc;
private:
	_float	m_fCurFrame = 0.f;
	_float	m_fFrameRatio = 0.f;
public:
	static CTextureFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
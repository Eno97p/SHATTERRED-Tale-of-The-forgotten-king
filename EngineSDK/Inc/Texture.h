#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Prototype(istream& os);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Save_Texture(ostream& os);
	ID3D11ShaderResourceView* Get_Texture() { return m_Textures.front(); }
	vector<ID3D11ShaderResourceView*>	 Get_Textures() { return m_Textures; }
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);
	HRESULT Fetch_Pixel_FromPixelShader(const _float2& vTexCoord, _float4* pPixelColor, _uint iTextureIndex);
private:
	void TransformFilePath(_tchar* filePath);
private:
	_uint													m_iNumTextures = { 0 };
public:
	vector<ID3D11ShaderResourceView*>						m_Textures;
	_tchar			m_szTextureFilePath[MAX_PATH] = TEXT("");
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& os);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
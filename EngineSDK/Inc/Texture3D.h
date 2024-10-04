#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture3D final : public CComponent
{
private:
	CTexture3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture3D(const CTexture3D& rhs);
	virtual ~CTexture3D() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Save_Texture(ostream& os);
	HRESULT Bind_ShaderResource(CShader* pShader, const char* pConstantName);

	ID3D11ShaderResourceView* Get_Texture() { return m_Textures.front(); }
	vector<ID3D11ShaderResourceView*>	 Get_Textures() { return m_Textures; }
public:
	//HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Fetch_Pixel_FromPixelShader(const _float2& vTexCoord, _float4* pPixelColor, _uint iTextureIndex);
private:
	void TransformFilePath(_tchar* filePath);
private:
	_uint													m_iNumTextures = { 0 };
public:
	vector<ID3D11ShaderResourceView*>						m_Textures;
	_tchar			m_szTextureFilePath[MAX_PATH] = TEXT("");

private:
	ID3D11Texture3D* m_pTexture3D = nullptr;
	ID3D11ShaderResourceView* m_pSRV = nullptr;
	UINT m_iWidth = 0;
	UINT m_iHeight = 0;
	UINT m_iDepth = 0;
public:
	static CTexture3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath);
	//static CTexture3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures);
	//static CTexture3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& os);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
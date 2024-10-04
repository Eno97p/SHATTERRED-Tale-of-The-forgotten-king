#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent{ rhs }
	, m_iNumTextures{ rhs.m_iNumTextures }
	, m_Textures{ rhs.m_Textures }
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	_tchar			szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar			szEXT[MAX_PATH] = TEXT("");

	m_Textures.reserve(iNumTextures);

	m_iNumTextures = iNumTextures;

	for (size_t i = 0; i < iNumTextures; i++)
	{
		/* ..\Bin\Resources\Textures\Default0.jpg */
		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		wsprintf(m_szTextureFilePath, szTextureFilePath);
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT			hr{};

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		// else if(false == lstrcmp(szEXT, TEXT(".tga")))

		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Prototype(istream& os)
{
	os.read(reinterpret_cast<char*>(&m_iNumTextures), sizeof(_uint));

	os.read(reinterpret_cast<char*>(m_szTextureFilePath), sizeof(_tchar) * MAX_PATH);
	TransformFilePath(m_szTextureFilePath);

	m_Textures.reserve(m_iNumTextures);

	_tchar			szEXT[MAX_PATH] = TEXT("");
	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		_wsplitpath_s(m_szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT			hr{};
		

		
		
		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, m_szTextureFilePath, nullptr, &pSRV);

		else
			hr = CreateWICTextureFromFile(m_pDevice, m_szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Save_Texture(ostream& os)
{
	os.write(reinterpret_cast<char*>(&m_iNumTextures), sizeof(_uint));

	os.write(reinterpret_cast<char*>(m_szTextureFilePath), sizeof(_tchar) * MAX_PATH);
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_Textures.front(), m_iNumTextures);
}

HRESULT CTexture::Fetch_Pixel_FromPixelShader(const _float2& vTexCoord, _float4* pPixelColor, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV = m_Textures[iTextureIndex];
	ID3D11Resource* pResource = nullptr;
	pSRV->GetResource(&pResource);

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
	Safe_Release(pResource);

	if (FAILED(hr))
		return hr;

	D3D11_TEXTURE2D_DESC desc;
	pTexture->GetDesc(&desc);

	// 스테이징 텍스처 생성
	D3D11_TEXTURE2D_DESC stagingDesc = desc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D* pStagingTexture = nullptr;
	hr = m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture);
	if (FAILED(hr))
	{
		Safe_Release(pTexture);
		return hr;
	}

	// 원본 텍스처를 스테이징 텍스처로 복사
	m_pContext->CopyResource(pStagingTexture, pTexture);

	// 픽셀 데이터 읽기
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		UINT row = (UINT)(vTexCoord.y * (desc.Height - 1));
		UINT col = (UINT)(vTexCoord.x * (desc.Width - 1));

		UINT pixelSize = 4; // Assuming RGBA8 format
		BYTE* pData = (BYTE*)mappedResource.pData + (row * mappedResource.RowPitch) + (col * pixelSize);

		pPixelColor->x = pData[0] / 255.0f; // R
		pPixelColor->y = pData[1] / 255.0f; // G
		pPixelColor->z = pData[2] / 255.0f; // B
		pPixelColor->w = pData[3] / 255.0f; // A

		m_pContext->Unmap(pStagingTexture, 0);
	}

	Safe_Release(pStagingTexture);
	Safe_Release(pTexture);

	return hr;
}

void CTexture::TransformFilePath(_tchar* filePath)
{
	_tchar fileName[MAX_PATH];
	_tchar fileExtension[MAX_PATH];
	_tchar drive[MAX_PATH];
	_tchar dir[MAX_PATH];
	_tsplitpath_s(filePath, drive, dir, fileName, fileExtension);
	_tchar newPath[MAX_PATH] = TEXT("../../Client/Bin/Resources/Textures/DDS_Storage/");
	_tchar newExtension[] = TEXT(".dds");
	_tchar newFilePath[MAX_PATH];
	_stprintf_s(newFilePath, MAX_PATH, TEXT("%s%s%s"), newPath, fileName, newExtension);
	_tcscpy_s(filePath, MAX_PATH, newFilePath);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed To Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& os)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(os)))
	{
		MSG_BOX("Failed To LoadCreate : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);
	m_Textures.clear();
}

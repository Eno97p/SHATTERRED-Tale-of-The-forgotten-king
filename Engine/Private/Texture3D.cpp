#include "..\Public\Texture3D.h"
#include "Shader.h"

CTexture3D::CTexture3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture3D::CTexture3D(const CTexture3D& rhs)
	: CComponent{ rhs }
	, m_pTexture3D{ rhs.m_pTexture3D }
	, m_pSRV{ rhs.m_pSRV }
	, m_iWidth{ rhs.m_iWidth }
	, m_iHeight{ rhs.m_iHeight }
	, m_iDepth{ rhs.m_iDepth }
{
	Safe_AddRef(m_pTexture3D);
	Safe_AddRef(m_pSRV);
}


HRESULT CTexture3D::Initialize_Prototype(const wstring& strTextureFilePath)
{
	wcscpy_s(m_szTextureFilePath, strTextureFilePath.c_str());

	ID3D11Resource* pResource = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		m_pDevice,
		m_szTextureFilePath,
		&pResource,
		&m_pSRV
	);

	if (FAILED(hr))
		return E_FAIL;

	hr = pResource->QueryInterface(__uuidof(ID3D11Texture3D), (void**)&m_pTexture3D);
	Safe_Release(pResource);

	if (FAILED(hr))
		return E_FAIL;

	D3D11_TEXTURE3D_DESC desc;
	m_pTexture3D->GetDesc(&desc);
	m_iWidth = desc.Width;
	m_iHeight = desc.Height;
	m_iDepth = desc.Depth;

	return S_OK;
}

//HRESULT CTexture3D::Initialize_Prototype(istream& os)
//{
//	os.read(reinterpret_cast<char*>(&m_iNumTextures), sizeof(_uint));
//
//	os.read(reinterpret_cast<char*>(m_szTextureFilePath), sizeof(_tchar) * MAX_PATH);
//	TransformFilePath(m_szTextureFilePath);
//
//	m_Textures.reserve(m_iNumTextures);
//
//	_tchar			szEXT[MAX_PATH] = TEXT("");
//	for (size_t i = 0; i < m_iNumTextures; i++)
//	{
//		_wsplitpath_s(m_szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);
//
//		ID3D11ShaderResourceView* pSRV = { nullptr };
//
//		HRESULT			hr{};
//		
//
//		
//		
//		if (false == lstrcmp(szEXT, TEXT(".dds")))
//			hr = CreateDDSTextureFromFile(m_pDevice, m_szTextureFilePath, nullptr, &pSRV);
//
//		else
//			hr = CreateWICTexture3DFromFile(m_pDevice, m_szTextureFilePath, nullptr, &pSRV);
//
//		if (FAILED(hr))
//			return E_FAIL;
//
//		m_Textures.push_back(pSRV);
//	}
//	return S_OK;
//}

HRESULT CTexture3D::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture3D::Save_Texture(ostream& os)
{
	os.write(reinterpret_cast<char*>(&m_iNumTextures), sizeof(_uint));

	os.write(reinterpret_cast<char*>(m_szTextureFilePath), sizeof(_tchar) * MAX_PATH);
	return S_OK;
}

HRESULT CTexture3D::Bind_ShaderResource(CShader * pShader, const char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}


HRESULT CTexture3D::Fetch_Pixel_FromPixelShader(const _float2& vTexCoord, _float4* pPixelColor, _uint iTextureIndex)
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

	// ������¡ �ؽ�ó ����
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

	// ���� �ؽ�ó�� ������¡ �ؽ�ó�� ����
	m_pContext->CopyResource(pStagingTexture, pTexture);

	// �ȼ� ������ �б�
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

void CTexture3D::TransformFilePath(_tchar* filePath)
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

CTexture3D* CTexture3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath)
{
	CTexture3D* pInstance = new CTexture3D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath)))
	{
		MSG_BOX("Failed To Created : CTexture3D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture3D::Clone(void* pArg)
{
	CTexture3D* pInstance = new CTexture3D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTexture3D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture3D::Free()
{
	__super::Free();

	Safe_Release(m_pTexture3D);
	Safe_Release(m_pSRV);
}



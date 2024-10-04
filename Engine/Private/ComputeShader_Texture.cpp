#include "ComputeShader_Texture.h"
#pragma comment(lib, "d3dcompiler.lib")

CComputeShader_Texture::CComputeShader_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CComputeShader_Texture::CComputeShader_Texture(const CComputeShader_Texture& rhs)
	: CComponent{ rhs }
{
}

HRESULT CComputeShader_Texture::Initialize_Prototype(const wstring& ShaderFilePath, _uint NumPasses, const _char** EntryPointNames)
{
	WORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* csBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr;
	for (_uint i = 0; i < NumPasses; i++)
	{
		hr = D3DCompileFromFile(ShaderFilePath.c_str(),
			0,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			EntryPointNames[i],
			"cs_5_0",
			shaderFlags,
			0,
			&csBlob,
			&errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				Safe_Release(errorBlob);
			}
			return E_FAIL;
		}
		hr = m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &computeShaders[i]);
		if (FAILED(hr)) {
			std::cerr << "컴퓨트 셰이더 생성 실패" << std::endl;
			Safe_Release(csBlob);
			return E_FAIL;
		}

		Safe_Release(csBlob);
	}


	Initialize(nullptr);
	return S_OK;
}

HRESULT CComputeShader_Texture::Initialize(void* arg)
{
	// 입력 텍스처 생성
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 1280;
	textureDesc.Height = 720;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;

	HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &inputTexture);
	if (FAILED(hr)) {
		std::cerr << "입력 텍스처 생성 실패" << std::endl;
		return E_FAIL;
	}

	for (int i = 0; i < MAX_PASS; i++)
	{
		// 출력 텍스처 생성
		textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &outputTexture[i]);
		if (FAILED(hr)) {
			std::cerr << "출력 텍스처 생성 실패" << std::endl;
			inputTexture->Release();
			return E_FAIL;
		}

		// 출력 텍스처에 대한 UAV 생성
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		hr = m_pDevice->CreateUnorderedAccessView(outputTexture[i], &uavDesc, &outputUAV[i]);
		if (FAILED(hr)) {
			std::cerr << "출력 UAV 생성 실패" << std::endl;
			inputTexture->Release();
			outputTexture[i]->Release();
			return E_FAIL;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = m_pDevice->CreateShaderResourceView(outputTexture[i], &srvDesc, &outputSRV[i]);
		if (FAILED(hr)) {
			std::cerr << "출력 텍스처에 대한 SRV 생성 실패" << std::endl;
			return E_FAIL;
		}
	}



	return S_OK;
}

HRESULT CComputeShader_Texture::Compute(_uint dispatchX, _uint dispatchY, _uint dispatchZ)
{
	// 컴퓨트 셰이더 설정
	m_pContext->CSSetShader(computeShaders[m_iPass], nullptr, 0);
	m_pContext->CSSetShaderResources(0, 1, &inputSRV[m_iPass]);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &outputUAV[m_iPass], nullptr);

	// 컴퓨트 셰이더 디스패치
	m_pContext->Dispatch(dispatchX, dispatchY, dispatchZ);

	// 리소스를 해제
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	//m_pContext->PSSetShaderResources(0, 1, &outputSRV);

	return S_OK;
}

HRESULT CComputeShader_Texture::Begin(_uint iPassIndex)
{
	m_iPass = iPassIndex;
	return S_OK;
}

HRESULT CComputeShader_Texture::Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength)
{
	return S_OK;
}

HRESULT CComputeShader_Texture::Get_RawValue(const _char* pConstantName, const void* pValue, _uint iLength)
{
	return S_OK;
}

HRESULT CComputeShader_Texture::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	inputSRV[m_iPass] = pSRV;
	pSRV = nullptr;
	return S_OK;
}

ID3D11ShaderResourceView* CComputeShader_Texture::Get_SRV(_uint PassNum)
{
	return outputSRV[PassNum];
}

HRESULT CComputeShader_Texture::Bind_UAV(const _char* pConstantName, ID3D11UnorderedAccessView* pUAV)
{
	return S_OK;
}

HRESULT CComputeShader_Texture::Get_UAV(ID3D11UnorderedAccessView* pUAV)
{
	return S_OK;
}

CComputeShader_Texture* CComputeShader_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& ShaderFilePath, _uint NumPasses, const _char** EntryPointNames)
{
	CComputeShader_Texture* pInstance = new CComputeShader_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ShaderFilePath, NumPasses, EntryPointNames)))
	{
		MSG_BOX("Failed to Created : CComputeShader_Texture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CComputeShader_Texture::Clone(void* pArg)
{
	CComputeShader_Texture* pInstance = new CComputeShader_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CComputeShader_Texture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComputeShader_Texture::Free()
{
	__super::Free();
	for (_uint i = 0; i < MAX_PASS; i++)
	{
		Safe_Release(computeShaders[i]);
		Safe_Release(outputSRV[i]);
		Safe_Release(outputTexture[i]);
		Safe_Release(outputUAV[i]);
	}
	Safe_Release(inputTexture);
}

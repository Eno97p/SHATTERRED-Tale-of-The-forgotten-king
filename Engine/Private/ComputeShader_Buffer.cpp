#include "ComputeShader_Buffer.h"
#pragma comment(lib, "d3dcompiler.lib")

CComputeShader_Buffer::CComputeShader_Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CComputeShader_Buffer::CComputeShader_Buffer(const CComputeShader_Buffer& rhs)
	: CComponent{ rhs }, m_pComputeShader{ rhs.m_pComputeShader }
{
	Safe_AddRef(m_pComputeShader);
}

HRESULT CComputeShader_Buffer::Initialize_Prototype(const wstring& ShaderFilePath, const _char* EntryPointName)
{
	WORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* csBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr;
	hr = D3DCompileFromFile(ShaderFilePath.c_str(),
		0,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		EntryPointName,
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
	hr = m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_pComputeShader);
	if (FAILED(hr)) {
		std::cerr << "컴퓨트 셰이더 생성 실패" << std::endl;
		Safe_Release(csBlob);
		return E_FAIL;
	}

	Safe_Release(csBlob);
	Safe_Release(errorBlob);

	return S_OK;
}

HRESULT CComputeShader_Buffer::Initialize(void* arg)
{
	CSBUFFER_DESC csBufferDesc = *((CSBUFFER_DESC*)arg);
	m_iNumInputVariables = csBufferDesc.numInputVariables;
	m_iVariableNum = csBufferDesc.variableNum;
	m_iVariableSize = csBufferDesc.variableSize;
	void** ppArg = csBufferDesc.arg;

	// 입력 버퍼 생성 (텍스처 대신 버퍼 사용) - 바뀌었다
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = m_iVariableSize * m_iVariableNum; // 버퍼 크기 설정
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = m_iVariableSize;

	for (int i = 0; i < m_iNumInputVariables; i++)
	{
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = ppArg[i]; // 초기 데이터 설정

		ID3D11Buffer* pBuffer = nullptr;
		HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &initData, &pBuffer);
		if (FAILED(hr)) {
			Safe_Release(pBuffer);
			return E_FAIL;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = csBufferDesc.variableFormat; // 포맷 설정 (예: float 데이터)
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementWidth = m_iVariableSize;
		srvDesc.Buffer.NumElements = m_iVariableNum;

		hr = m_pDevice->CreateShaderResourceView(pBuffer, &srvDesc, &m_pInputSRV[i]);
		if (FAILED(hr)) {
			Safe_Release(pBuffer);
			return E_FAIL;
		}

		Safe_Release(pBuffer);
	}

	bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pOutputBuffer);
	if (FAILED(hr)) {
		Safe_Release(m_pOutputBuffer);
		return E_FAIL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = csBufferDesc.variableFormat; // 포맷 설정 (예: float 데이터)
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = m_iVariableNum;

	hr = m_pDevice->CreateUnorderedAccessView(m_pOutputBuffer, &uavDesc, &m_pOutputUAV);
	if (FAILED(hr)) {
		Safe_Release(m_pOutputBuffer);
		Safe_Release(m_pOutputUAV);
		return E_FAIL;
	}

	// 1. 스테이징 버퍼 생성
	D3D11_BUFFER_DESC stagingBufferDesc;
	ZeroMemory(&stagingBufferDesc, sizeof(stagingBufferDesc));
	stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
	stagingBufferDesc.ByteWidth = m_iVariableSize * m_iVariableNum; // 버퍼 크기를 적절히 설정
	stagingBufferDesc.BindFlags = 0;
	stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = m_pDevice->CreateBuffer(&stagingBufferDesc, nullptr, &m_pStagingBuffer);
	if (FAILED(hr)) {
		Safe_Release(m_pOutputBuffer);
		Safe_Release(m_pOutputUAV);
		Safe_Release(m_pStagingBuffer);
		return E_FAIL;
	}

	return S_OK;
}

void* CComputeShader_Buffer::Compute(_uint dispatchX, _uint dispatchY, _uint dispatchZ)
{
	// 컴퓨트 셰이더 설정
	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);
	for (_uint i = 0; i < m_iNumInputVariables; i++)
	{
		m_pContext->CSSetShaderResources(i, 1, &m_pInputSRV[i]);
	}
	
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pOutputUAV, nullptr);

	// 컴퓨트 셰이더 디스패치
	m_pContext->Dispatch(dispatchX, dispatchY, dispatchZ);

	m_pContext->CopyResource(m_pStagingBuffer, m_pOutputBuffer);
	
	// 3. 데이터 매핑
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
	{
		return nullptr;
	}

	// 4. 데이터 읽기
	void* pData = mappedResource.pData;

	m_pContext->Unmap(m_pStagingBuffer, 0);

	// 리소스를 해제
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	return pData;
}

CComputeShader_Buffer* CComputeShader_Buffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& ShaderFilePath, const _char* EntryPointName)
{
	CComputeShader_Buffer* pInstance = new CComputeShader_Buffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ShaderFilePath, EntryPointName)))
	{
		MSG_BOX("Failed to Created : CComputeShader_Buffer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CComputeShader_Buffer::Clone(void* pArg)
{
	CComputeShader_Buffer* pInstance = new CComputeShader_Buffer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CComputeShader_Buffer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComputeShader_Buffer::Free()
{
	__super::Free();
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pOutputUAV);
	Safe_Release(m_pOutputBuffer);
	Safe_Release(m_pStagingBuffer);
	for (_uint i = 0; i < MAX_INPUTDATA; i++)
	{
		Safe_Release(m_pInputSRV[i]);
	}
}

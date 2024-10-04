#pragma once
#include "Base.h"
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CComputeShader_Buffer : public CComponent
{
#define MAX_INPUTDATA 5

public:
	typedef struct CSBUFFER_DESC
	{
		_uint numInputVariables;
		void** arg;
		size_t variableSize;
		size_t variableNum;
		DXGI_FORMAT variableFormat;
	}CSBUFFER_DESC;

private:
	CComputeShader_Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader_Buffer(const CComputeShader_Buffer& rhs);
	virtual ~CComputeShader_Buffer() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& ShaderFilePath, const _char* EntryPointName);
	virtual HRESULT Initialize(void* arg);
	void* Compute(_uint dispatchX, _uint dispatchY, _uint dispatchZ);

private:
	_uint						m_iNumInputVariables = { 0 };

	ID3D11ComputeShader* m_pComputeShader = { nullptr };
	ID3D11ShaderResourceView* m_pInputSRV[MAX_INPUTDATA] = { nullptr };
	ID3D11UnorderedAccessView* m_pOutputUAV = { nullptr };
	ID3D11Buffer* m_pOutputBuffer = { nullptr };
	ID3D11Buffer* m_pStagingBuffer = { nullptr };
	size_t m_iVariableSize;
	size_t m_iVariableNum;


public:
	static CComputeShader_Buffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& ShaderFilePath, const _char* EntryPointName);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
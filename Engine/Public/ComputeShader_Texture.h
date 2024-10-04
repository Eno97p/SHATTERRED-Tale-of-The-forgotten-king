#pragma once
#include "Base.h"
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CComputeShader_Texture : public CComponent
{
#define MAX_PASS 20

public:
	struct InitData {
		_uint size;
		_uint NumElements = 1;
	};

private:
	CComputeShader_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader_Texture(const CComputeShader_Texture& rhs);
	virtual ~CComputeShader_Texture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& ShaderFilePath, _uint NumPasses, const _char** EntryPointNames);
	virtual HRESULT Initialize(void* arg) override;
	HRESULT Compute(_uint dispatchX, _uint dispatchY, _uint dispatchZ);
	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength);
	HRESULT Get_RawValue(const _char* pConstantName, const void* pValue, _uint iLength);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	ID3D11ShaderResourceView* Get_SRV(_uint PassNum);
	HRESULT Bind_UAV(const _char* pConstantName, ID3D11UnorderedAccessView* pUAV);
	HRESULT Get_UAV(ID3D11UnorderedAccessView* pUAV);

private:
	_uint m_iPass = 0;

	_uint						m_iNumPasses = { 0 };

	ID3D11ComputeShader* computeShaders[MAX_PASS] = { nullptr };
	ID3D11Texture2D* inputTexture = nullptr;
	ID3D11ShaderResourceView* inputSRV[MAX_PASS] = { nullptr };
	ID3D11Texture2D* outputTexture[MAX_PASS] = { nullptr };
	ID3D11UnorderedAccessView* outputUAV[MAX_PASS] = { nullptr };
	ID3D11ShaderResourceView* outputSRV[MAX_PASS] = { nullptr };


public:
	static CComputeShader_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& ShaderFilePath, _uint NumPasses, const _char** EntryPointNames);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
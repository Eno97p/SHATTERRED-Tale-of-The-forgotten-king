#pragma once

#include "Component.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct Shader_Editable_Desc : public CComponent::ComponentDesc
	{
		vector<wstring> PassNames;
		_uint iCurrentPass = 0;
		_uint* iNumPasses = nullptr;
	}Shader_Editable_Desc;


private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElements, _bool IsOptimization);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Begin(_uint iPassIndex, ID3D11DeviceContext* pDeferredContext);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);
	HRESULT Unbind_SRVs();


	virtual Shader_Editable_Desc* GetData() override {
		return &m_ShaderComponentDesc; }
private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumPasses = { 0 };

	vector<ID3D11InputLayout*>	m_InputLayouts;



	Shader_Editable_Desc			m_ShaderComponentDesc;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElements,_bool IsOptimization=true);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
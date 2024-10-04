#pragma once
#include"CudaDefines.h"
#include "Base.h"


BEGIN(Engine)
class ENGINE_DLL CCuda  final : public CBase
{
private:
	CCuda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCuda() = default;

public:
	HRESULT Initialize();







private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	cudaStream_t m_cudaStream = { nullptr };
	cudaGraphicsResource* m_cudaResource = { nullptr };

public:
	static CCuda* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;










public:
	static HRESULT LaunchKernel_CullingInstance(VTXMATRIX* d_instanceData, int numInstances, float3 cameraPos, float maxRenderDistance, int* d_visibleCount);
	static HRESULT LaunchKernel_CullingInstance(cudaGraphicsResource* cudaResource, int numInstances, float3 cameraPos, float maxRenderDistance, int* d_visibleCount);
	static HRESULT InitCuda();

};





END
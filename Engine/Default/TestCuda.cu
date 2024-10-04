#include<Windows.h>
#include"TestCuda.cuh"





__constant__ float3 g_cameraPos;
__constant__ float g_maxDistanceSquared;



__device__ int atomicAdd(int* address, int val);

__global__ void cullingKernel(cu_VTXMATRIX* instances, int numInstances, float3 cameraPos, float maxDistanceSquared, int* visibleCount)
{

	unsigned int index_X = blockIdx.x * blockDim.x + threadIdx.x;
	
	if (index_X < numInstances)
	{
		float4 instancePos = make_float4(instances[index_X].vTranslation.x, instances[index_X].vTranslation.y, instances[index_X].vTranslation.z, instances[index_X].vTranslation.w);
		float distanceSquared = (instancePos.x - cameraPos.x) * (instancePos.x - cameraPos.x) + 
								(instancePos.y - cameraPos.y) * (instancePos.y - cameraPos.y) +
								(instancePos.z - cameraPos.z) * (instancePos.z - cameraPos.z);

		if (distanceSquared < maxDistanceSquared)
		{
			int visibleIndex = atomicAdd(visibleCount, 1);
			if (visibleIndex != index_X)
			{
				// Swap
				cu_VTXMATRIX temp = instances[visibleIndex];
				instances[visibleIndex] = instances[index_X];
				instances[index_X] = temp;
			}
		}

	}
}

cudaError_t LaunchCullingKernel(cu_VTXMATRIX* d_instanceData, int numInstances, float3 cameraPos, float maxDistanceSquared, int* d_visibleCount)
{
	const DWORD MAX_NUM_PER_ONCE = 65536 * 10;
	const DWORD THREAD_NUM_PER_BLOCK = 1024;
	cudaError_t cudaStatus;

	while (numInstances)
	{
		DWORD NumPerOnce = numInstances;
		if(NumPerOnce > MAX_NUM_PER_ONCE)
		{
			NumPerOnce = MAX_NUM_PER_ONCE;
		}

		dim3 threadPerBlock(1, 1);
		dim3 blockPerGrid(1, 1, 1);

		threadPerBlock.x = THREAD_NUM_PER_BLOCK;
		threadPerBlock.y = 1;

		blockPerGrid.x = (NumPerOnce / THREAD_NUM_PER_BLOCK) + ((NumPerOnce % THREAD_NUM_PER_BLOCK) != 0);
		blockPerGrid.y = 1;

		cullingKernel<<< blockPerGrid , threadPerBlock>>> (d_instanceData, NumPerOnce, cameraPos, maxDistanceSquared, d_visibleCount);
		cudaStatus = cudaDeviceSynchronize();
		if(cudaStatus != cudaSuccess)
		{
			return cudaStatus;
		}
		d_instanceData += NumPerOnce;
		numInstances -= NumPerOnce;
	}
	
	return cudaStatus;

}


#pragma once

#include<cuda.h>
#include<cuda_runtime.h>
#include <device_atomic_functions.h>
#include <device_launch_parameters.h>

#include <cuda_d3d11_interop.h>




struct float4_cu {
	float x, y, z, w;
};

struct float2_cu {
	float x, y;
};



 struct cu_VTXMATRIX
{
	 float4_cu  vRight;
	 float4_cu  vUp;
	 float4_cu  vLook;
	 float4_cu  vTranslation;
	 float2_cu vLifeTime;
	float vGravity;
	
};

 struct Cu_VTXMATRIX
 {
	 float4  vRight;
	 float4  vUp;
	 float4  vLook;
	 float4  vTranslation;
	 float2 vLifeTime;
	 float vGravity;
	 float vPadding;

 };
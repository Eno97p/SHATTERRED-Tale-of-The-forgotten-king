#pragma once
#include "VIBuffer.h"
#include"CudaDefines.h"
BEGIN(Engine)
class CVIBuffer_Terrain;
class CComputeShader_Buffer;

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct INSTANCE_DESC
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float3			vPivotPos;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_float2			vGravity;
		_bool			isLoop;
	}INSTANCE_DESC;

	typedef struct INSTANCE_MAP_DESC
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float3			vPivotPos;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_float2			vGravity;
		_bool			isLoop;

		//For Map Elements
		vector<_float4x4*>		WorldMats;
	}INSTANCE_MAP_DESC;



protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC& InstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
	INSTANCE_DESC* Get_Instance_Desc() { return &m_InstanceDesc; }

	


public:
	virtual void Spread(_float fTimeDelta);	//선생님 스프레드 코드 ( 중력, 회전 추가 )
	virtual void Drop(_float fTimeDelta);
	virtual void GrowOut(_float fTimeDelta); //스피드 값에 따라 점점 커지는 함수
	virtual void Spread_Size_Up(_float fTimeDelta);	//스피드에 비례해서 사이즈가 커짐
	virtual void Spread_Non_Rotation(_float fTimeDelta);	//회전 없는 스프레드
	virtual void CreateSwirlEffect(_float fTimeDelta);	//회오리 이펙트 y축 증가할수록 반경이 커짐
	virtual void Spread_Speed_Down(_float fTimeDelta);	//스프레드인데 스피드가 점점 느려짐 중력값에 비례
	virtual void SlashEffect(_float fTimeDelta);
	virtual void Spread_Speed_Down_SizeUp(_float fTimeDelta);
	virtual void Gather(_float fTimeDelta);
	virtual void Extinction(_float fTimeDelta);
	virtual void GrowOutY(_float fTimeDelta);
	virtual void GrowOut_Speed_Down(_float fTimeDelta);
	virtual void GrowOut_Speed_Down_Texture(_float fTimeDelta);
	virtual void Lenz_Flare(_float fTimeDelta);
	virtual void Spiral_Extinction(_float fTimeDelta);
	virtual void Spiral_Expansion(_float fTimeDelta);
	virtual void Leaf_Fall(_float fTimeDelta);
	virtual void Blow(_float fTimeDelta);
	virtual void Up_To_Stop(_float fTimeDelta);
	virtual void Only_Up(_float fTimeDelta);

	//For Grass
	void Initial_RotateY();
	void Initial_RandomOffset(CVIBuffer_Terrain* pTerrain);
	void Initial_InsideCircle(float radius, _float3 pivot);
	vector<_float4x4*> Get_VtxMatrices();
	//virtual void Setup_Onterrain(CVIBuffer_Terrain* pTerrain);
	_bool Check_Instance_Dead() { return m_bInstanceDead; }	//IsLoop가 false일 경우 완전히 끝났는지 확인해주는 함수

public:
	HRESULT Render_Culling() ;

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	_float* m_pSpeeds = { nullptr };
	_float* m_pOriginalSpeed = { nullptr };
	_float3* m_pOriginalPositions = { nullptr };
	_float* m_pOriginalGravity = { nullptr };
	_float* m_pSize = { nullptr };
	_float* m_pOriginalSize = { nullptr };


	_bool						m_bInstanceDead = false;
	INSTANCE_DESC				m_InstanceDesc = {};
	INSTANCE_MAP_DESC			m_InstanceMapDesc = {};


	CComputeShader_Buffer*		m_pComputeShader = nullptr;





private:
//cuda Test----------------------------------------
	class CCuda* m_pCuda = nullptr;
	VTXMATRIX* d_pInstanceData = nullptr;
	int* d_visibleCount = nullptr;
	cudaGraphicsResource* m_cudaResource = nullptr;
public:
	void TestCuda();
//cuda Test----------------------------------------
protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

private:
	_uint m_iVisibleInstances = 0;

public:
	void Culling_Instance(const _float3& cameraPosition, _float maxRenderDistance);

private:

	float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	float Lerp(float t, float a, float b) { return a + t * (b - a); }
	float Grad(int hash, float x, float y, float z)
	{
		int h = hash & 15;
		float u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	float PerlinNoise(float x, float y, float z)
	{
		static const int permutation[] = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};

		static  int p[512] = { 0 };
		static bool initialized = false;
		if (!initialized) {
			for (int i = 0; i < 512; i++) p[i] = permutation[i & 255];
			initialized = true;
		}

		int X = (int)floor(x) & 255,
			Y = (int)floor(y) & 255,
			Z = (int)floor(z) & 255;
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);
		float u = Fade(x),
			v = Fade(y),
			w = Fade(z);
		int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
			B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}
};

END


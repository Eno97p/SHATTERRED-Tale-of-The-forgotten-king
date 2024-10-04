#include "PhysXParticle.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "extensions/PxCudaHelpersExt.h"

CPhysXParticle::CPhysXParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CPhysXParticle::CPhysXParticle(const CPhysXParticle& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CPhysXParticle::Initialize_Prototype(CMesh* m_Meshes, const PhysX_Particle_Desc& InstanceDesc)
{
	m_Owndesc = make_unique<PhysX_Particle_Desc>(InstanceDesc);
	m_iNumInstance = m_Owndesc->iNumInstance;
	if (FAILED(Compute_RandomNumbers()))
		return E_FAIL;
	if (FAILED(Init_Mesh_InstanceBuffer(m_Meshes)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPhysXParticle::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;


	return S_OK;
}

HRESULT CPhysXParticle::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance
	};
	_uint					iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};
	_uint					iOffsets[] = {
		0,
		0
	};
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	return S_OK;
}

HRESULT CPhysXParticle::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CPhysXParticle::Tick(_float fTimeDelta)
{
	_bool InstanceDead = true;
	if (g_ParticleSystem)
	{
		PxU32	numActiveParticles = g_ParticleBuffer->getNbActiveParticles();
		vector<PxVec4> Positions(numActiveParticles);
		vector<PxVec4> velocities(numActiveParticles);

		g_CudaContext->acquireContext();
		PxCudaContext* cudaContext = g_CudaContext->getCudaContext();
		cudaContext->memcpyDtoH(Positions.data(), CUdeviceptr(g_ParticleBuffer->getPositionInvMasses()), sizeof(PxVec4) * numActiveParticles);
		cudaContext->memcpyDtoH(velocities.data(), CUdeviceptr(g_ParticleBuffer->getVelocities()), sizeof(PxVec4) * numActiveParticles);
		g_CudaContext->releaseContext();

		D3D11_MAPPED_SUBRESOURCE		SubResource{};
		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
		VTXPARTICLE* pInstanceData = (VTXPARTICLE*)SubResource.pData;
		for (PxU32 i = 0; i < numActiveParticles; ++i)
		{
			pInstanceData[i].vLifeTime.y += fTimeDelta;
			pInstanceData[i].vTranslation = _float4(Positions[i].x, Positions[i].y, Positions[i].z, 1.0f);
			_float4 direction = { velocities[i].x, velocities[i].y, velocities[i].z, 0.f};

		
			_vector vDir = XMVector4Normalize(XMLoadFloat4(&direction));
			_vector vRight = XMVector4Normalize(XMVector3Cross(vDir, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
			_vector vUp = XMVector4Normalize(XMVector3Cross(vDir, vRight));
			XMStoreFloat4(&pInstanceData[i].vRight, vRight * m_Size[i]);
			XMStoreFloat4(&pInstanceData[i].vUp, vUp * m_Size[i]);
			XMStoreFloat4(&pInstanceData[i].vLook, vDir * m_Size[i]);
			

			if (pInstanceData[i].vLifeTime.y >= pInstanceData[i].vLifeTime.x)
			{
				pInstanceData[i].vLifeTime.y = pInstanceData[i].vLifeTime.x;
			}
			else
				InstanceDead = false;
		}
		m_pContext->Unmap(m_pVBInstance, 0);
	}
	m_InstanceDead = InstanceDead;
}


HRESULT CPhysXParticle::Compute_RandomNumbers()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());
	return S_OK;
}

HRESULT CPhysXParticle::Init_Mesh_InstanceBuffer(CMesh* m_Meshes)
{
	m_iMaterialIndex = m_Meshes->Get_MaterialIndex();
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = m_Meshes->Get_NumVertex();
	m_iIndexStride = 4;
	m_iIndexCountPerInstance = m_Meshes->Get_NumIndices() * 3;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(VTXPARTICLE);
#pragma region VERTEX
	m_iVertexStride = sizeof(VTXMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i], &m_Meshes->Get_Vertices()[i], sizeof(VTXMESH));
	}
	m_InitialData.pSysMem = pVertices;
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);
#pragma endregion VERTEX
#pragma region INDEX
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint      iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_uint iFaceCnt = m_Meshes->Get_NumIndices() / 3;
		_uint      iNumFacesIndices = { 0 };
		for (size_t j = 0; j < iFaceCnt; j++)
		{
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
		}
	}
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);

#pragma endregion INDEX
#pragma region INSTANCE
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXPARTICLE* pInstanceVertices = new VTXPARTICLE[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXPARTICLE) * m_iNumInstance);

	if (FAILED(Init_Particle_System(&pInstanceVertices)))
		return E_FAIL;
	m_InitialData.pSysMem = pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);
#pragma endregion INSTANCE
	return S_OK;
}

HRESULT CPhysXParticle::Init_Particle_System(VTXPARTICLE** Buffer)
{
	g_Scene = m_pGameInstance->GetScene();
	g_PhysXs = m_pGameInstance->GetPhysics();
	g_CudaContext = g_Scene->getCudaContextManager();

	PxU32 maxParticles = m_iNumInstance;
	const PxReal restOffset = 0.5f * m_Owndesc->Size.x / 0.6f;

	
	PxPBDMaterial* DefaultMat = g_PhysXs->createPBDMaterial(0.05f, 0.05f, 0.f, 0.001f, 0.5f, 0.005f, 0.01f, 0.f, 0.f);
	DefaultMat->setViscosity(m_Owndesc->Viscosity);
	DefaultMat->setSurfaceTension(m_Owndesc->SurfaceTension);
	DefaultMat->setCohesion(m_Owndesc->Cohesion);
	DefaultMat->setVorticityConfinement(m_Owndesc->VorticityConfinement);
	DefaultMat->setFriction(m_Owndesc->Friction);
	DefaultMat->setDamping(m_Owndesc->Damping);

	PxPBDParticleSystem* particleSystem = g_PhysXs->createPBDParticleSystem(*g_CudaContext, 96);
	g_ParticleSystem = particleSystem;

	const PxReal fluidDensity = 1000.0f;
	const PxReal solidRestOffset = restOffset;
	const PxReal fluidRestOffset = restOffset * 0.6f;
	const PxReal particleMass = fluidDensity * 1.333f * 3.14159f * m_Owndesc->Size.x * m_Owndesc->Size.x * m_Owndesc->Size.x;
	particleSystem->setRestOffset(restOffset);
	particleSystem->setContactOffset(restOffset * m_Owndesc->ContactOffset);
	particleSystem->setParticleContactOffset(fluidRestOffset / 0.6f);
	particleSystem->setSolidRestOffset(solidRestOffset);
	particleSystem->setFluidRestOffset(fluidRestOffset);
	particleSystem->setParticleFlag(PxParticleFlag::eENABLE_SPECULATIVE_CCD, false);
	particleSystem->setMaxVelocity(solidRestOffset * 100.f);

	g_Scene->addActor(*particleSystem);

	PxDiffuseParticleParams dpParams;
	dpParams.threshold = m_Owndesc->Threshold;
	dpParams.bubbleDrag = m_Owndesc->BubbleDrag;
	dpParams.buoyancy = m_Owndesc->Buoyancy;
	dpParams.airDrag = m_Owndesc->AirDrag;
	dpParams.kineticEnergyWeight = m_Owndesc->Energy;
	dpParams.pressureWeight = m_Owndesc->PressureWeight;
	dpParams.divergenceWeight = m_Owndesc->DivergenceWeight;
	dpParams.lifetime = m_Owndesc->LifeTime.y;
	dpParams.useAccurateVelocity = false;
	
	const PxU32 particlePhase = particleSystem->createPhase(DefaultMat, PxParticlePhaseFlags(/*PxParticlePhaseFlag::eParticlePhaseFluid |*/ PxParticlePhaseFlag::eParticlePhaseSelfCollide));

	PxU32* phase = PX_EXT_PINNED_MEMORY_ALLOC(PxU32, *g_CudaContext, maxParticles);
	PxVec4* positionInvMass = PX_EXT_PINNED_MEMORY_ALLOC(PxVec4, *g_CudaContext, maxParticles);
	PxVec4* velocity = PX_EXT_PINNED_MEMORY_ALLOC(PxVec4, *g_CudaContext, maxParticles);

#pragma region Random
	uniform_real_distribution<float>	RangeX(m_Owndesc->Pivot.x - m_Owndesc->Range.x * 0.5f, m_Owndesc->Pivot.x + m_Owndesc->Range.x * 0.5f);
	uniform_real_distribution<float>	RangeY(m_Owndesc->Pivot.y - m_Owndesc->Range.y * 0.5f, m_Owndesc->Pivot.y + m_Owndesc->Range.y * 0.5f);
	uniform_real_distribution<float>	RangeZ(m_Owndesc->Pivot.z - m_Owndesc->Range.z * 0.5f, m_Owndesc->Pivot.z + m_Owndesc->Range.z * 0.5f);

	uniform_real_distribution<float>	Size(m_Owndesc->Size.x, m_Owndesc->Size.y);
	uniform_real_distribution<float>	Velocity(m_Owndesc->Velocity.x, m_Owndesc->Velocity.y);
	uniform_real_distribution<float>	LifeTime(m_Owndesc->LifeTime.x, m_Owndesc->LifeTime.y);
#pragma endregion Random
	m_Size = new _float[m_iNumInstance];
	ZeroMemory(m_Size, sizeof(_float) * m_iNumInstance);

	for (PxU32 i = 0; i < m_iNumInstance; i++)
	{
		(*Buffer)[i].vLifeTime.y = 0.f;
		(*Buffer)[i].vLifeTime.x = LifeTime(m_RandomNumber);
		m_Size[i] = Size(m_RandomNumber);
		(*Buffer)[i].vRight = _float4(m_Size[i], 0.f, 0.f, 0.f);
		(*Buffer)[i].vUp = _float4(0.f, m_Size[i], 0.f, 0.f);
		(*Buffer)[i].vLook = _float4(0.f, 0.f, m_Size[i], 0.f);
		(*Buffer)[i].vTranslation = _float4(RangeX(m_RandomNumber), RangeY(m_RandomNumber), RangeZ(m_RandomNumber), 1.f);
		phase[i] = particlePhase;
		positionInvMass[i] = PxVec4((*Buffer)[i].vTranslation.x, (*Buffer)[i].vTranslation.y, (*Buffer)[i].vTranslation.z, 1.0f / particleMass);
		XMVECTOR vDir = XMVector4Normalize(XMLoadFloat4(&(*Buffer)[i].vTranslation) - XMLoadFloat3(&m_Owndesc->Offset)) * Velocity(m_RandomNumber);
		velocity[i] = PxVec4(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir),0.f);
	}
	
	PxParticleAndDiffuseBufferDesc bufferDesc;
	bufferDesc.maxParticles = maxParticles;
	bufferDesc.numActiveParticles = maxParticles;
	bufferDesc.maxDiffuseParticles = maxParticles;
	bufferDesc.maxActiveDiffuseParticles = maxParticles;
	bufferDesc.positions = positionInvMass;
	bufferDesc.diffuseParams = dpParams;
	bufferDesc.velocities = velocity;
	bufferDesc.phases = phase;

	g_ParticleBuffer = PxCreateAndPopulateParticleAndDiffuseBuffer(bufferDesc, g_CudaContext);
	g_ParticleSystem->addParticleBuffer(g_ParticleBuffer);

	
	PX_EXT_PINNED_MEMORY_FREE(*g_CudaContext, positionInvMass);
	PX_EXT_PINNED_MEMORY_FREE(*g_CudaContext, velocity);
	PX_EXT_PINNED_MEMORY_FREE(*g_CudaContext, phase);

	return S_OK;
}

//PxParticleBuffer* CPhysXParticle::CreateParticleBuffer(const PxParticleBufferDesc& desc, PxParticleBuffer* particleBuffer)
//{
//#if PX_SUPPORT_GPU_PHYSX
//
//	PxParticleBuffer* tmpparticleBuffer = g_PhysXs->createParticleBuffer(desc.maxParticles, desc.maxVolumes, g_CudaContext);
//
//	PxVec4* positionBuffer = tmpparticleBuffer->getPositionInvMasses();
//	PxVec4* velocityBuffer = tmpparticleBuffer->getVelocities();
//	PxU32* phaseBuffer = tmpparticleBuffer->getPhases();
//	PxParticleVolume* volumeBuffer = tmpparticleBuffer->getParticleVolumes();
//
//	g_CudaContext->getCudaContext()->memcpyHtoDAsync(CUdeviceptr(positionBuffer), desc.positions, desc.numActiveParticles * sizeof(PxVec4), 0);
//	g_CudaContext->getCudaContext()->memcpyHtoDAsync(CUdeviceptr(velocityBuffer), desc.velocities, desc.numActiveParticles * sizeof(PxVec4), 0);
//	g_CudaContext->getCudaContext()->memcpyHtoDAsync(CUdeviceptr(phaseBuffer), desc.phases, desc.numActiveParticles * sizeof(PxU32), 0);
//	g_CudaContext->getCudaContext()->memcpyHtoDAsync(CUdeviceptr(volumeBuffer), desc.volumes, desc.numVolumes * sizeof(PxParticleVolume), 0);
//
//	tmpparticleBuffer->setNbActiveParticles(desc.numActiveParticles);
//	tmpparticleBuffer->setNbParticleVolumes(desc.numVolumes);
//
//	g_CudaContext->getCudaContext()->streamSynchronize(0);
//
//#else
//	PX_UNUSED(cudaContextManager);
//	PX_UNUSED(particleBuffer);
//	PX_UNUSED(desc);
//#endif
//
//	return tmpparticleBuffer;
//}

void CPhysXParticle::PxDmaDataToDevice(PxCudaContextManager* cudaContextManager, PxParticleBuffer* particleBuffer, const PxParticleBufferDesc& desc)
{
#if PX_SUPPORT_GPU_PHYSX
	cudaContextManager->acquireContext();

	PxVec4* posInvMass = particleBuffer->getPositionInvMasses();
	PxVec4* velocities = particleBuffer->getVelocities();
	PxU32* phases = particleBuffer->getPhases();
	PxParticleVolume* volumes = particleBuffer->getParticleVolumes();

	PxCudaContext* cudaContext = cudaContextManager->getCudaContext();

	//KS - TODO - use an event to wait for this
	cudaContext->memcpyHtoDAsync(CUdeviceptr(posInvMass), desc.positions, desc.numActiveParticles * sizeof(PxVec4), 0);
	cudaContext->memcpyHtoDAsync(CUdeviceptr(velocities), desc.velocities, desc.numActiveParticles * sizeof(PxVec4), 0);
	cudaContext->memcpyHtoDAsync(CUdeviceptr(phases), desc.phases, desc.numActiveParticles * sizeof(PxU32), 0);
	cudaContext->memcpyHtoDAsync(CUdeviceptr(volumes), desc.volumes, desc.numVolumes * sizeof(PxParticleVolume), 0);

	particleBuffer->setNbActiveParticles(desc.numActiveParticles);
	particleBuffer->setNbParticleVolumes(desc.numVolumes);

	cudaContext->streamSynchronize(0);

	cudaContextManager->releaseContext();
#else
	PX_UNUSED(cudaContextManager);
	PX_UNUSED(particleBuffer);
	PX_UNUSED(desc);
#endif
}

PxParticleAndDiffuseBuffer* CPhysXParticle::PxCreateAndPopulateParticleAndDiffuseBuffer(const PxParticleAndDiffuseBufferDesc& desc, PxCudaContextManager* cudaContextManager)
{
	PxParticleAndDiffuseBuffer* particleBuffer = PxGetPhysics().createParticleAndDiffuseBuffer(desc.maxParticles, desc.maxVolumes, desc.maxDiffuseParticles, cudaContextManager);
	PxDmaDataToDevice(cudaContextManager, particleBuffer, desc);
	particleBuffer->setMaxActiveDiffuseParticles(desc.maxActiveDiffuseParticles);
	return particleBuffer;
}

CPhysXParticle* CPhysXParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CMesh* m_Meshes, const PhysX_Particle_Desc& InstanceDesc)
{
	CPhysXParticle* pInstance = new CPhysXParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(m_Meshes, InstanceDesc)))
	{
		MSG_BOX("Failed To Created : CPhysXParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysXParticle::Clone(void* pArg)
{
	CPhysXParticle* pInstance = new CPhysXParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPhysXParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysXParticle::Free()
{
	__super::Free();
	Safe_physX_Release(g_ParticleBuffer);
	Safe_physX_Release(g_ParticleSystem);
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_Size);
}

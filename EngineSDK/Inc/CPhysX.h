#pragma once
#include"Base.h"




BEGIN(Engine)
class CSimulationCallBack;

class CPhysX final : public CBase
{


private:
	CPhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX() = default;



public:
	HRESULT Initialize();


	void Tick(_float fTimeDelta);


public:
	HRESULT AddActor(PxActor* pActor);

public:
	PxPhysics* GetPhysics()const  { return m_pPhysics; }
	PxScene* GetScene()const { return m_pScene; }
	PxControllerManager* GetControllerManager()const { return m_pControllerManager; }
	static PxFilterFlags FilterShaderExample(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

	PxParticleBuffer* CreateParticleBuffer(const PxParticleBufferDesc& desc, PxParticleBuffer* particleBuffer);



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



private:
	PxMaterial*				m_pMaterial = { nullptr };
	PxFoundation*			m_pFoundation = { nullptr };
	PxPhysics*				m_pPhysics = { nullptr };
	PxDefaultCpuDispatcher*	m_pCPUDispatcher = { nullptr };
	PxScene*				m_pScene = { nullptr };
	PxPvd*					m_pPvd = { nullptr };
	PxCudaContextManager*	m_pCudaContextManager = { nullptr };
	PxControllerManager*	m_pControllerManager = { nullptr };
	CSimulationCallBack*	m_pSimulationCallBack = { nullptr };
	PxPBDParticleSystem*	m_pParticleSystem;
public:
	static CPhysX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};



END

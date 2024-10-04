#include "stdafx.h"

#include"GameInstance.h"
#include"CWorker.h"
#include "SavePoint.h"

#include "UI_Activate.h"
#include "UI_FadeInOut.h"
#include "EffectManager.h"






CSavePoint::CSavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSavePoint::CSavePoint(const CSavePoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSavePoint::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CSavePoint::Initialize(void* pArg)
{
	_tagSavePoint_Desc* pDesc = (_tagSavePoint_Desc*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(pDesc->vPosition.x, pDesc->vPosition.y, pDesc->vPosition.z,1.0f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	if(FAILED(Create_Activate()))
		return E_FAIL;

	EFFECTMGR->Generate_WellCylinder(m_pTransformCom->Get_WorldFloat4x4());
	EFFECTMGR->Generate_Particle(117, _float4(pDesc->vPosition.x, pDesc->vPosition.y, pDesc->vPosition.z, 1.0f));
	return S_OK;
}

void CSavePoint::Priority_Tick(_float fTimeDelta)
{
}

void CSavePoint::Tick(_float fTimeDelta)
{
	m_pActivateUI->Tick(fTimeDelta);
		




	vector<future<void>> futures;

	//for (int i = 0; i < 8; i++)
	//{
	//	this->Test();
	//}


	
	
		//futures.push_back(m_pGameInstance->AddWork([this]() {
		//	for (int i = 0; i < 8; i++)
		//	{
		//		this->Test();
		//	}
		//}));
	
	
	

	//futures.push_back(m_pGameInstance->AddWork([this]() {
	//	this->Test();
	//}));


	/*m_pGameInstance->AddWork([this]() {
		this->Test();
	});*/
}

void CSavePoint::Late_Tick(_float fTimeDelta)
{

	CComponent* pComponent = m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform"));
	CTransform* pPlayerTransformCom = nullptr;
	if (pComponent == nullptr)
		return;

	pPlayerTransformCom = dynamic_cast<CTransform*>(pComponent);


	_vector vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSavePointPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVectorGetX(XMVector4Length(vPlayerPos - vSavePointPos));


	if (fabs(fDistance) < 5.0f)
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (KEY_TAP(DIK_F))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("SavePoint.mp3"), SOUND_SAVEPOINT);
			_float4 vParticlePos;
			XMStoreFloat4(&vParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Particle(119, vParticlePos);
			vParticlePos.y += 1.f;
			EFFECTMGR->Generate_Particle(118, vParticlePos, nullptr, XMVectorSet(1.f,0.f,0.f,0.f), 90.f);

			CUI_FadeInOut::UI_FADEINOUT_DESC pDesc = {};
			pDesc.isFadeIn = false; // true
			pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;
			pDesc.isLevelChange = false;

			LEVEL eCurLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();
			if (FAILED(m_pGameInstance->Add_CloneObject(eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
				return;
			

			//vector<future<void>> vecfutures;
			//비동기 적으로 현재 비석의 위치 저장

			
			auto futures = m_pGameInstance->AddWork([this, eCurLevel]() {
				Engine::Save_Data(L"../Bin/DataFiles/SavePoint.bin", true, m_pTransformCom->Get_State(CTransform::STATE_POSITION), eCurLevel);
				
			});
			
			if (futures.wait_for(chrono::milliseconds(1000)) == future_status::ready)
			{
				Client::g__Exit_Delete_FileList.insert(L"../Bin/DataFiles/SavePoint.bin");
				int tes = 0;
				//작업이 끝났을 때 불림
			}
			


		}
		
	}


	if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.0f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);

#ifdef _DEBUG

		m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif // _DEBUG


	}
}

HRESULT CSavePoint::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNummeshCount = m_pModelCom->Get_NumMeshes();


	for (size_t i = 0; i < iNummeshCount; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;





		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}



#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CSavePoint::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNummeshCount = m_pModelCom->Get_NumMeshes();


		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", 0, aiTextureType_DIFFUSE)))
			return E_FAIL;



		m_pShaderCom->Begin(12);

		m_pModelCom->Render(0);





	return S_OK;
}

HRESULT CSavePoint::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Well"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CPhysXComponent::PHYSX_DESC      PhysXDesc{};
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
	PhysXDesc.pName = "SavePoint";
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	//XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	/* For.Com_PhysX */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_static_For_Well"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSavePoint::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

HRESULT CSavePoint::Create_Activate()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	m_pActivateUI = dynamic_cast<CUI_Activate*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Activate"), &pDesc));
	if (nullptr == m_pActivateUI)
		return E_FAIL;



	return S_OK;
}

void CSavePoint::Test()
{
	int a = 0;
	for (int i = 0; i < 1000000; i++)
	{
		a += i;
	}

}

CSavePoint* CSavePoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSavePoint* pInstance = new CSavePoint(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSavePoint");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
    
}

CGameObject* CSavePoint::Clone(void* pArg)
{
	CSavePoint* pInstance = new CSavePoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSavePoint");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSavePoint::Free()
{
	__super::Free();
	Safe_Release(m_pActivateUI);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysXCom);
}

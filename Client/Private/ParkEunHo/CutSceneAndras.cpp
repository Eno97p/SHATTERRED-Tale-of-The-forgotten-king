#include "CutSceneAndras.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "EffectManager.h"

#include "Particle.h"
#include "CutSceneCamera.h"

CutSceneAndras::CutSceneAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CutSceneAndras::CutSceneAndras(const CutSceneAndras& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CutSceneAndras::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CutSceneAndras::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	
	m_OwnDesc = make_unique<CUTSCENEANDRAS>(*((CUTSCENEANDRAS*)pArg));

	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 5.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	m_pTransformCom->LookAt_For_LandObject(m_pGameInstance->Get_CamPosition());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vPosition));

	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Andras-Head");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	return S_OK;
}

void CutSceneAndras::Priority_Tick(_float fTimeDelta)
{

}

void CutSceneAndras::Tick(_float fTimeDelta)
{
	m_AccTime += fTimeDelta;
	if (m_AccTime > m_OwnDesc->fLifeTime)
	{
		m_pGameInstance->Erase(this);
	}

	if (m_AccTime > m_OwnDesc->EyeChangeBetwin.x && m_AccTime < m_OwnDesc->EyeChangeBetwin.y)
	{
		float range = m_OwnDesc->EyeChangeBetwin.y - m_OwnDesc->EyeChangeBetwin.x;
		float timeSinceStart = m_AccTime - m_OwnDesc->EyeChangeBetwin.x;
		m_EyechangeRatio = timeSinceStart / range;
		m_EyechangeRatio = max(0.f, min(m_EyechangeRatio, 1.f));

		if (!m_EyeChanged)
		{
			_float4 vstartPos;
			XMStoreFloat4(&vstartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameObject* Oura = EFFECTMGR->Generate_Particle(94, vstartPos, this);
			m_Particles.emplace_back(Oura);
			CGameObject* Oura2 = EFFECTMGR->Generate_Particle(94, vstartPos, this, XMVectorSet(0.f, 1.f, 0.f, 0.f), 90.f);
			m_Particles.emplace_back(Oura2);
			m_EyeChanged = true;
		}
	}
	else if (m_AccTime >= m_OwnDesc->EyeChangeBetwin.y)
	{
		m_EyechangeRatio = 1.f;
	}
	else
		m_EyechangeRatio = 0.f;

	if (!m_EffectSpawned && m_AccTime > m_OwnDesc->fStartEffect)
	{
		m_EffectSpawned = true;
		Add_Child_Effects();
	}


	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pTransformCom->LookAt_For_LandObject(m_pGameInstance->Get_CamPosition());


}

void CutSceneAndras::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	if(m_EyeChanged)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);


}

HRESULT CutSceneAndras::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;


		if (i > 0 && i != 3)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}


		if (i != 3)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin(13);
		

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CutSceneAndras::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;

	m_pShaderCom->Unbind_SRVs();
	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 3);
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", 3, aiTextureType_DIFFUSE)))
		return E_FAIL;

	m_pShaderCom->Begin(14);
	m_pModelCom->Render(3);



	return S_OK;
}




HRESULT CutSceneAndras::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Cut"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	return S_OK;
}

HRESULT CutSceneAndras::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("EyeColor", &m_OwnDesc->vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EyeRatio", &m_EyechangeRatio, sizeof(_float))))
		return E_FAIL;

#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CutSceneAndras::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("EyeColor", &m_OwnDesc->vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EyeRatio", &m_EyechangeRatio, sizeof(_float))))
		return E_FAIL;

#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CutSceneAndras::Add_Child_Effects()
{
	dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->StartIntenseShaking(2.f, 2.5f);

	m_pGameInstance->Disable_Echo();
	m_pGameInstance->Play_Effect_Sound(TEXT("Andras_Phase2.mp3"), SOUND_EFFECT, 0.f, 1.f, 1.f);

	_float4 ParticlePos;
	XMStoreFloat4(&ParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	EFFECTMGR->Generate_Particle(125, ParticlePos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 180.f);
	EFFECTMGR->Generate_Particle(126, ParticlePos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), -90.f);
	EFFECTMGR->Generate_Particle(127, ParticlePos);
	EFFECTMGR->Generate_Particle(128, ParticlePos);

	m_OwnDesc->Cylinder.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->AndrasPillar.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->AndrasPillar.fMaxLifeTime = m_OwnDesc->Cylinder.fMaxLifeTime;
	m_OwnDesc->AndrasPillar.fThreadRatio = m_OwnDesc->Cylinder.fThreadRatio;
	m_OwnDesc->AndrasPillar.fSlowStrength = m_OwnDesc->Cylinder.fSlowStrength;
	m_OwnDesc->AndrasPillar.vOffset = m_OwnDesc->Cylinder.vOffset;

	m_OwnDesc->SphereDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->SphereDesc.vOffset = m_OwnDesc->Cylinder.vOffset;



	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_DefaultCylinder"), &m_OwnDesc->Cylinder);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_AndrasPillar"), &m_OwnDesc->AndrasPillar);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_AndrasSphere"), &m_OwnDesc->SphereDesc);




	return S_OK;
}

CutSceneAndras* CutSceneAndras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CutSceneAndras* pInstance = new CutSceneAndras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CutSceneAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CutSceneAndras::Clone(void* pArg)
{
	CutSceneAndras* pInstance = new CutSceneAndras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CutSceneAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CutSceneAndras::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (auto& iter : m_Particles)
	{
		static_cast<CParticle*>(iter)->Set_Target(nullptr);
		static_cast<CParticle*>(iter)->Set_Delete();
	}
	m_Particles.clear();
}

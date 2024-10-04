#include "Particle_PhysX.h"
#include "GameInstance.h"

CParticle_PhysX::CParticle_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject(pDevice, pContext)
{
}

CParticle_PhysX::CParticle_PhysX(const CParticle_PhysX& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CParticle_PhysX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_PhysX::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	OwnDesc = make_unique<PARTICLE_PHYSXDESC>(*((PARTICLE_PHYSXDESC*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	switch (OwnDesc->eModelType)
	{
	case CUBE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Cube");
		break;
	case CIRCLE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Circle");
		break;
	case SLASH:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Slash");
		break;
	case LEAF0:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Leaf0");
		break;
	case LEAF1:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Leaf1");
		break;
	case BLADE_SLASH:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Blade_Slash");
		break;
	case BLADE_SLASH_LONG:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Blade_Slash_Long");
		break;
	case GRASS:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_GrassParticle");
		break;
	case ROCK0:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_RockParticle1");
		break;
	case ROCK1:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_RockParticle2");
		break;
	case NEEDLE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Needle");
		break;
	case BUBBLE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Bubble_Mesh");
		break;
	}

	if (FAILED(Add_Components(m_ModelPrototypeTag)))
		return E_FAIL;
	
	m_InstModelCom->Ready_PhysXParticle(OwnDesc->PhysXDesc);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&OwnDesc->vStartPos));
	return S_OK;
}

void CParticle_PhysX::Priority_Tick(_float fTimeDelta)
{
}

void CParticle_PhysX::Tick(_float fTimeDelta)
{
	if (m_InstModelCom->Check_PhysX_Dead())
		m_pGameInstance->Erase(this);
	m_InstModelCom->PhysX_Particle_Simulate(fTimeDelta);
}

void CParticle_PhysX::Late_Tick(_float fTimeDelta)
{
	
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	if (OwnDesc->bValues[BoolValue::BOOL_BLOOM])
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CParticle_PhysX::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_PhysX(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;


		m_pShaderCom->Begin(0);

		m_InstModelCom->Render_PhysXInstance(i);
	}

	return S_OK;
}

HRESULT CParticle_PhysX::Render_Bloom()
{
	if (FAILED(Bind_BlurResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_PhysX(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_InstModelCom->Render_PhysXInstance(i);
	}

	return S_OK;
}



HRESULT CParticle_PhysX::Add_Components(const wstring& strModelPrototype)
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DesolveTexture"), reinterpret_cast<CComponent**>(&m_pDesolveTexture))))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelPrototype,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_InstModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_PhysX::Bind_ShaderResources()
{
	//enum BoolValue { BOOL_ALPHA, BOOL_COLORMAP, BOOL_BLOOM, BOOL_DESOLVE, BOOL_END};
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->bValues[BoolValue::BOOL_DESOLVE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->NumDesolve)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->bValues[BoolValue::BOOL_COLORMAP], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &OwnDesc->vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &OwnDesc->vEndColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->bValues[BoolValue::BOOL_ALPHA], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->fDesolveLength, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle_PhysX::Bind_BlurResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BlurPower", &OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->bValues[BoolValue::BOOL_DESOLVE], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->NumDesolve)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->bValues[BoolValue::BOOL_COLORMAP], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &OwnDesc->vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &OwnDesc->vEndColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->bValues[BoolValue::BOOL_ALPHA], sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->fDesolveLength, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CParticle_PhysX* CParticle_PhysX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_PhysX* pInstance = new CParticle_PhysX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CParticle_PhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_PhysX::Clone(void* pArg)
{
	CParticle_PhysX* pInstance = new CParticle_PhysX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticle_PhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_PhysX::Free()
{
	__super::Free();
	Safe_Release(m_InstModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDesolveTexture);
}

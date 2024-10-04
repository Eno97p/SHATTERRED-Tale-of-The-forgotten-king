#include "ParticleMesh.h"
#include "GameInstance.h"
#include "stdafx.h"

CParticleMesh::CParticleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CParticle(pDevice, pContext)
{
}

CParticleMesh::CParticleMesh(const CParticleMesh& rhs)
	: CParticle(rhs)
{
}

HRESULT CParticleMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleMesh::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	PARTICLEMESH* pDesc = reinterpret_cast<PARTICLEMESH*>(pArg);
	OwnDesc = make_shared<PARTICLEMESH>(*pDesc);

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

	
	m_InstModelCom->Ready_Instance(OwnDesc->SuperDesc.InstanceDesc);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&OwnDesc->SuperDesc.vStartPos));
	return S_OK;
}

void CParticleMesh::Priority_Tick(_float fTimeDelta)
{
}

void CParticleMesh::Tick(_float fTimeDelta)
{
	if (m_InstModelCom->Check_Instance_Dead() || m_Delete == true)
	{
		m_pGameInstance->Erase(this);
		return;
	}
	if (m_pTarget != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_State(CTransform::STATE_POSITION));
	}

	switch (OwnDesc->SuperDesc.eType)
	{
	case SPREAD:
		m_InstModelCom->Spread(fTimeDelta);
		break;
	case DROP:
		m_InstModelCom->Drop(fTimeDelta);
		break;
	case GROWOUT:
		m_InstModelCom->GrowOut(fTimeDelta);
		break;
	case SPREAD_SIZEUP:
		m_InstModelCom->Spread_Size_Up(fTimeDelta);
		break;
	case SPREAD_NONROTATION:
		m_InstModelCom->Spread_Non_Rotation(fTimeDelta);
		break;
	case TORNADO:
		m_InstModelCom->CreateSwirlEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN:
		m_InstModelCom->Spread_Speed_Down(fTimeDelta);
		break;
	case SLASH_EFFECT:
		m_InstModelCom->SlashEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN_SIZE_UP:
		m_InstModelCom->Spread_Speed_Down_SizeUp(fTimeDelta);
		break;
	case GATHER:
		m_InstModelCom->Gather(fTimeDelta);
		break;
	case EXTINCTION:
		m_InstModelCom->Extinction(fTimeDelta);
		break;
	case GROWOUTY:
		m_InstModelCom->GrowOutY(fTimeDelta);
		break;
	case GROWOUT_SPEEDDOWN:
		m_InstModelCom->GrowOut_Speed_Down(fTimeDelta);
		break;
	case LEAF_FALL:
		m_InstModelCom->Leaf_Fall(fTimeDelta);
		break;
	case SPIRAL_EXTINCTION:
		m_InstModelCom->Spiral_Extinction(fTimeDelta);
		break;
	case SPIRAL_SPERAD:
		m_InstModelCom->Spiral_Expansion(fTimeDelta);
		break;
	case LENZ_FLARE:
		m_InstModelCom->Lenz_Flare(fTimeDelta);
		break;
	case BLOW:
		m_InstModelCom->Blow(fTimeDelta);
		break;
	case UP_TO_STOP:
		m_InstModelCom->Up_To_Stop(fTimeDelta);
		break;
	case ONLY_UP:
		m_InstModelCom->Only_Up(fTimeDelta);
		break;
	}
}

void CParticleMesh::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (OwnDesc->SuperDesc.IsBlur)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLUR, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);

	if (OwnDesc->SuperDesc.IsBloom)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CParticleMesh::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_Instance(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (OwnDesc->eModelType >= BLADE_SLASH && OwnDesc->eModelType <= BLADE_SLASH_LONG)
			m_pShaderCom->Begin(2);
		else
			m_pShaderCom->Begin(0);

		m_InstModelCom->Render_Instance(i);
	}

	return S_OK;
}

HRESULT CParticleMesh::Render_Bloom()
{
	if (FAILED(Bind_BlurResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_Instance(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (OwnDesc->eModelType >= BLADE_SLASH && OwnDesc->eModelType <= BLADE_SLASH_LONG)
			m_pShaderCom->Begin(3);
		else
			m_pShaderCom->Begin(1);

		m_InstModelCom->Render_Instance(i);
	}

	return S_OK;
}

HRESULT CParticleMesh::Render_Blur()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_Instance(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (OwnDesc->eModelType >= BLADE_SLASH && OwnDesc->eModelType <= BLADE_SLASH_LONG)
			m_pShaderCom->Begin(2);
		else
			m_pShaderCom->Begin(0);

		m_InstModelCom->Render_Instance(i);
	}

	return S_OK;
}

HRESULT CParticleMesh::Add_Components(const wstring& strModelPrototype)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelPrototype,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_InstModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->SuperDesc.Desolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->SuperDesc.DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->SuperDesc.vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->SuperDesc.IsColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &OwnDesc->SuperDesc.vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &OwnDesc->SuperDesc.vEndColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->SuperDesc.IsAlpha, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->SuperDesc.fDesolveLength, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticleMesh::Bind_BlurResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->SuperDesc.Desolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->SuperDesc.DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->SuperDesc.vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BlurPower", &OwnDesc->SuperDesc.fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomColor", &OwnDesc->SuperDesc.vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->SuperDesc.IsAlpha, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->SuperDesc.fDesolveLength, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CParticleMesh* CParticleMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleMesh* pInstance = new CParticleMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CParticleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticleMesh::Clone(void* pArg)
{
	CParticleMesh* pInstance = new CParticleMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleMesh::Free()
{
	__super::Free();

	Safe_Release(m_InstModelCom);
	OwnDesc.reset();
}

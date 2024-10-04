#include "AndrasRain.h"
#include "GameInstance.h"

CAndrasRain::CAndrasRain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CAndrasRain::CAndrasRain(const CAndrasRain& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CAndrasRain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrasRain::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<ANDRAS_RAIN_DESC>(*((ANDRAS_RAIN_DESC*)pArg));
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->fRotationSpeed);
	desc.fSpeedPerSec = 0.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;


	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_CurrentSize = { 0.f,0.f,0.f };
	return S_OK;
}

void CAndrasRain::Priority_Tick(_float fTimeDelta)
{
}

void CAndrasRain::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fLifeTimeRatio < 0.2f)
	{
		_vector CurSize = XMLoadFloat3(&m_CurrentSize);
		_vector LerpSize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		CurSize = XMVectorLerp(CurSize, LerpSize, 0.5f);
		XMStoreFloat3(&m_CurrentSize, CurSize);
	}

	m_pTransformCom->Set_Scale(m_CurrentSize.x, m_CurrentSize.y, m_CurrentSize.z);
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
}

void CAndrasRain::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CAndrasRain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(5);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CAndrasRain::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(5);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CAndrasRain::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(5);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CAndrasRain::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras_Lazer_Cylinder"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AndrasRain"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndrasRain::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CAndrasRain* CAndrasRain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrasRain* pInstance = new CAndrasRain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndrasRain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAndrasRain::Clone(void* pArg)
{
	CAndrasRain* pInstance = new CAndrasRain(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndrasRain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAndrasRain::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	for (auto& iter : m_pTextureCom)
		Safe_Release(iter);
}

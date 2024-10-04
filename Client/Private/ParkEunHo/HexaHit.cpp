#include "HexaHit.h"
#include "GameInstance.h"
CHexaHit::CHexaHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CHexaHit::CHexaHit(const CHexaHit& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CHexaHit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHexaHit::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	
	m_OwnDesc = make_shared<HEXASHIELDHIT>(*((HEXASHIELDHIT*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);
	return S_OK;
}

void CHexaHit::Priority_Tick(_float fTimeDelta)
{
}

void CHexaHit::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fCurLifeTime > m_OwnDesc->fLifeTime)
	{
		m_pGameInstance->Erase(this);
	}

	_vector StartScale = XMLoadFloat3(&m_OwnDesc->vSize);
	_vector EndScale = XMLoadFloat3(&m_OwnDesc->vMaxSize);
	_vector ResultScale = XMVectorLerp(StartScale, EndScale, m_fLifeTimeRatio);


	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	ParentMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), ParentMat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
	m_pTransformCom->Set_Scale(XMVectorGetX(ResultScale), XMVectorGetY(ResultScale), XMVectorGetZ(ResultScale));
}

void CHexaHit::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CHexaHit::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
			return E_FAIL;
		m_pShaderCom->Begin(35);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CHexaHit::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HexaShield"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHexaHit::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CHexaHit* CHexaHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHexaHit* pInstance = new CHexaHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHexaHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHexaHit::Clone(void* pArg)
{
	CHexaHit* pInstance = new CHexaHit(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHexaHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHexaHit::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

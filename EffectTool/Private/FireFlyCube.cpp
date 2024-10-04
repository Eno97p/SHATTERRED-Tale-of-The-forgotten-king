#include "FireFlyCube.h"
#include "GameInstance.h"
CFireFlyCube::CFireFlyCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CFireFlyCube::CFireFlyCube(const CFireFlyCube& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CFireFlyCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFireFlyCube::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_unique<FIREFLYCUBE>(*((FIREFLYCUBE*)pArg));
	GAMEOBJECT_DESC GDesc{};
	GDesc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->fRotSpeed);
	if (FAILED(__super::Initialize(&GDesc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix ParentMat;
	ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_OwnDesc->fStartRotAngle));
	m_pTransformCom->Set_Scale(m_OwnDesc->fMinSize.x, m_OwnDesc->fMinSize.y, m_OwnDesc->fMinSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), ParentMat));

	return S_OK;
}

void CFireFlyCube::Priority_Tick(_float fTimeDelta)
{

}

void CFireFlyCube::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fCurLifeTime > m_OwnDesc->fLifeTime)			//라이프타임 다하면 삭제
	{
		m_pGameInstance->Erase(this);
	}


	if (m_fCurLifeTime < m_OwnDesc->fStartdelay)						//처음 시작할때 사이즈 키우는 로직
	{
		_float t = m_fCurLifeTime / m_OwnDesc->fStartdelay;
		t = max(0.f, min(t, 1.f));
		_vector Minsize = XMLoadFloat3(&m_OwnDesc->fMinSize);
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->fMaxSize);
		_vector ResultSize = XMVectorLerp(Minsize, Maxsize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}

	if (m_fLifeTimeRatio > m_OwnDesc->fBloomIntervalStartRatio)			 //일정 시간 지나면 깜빡거리는 로직
	{
		if (m_BloomChange)
			m_BloomPower += fTimeDelta * 2;
		else
			m_BloomPower -= fTimeDelta * 2;

		m_BloomPower = max(0.f, min(m_BloomPower, 1.f));

		m_BloomTick -= fTimeDelta;
		if (m_BloomTick < 0.f)
		{
			m_BloomChange = !m_BloomChange;
			m_BloomTick = m_OwnDesc->fBloomInterval;
		}
	}


	if (m_fCurLifeTime > m_OwnDesc->fLifeTime - m_OwnDesc->fStartdelay)
	{
		_float Threadhold = (m_OwnDesc->fLifeTime - m_OwnDesc->fStartdelay) / m_OwnDesc->fLifeTime;
		_float t = (m_fLifeTimeRatio - Threadhold) / (1.0f - m_fLifeTimeRatio);
		t = max(0.f, min(t, 1.f));
		_vector Maxsize = XMLoadFloat3(&m_OwnDesc->fMaxSize);
		_vector MinSize = XMVectorZero();
		_vector ResultSize = XMVectorLerp(Maxsize, MinSize, t);
		m_pTransformCom->Set_Scale(XMVectorGetX(ResultSize), XMVectorGetY(ResultSize), XMVectorGetZ(ResultSize));
	}

	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	_vector DirPos = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), ParentMat);
	_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector ResultPos = XMVectorLerp(CurPos, DirPos, m_OwnDesc->fChaseSpeed * fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ResultPos);


	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);
}

void CFireFlyCube::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CFireFlyCube::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(25);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CFireFlyCube::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(26);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CFireFlyCube::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireFlyCube::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireFlyCube::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_BloomPower, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CFireFlyCube* CFireFlyCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireFlyCube* pInstance = new CFireFlyCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFireFlyCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFireFlyCube::Clone(void* pArg)
{
	CFireFlyCube* pInstance = new CFireFlyCube(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFireFlyCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireFlyCube::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

#include "HexaShield.h"
#include "GameInstance.h"

CHexaShield::CHexaShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CHexaShield::CHexaShield(const CHexaShield& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CHexaShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHexaShield::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<HEXASHIELD>(*((HEXASHIELD*)pArg));

	GAMEOBJECT_DESC GDesc{};
	GDesc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->RotationSpeed);
	if (FAILED(__super::Initialize(&GDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);

	m_OwnDesc->HitDesc.ParentMatrix = m_OwnDesc->ParentMatrix;
	Generate_HexaHit();
	return S_OK;
}

void CHexaShield::Priority_Tick(_float fTimeDelta)
{
}

void CHexaShield::Tick(_float fTimeDelta)
{


	m_AccTime += fTimeDelta;

	if (m_DesolveStart)
	{
		m_DesolveThreadhold += fTimeDelta;
		m_DesolveRatio = m_DesolveThreadhold / m_OwnDesc->DesolveTime;
		m_DesolveRatio = max(0.f, min(m_DesolveRatio, 1.f));
		if (m_DesolveThreadhold > m_OwnDesc->DesolveTime)
			m_pGameInstance->Erase(this);

		m_OwnDesc->vSize.x += m_DesolveRatio * 0.1f;
		m_OwnDesc->vSize.y += m_DesolveRatio * 0.1f;
		m_OwnDesc->vSize.z += m_DesolveRatio * 0.1f;
		m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);
	}
	else
	{
		m_Interval -= fTimeDelta;
		if (m_Interval < 0.f)
		{
			m_Interval = m_OwnDesc->LoopInterval;
			m_IntervalChange = !m_IntervalChange;
		
		}

		if (m_IntervalChange)
			m_OwnDesc->OpacityPower += m_OwnDesc->fOpacitySpeed * fTimeDelta;
		else
			m_OwnDesc->OpacityPower -= m_OwnDesc->fOpacitySpeed * fTimeDelta;

		m_OwnDesc->OpacityPower = max(0, min(m_OwnDesc->OpacityPower, 0.8f));

		if (IsHit)
		{
			m_HitInterval += fTimeDelta;
			m_HitRatio = m_HitInterval / m_OwnDesc->HitTime;
			m_HitRatio = max(0, min(m_HitRatio, 1));
			if (m_HitInterval > m_OwnDesc->HitTime)
			{
				m_HitInterval = 0.f;
				m_HitRatio = 0.f;
				IsHit = false;
			}
		}
	}
	
	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	ParentMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), ParentMat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
}

void CHexaShield::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CHexaShield::Render()
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
		
		m_pShaderCom->Begin(33);
		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CHexaShield::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
	
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
			return E_FAIL;
		

		m_pShaderCom->Begin(34);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CHexaShield::Render_Distortion()
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

void CHexaShield::Set_Shield_Hit()
{
	if(!IsHit)
		IsHit = true;
	else
	{
		m_HitInterval = 0.f;
		m_HitRatio = 0.f;
	}
	Generate_HexaHit();
}

HRESULT CHexaShield::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HexaShield_MK2"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHexaShield::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OpacityPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_DesolveStart, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_DesolveRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", 1)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CHexaShield::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OpacityPower, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color2", &m_OwnDesc->fBloomColor2, sizeof(_float3))))
		return E_FAIL;
	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_DesolveStart, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_DesolveRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameRatio", &m_HitRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Hit", &IsHit, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", 1)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CHexaShield::Generate_HexaHit()
{
	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_HexaHit"), &m_OwnDesc->HitDesc);

	return S_OK;
}

CHexaShield* CHexaShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHexaShield* pInstance = new CHexaShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHexaShield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHexaShield::Clone(void* pArg)
{
	CHexaShield* pInstance = new CHexaShield(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHexaShield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHexaShield::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}

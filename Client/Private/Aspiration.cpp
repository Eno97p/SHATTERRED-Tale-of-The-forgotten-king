#include "stdafx.h"
#include "..\Public\Aspiration.h"

#include "GameInstance.h"

CAspiration::CAspiration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAspiration::CAspiration(const CAspiration& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CAspiration::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAspiration::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC gameObjDesc = *(GAMEOBJECT_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameObjDesc.mWorldMatrix._41, gameObjDesc.mWorldMatrix._42, gameObjDesc.mWorldMatrix._43, 1.f)); //초기 위치 설정))

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(3.f, 3.f, 3.f);

	m_pGameInstance->Disable_Echo();
	m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_HandThree.ogg"), SOUND_MONSTER);

	return S_OK;
}

void CAspiration::Priority_Tick(_float fTimeDelta)
{
	m_fTexcoordY -= fTimeDelta * 1.15f;
	if (m_fTexcoordY < 0.f)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CGameObject::GAMEOBJECT_DESC gameObjDesc;
		gameObjDesc.fSpeedPerSec = 40.f;
		gameObjDesc.mWorldMatrix._41 = XMVectorGetX(vPos);
		gameObjDesc.mWorldMatrix._42 = XMVectorGetY(vPos);
		gameObjDesc.mWorldMatrix._43 = XMVectorGetZ(vPos);
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_Sphere"), &gameObjDesc);
		m_pGameInstance->Erase(this);
	}
}

void CAspiration::Tick(_float fTimeDelta)
{
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(1.f));
}

void CAspiration::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CAspiration::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CAspiration::Render_Bloom()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAspiration::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAspiration::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TexcoordY", &m_fTexcoordY, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CAspiration* CAspiration::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAspiration* pInstance = new CAspiration(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAspiration");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAspiration::Clone(void* pArg)
{
	CAspiration* pInstance = new CAspiration(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAspiration");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAspiration::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
}

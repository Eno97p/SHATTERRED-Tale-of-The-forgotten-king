#include "Weapon_Gun_LGGun.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon_Arrow_LGGun.h"
#include "Legionnaire_Gun.h"

CWeapon_Gun_LGGun::CWeapon_Gun_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Gun_LGGun::CWeapon_Gun_LGGun(const CWeapon_Gun_LGGun& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWeapon_Gun_LGGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Gun_LGGun::Initialize(void* pArg)
{
	GUN_DESC* pDesc = (GUN_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;
	m_pParent = pDesc->pParent;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CWeapon_Gun_LGGun::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 0.5f;
		break;
	default:
		break;
	}
}

void CWeapon_Gun_LGGun::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	//화살 발사
	if (m_bIsActive)
	{
		CWeapon_Arrow_LGGun::ARROW_DESC arrowDesc;
		arrowDesc.vDir = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) -
			XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
		arrowDesc.vDir.m128_f32[0] *= -1.f;
		arrowDesc.vDir.m128_f32[1] += 1.5f;
		arrowDesc.vDir.m128_f32[2] *= -1.f;
		arrowDesc.vPos = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
		arrowDesc.fSpeedPerSec = 30.f;
		arrowDesc.pParent = m_pParent;
		m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Weapon_Arrow_LGGun"), &arrowDesc);
	}
}

void CWeapon_Gun_LGGun::Late_Tick(_float fTimeDelta)
{
	if (m_bRenderAvailable)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
		if (m_pGameInstance->Get_MoveShadow())
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		}
	}
}

HRESULT CWeapon_Gun_LGGun::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CWeapon_Gun_LGGun::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4 fPos = m_pGameInstance->Get_PlayerPos();

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPos.x, fPos.y + 10.f, fPos.z - 10.f, 1.f), XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CWeapon_Gun_LGGun::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Legionnaire_Shoot.ogg"), SOUND_MONSTER);
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

HRESULT CWeapon_Gun_LGGun::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon_Gun_LGGun::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CWeapon_Gun_LGGun* CWeapon_Gun_LGGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Gun_LGGun* pInstance = new CWeapon_Gun_LGGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWeapon_Gun_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Gun_LGGun::Clone(void* pArg)
{
	CWeapon_Gun_LGGun* pInstance = new CWeapon_Gun_LGGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWeapon_Gun_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Gun_LGGun::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}

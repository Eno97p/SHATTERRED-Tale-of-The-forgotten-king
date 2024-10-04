#include "Elevator.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CElevator::CElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CElevator::CElevator(const CElevator& rhs)
	: CToolObj(rhs)
{
}

HRESULT CElevator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElevator::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Elevator");
		strcpy_s(m_szLayer, "Layer_Active_Element");
		strcpy_s(m_szModelName, "Prototype_Component_Model_Elevator");
		m_eModelType = CModel::TYPE_NONANIM;
		m_iTriggerType = pDesc->TriggerType;

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));

		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//TRIGGER STATE SET
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CElevator::Priority_Tick(_float fTimeDelta)
{

}

void CElevator::Tick(_float fTimeDelta)
{
	switch (m_eElevatorState)
	{
	case ELEVATOR_IDLE:
		// 대기 상태, 아무 동작도 하지 않음
		break;

	case ELEVATOR_ASCEND:
	{
		_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector newPos = XMVectorSetY(currentPos, XMVectorGetY(currentPos) + 0.05f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

		if (XMVectorGetY(newPos) >= XMVectorGetY(m_vTargetPos))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos);
			m_eElevatorState = ELEVATOR_REBOUND;
			m_fReboundTimer = 0.f;
		}
		break;
	}

	case ELEVATOR_DESCEND:
	{
		_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector newPos = XMVectorSetY(currentPos, XMVectorGetY(currentPos) - 0.05f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

		if (XMVectorGetY(newPos) <= XMVectorGetY(m_vTargetPos))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos);
			m_eElevatorState = ELEVATOR_REBOUND;
			m_fReboundTimer = 0.f;
		}
		break;
	}

	case ELEVATOR_REBOUND:
	{
		m_fReboundTimer += fTimeDelta;

		// 덜컹거리는 효과 구현
		_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float reboundOffset = sin(m_fReboundTimer * 10.f) * 0.3f; // 주기와 강도 조절 가능
		_vector newPos = XMVectorSetY(currentPos, XMVectorGetY(currentPos) + reboundOffset);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

		if (m_fReboundTimer > 1.f) // 1초 동안 덜컹거림
		{
			m_eElevatorState = ELEVATOR_IDLE;
			m_fReboundTimer = 0.f;
		}
		break;
	}

	default:
		break;
	}
}

void CElevator::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CElevator::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;

		//if ( i != 29)
		
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
}

HRESULT CElevator::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

		m_pShaderCom->Unbind_SRVs();


			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 1, aiTextureType_EMISSIVE)))
				return E_FAIL;


		m_pShaderCom->Begin(6);

		m_pModelCom->Render(1);
}

HRESULT CElevator::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_AABB;

	// 월드 매트릭스에서 스케일 추출
	_float3 vScale;
	vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	// 기본 크기에 스케일 적용
	ColliderDesc.vExtents = _float3(0.5f * vScale.x, 1.f * vScale.y, 0.5f * vScale.z);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CElevator::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CElevator* CElevator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElevator* pInstance = new CElevator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CElevator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElevator::Clone(void* pArg)
{
	CElevator* pInstance = new CElevator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CElevator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElevator::Free()
{
	__super::Free();

	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
}

#include "EventTrigger.h"
#include "ToolObj.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

#include "Elevator.h"
#include "SideViewCamera.h"

CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
	: CToolObj(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_EventTrigger");
		strcpy_s(m_szLayer, "Layer_Trigger");
		strcpy_s(m_szModelName, "Prototype_Component_Model_BasicCube");
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
		m_eTRIGState = (TRIGGER_TYPE)pDesc->TriggerType;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{

}

void CEventTrigger::Tick(_float fTimeDelta)
{

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider")));
	if (nullptr != pPlayerCollider)
	{
		if (CCollider::COLL_START == m_pColliderCom->Intersect(pPlayerCollider))
		{
			switch (m_eTRIGState)
			{
			case TRIG_TUTORIAL_BOSSENCOUNTER:
			{
				CToolObj::TOOLOBJ_DESC pDesc = {};
				_matrix vMat = { 1.4f, 0.f, 0.f, 0.f,
				0.f, 10.f, 0.f, 0.f,
				0.f, 0.f, 1.4f, 0.f,
				154.009f, 531.828f, 96.989f, 1.f };
				XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
				strcpy_s(pDesc.szModelName, "Prototype_Component_Model_BasicDonut");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_FakeWall"), &pDesc);
			}
			break;
			case TRIG_JUGGLAS_SPAWNSECONDROOM:
			{
				m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), 0));
				CToolObj::TOOLOBJ_DESC pDesc = {};
				_matrix vMat = XMMatrixIdentity();
				XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
				strcpy_s(pDesc.szModelName, "Prototype_Component_Model_RasSamrahCastle2");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_ToolObj"), &pDesc);
			}
				break;
			case TRIG_JUGGLAS_SPAWNTHIRDROOM:
			{
				m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), 0));
				CToolObj::TOOLOBJ_DESC pDesc = {};
				_matrix vMat = XMMatrixIdentity();
				XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
				strcpy_s(pDesc.szModelName, "Prototype_Component_Model_RasSamrahCastle3");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_ToolObj"), &pDesc);

				strcpy_s(pDesc.szModelName, "Prototype_Component_Model_RasSamrahCastle4");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_ToolObj"), &pDesc);
			}
			break;
			case TRIG_VIEWCHANGE_TTOS:
			{
				m_pGameInstance->Set_MainCamera(2);
			}
			break;
			case TRIG_VIEWCHANGE_STOT:
			{
				m_pGameInstance->Set_MainCamera(1);
			}
			break;
			case TRIG_ASCEND_ELEVATOR:
			{

				dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Active_Element"), 0))->Ascend(XMVectorSet(-389.f, 86.f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
			}
			break;
			case TRIG_DESCEND_ELEVATOR:
			{
				dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Active_Element"), 0))->Descend(XMVectorSet(-389.f, 7.485f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
			}
			break;
			case TRIG_SCENE_CHANGE:
			{

			}
			break;
			case TRIG_VIEWCHANGE_TTOBS:
			{
				m_pGameInstance->Set_MainCamera(2);
				dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_MainCamera())->Set_BossScene(true);
			}
			break;

			default:
				break;
			}
			m_pGameInstance->Erase(this);
		}
	}
	

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEventTrigger::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if ( i != 29)


		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &i, sizeof(_uint))))
		//	return;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}
}

HRESULT CEventTrigger::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
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

HRESULT CEventTrigger::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
	CEventTrigger* pInstance = new CEventTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

#include "..\Public\Decal.h"

#include "GameInstance.h"
#include "ComputeShader_Buffer.h"
#include "ToolObj_Manager.h"
CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CToolObj{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Decal");
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

	}

	DECAL_DESC* gameobjDesc = (DECAL_DESC*)pArg;
	m_iDecalIdx = gameobjDesc->iDecalIdx;
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&gameobjDesc->mWorldMatrix));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameobjDesc->mWorldMatrix._41, gameobjDesc->mWorldMatrix._42, gameobjDesc->mWorldMatrix._43, 1.f));

	return S_OK;
}

void CDecal::Priority_Tick(_float fTimeDelta)
{
}

void CDecal::Tick(_float fTimeDelta)
{
}

void CDecal::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DECAL, this);
}

pair<_uint, _matrix> CDecal::Render_Decal()
{
	// first : 텍스쳐 번호, second : 월드 역행렬(건들지 말것)
	return pair(m_iDecalIdx, m_pTransformCom->Get_WorldMatrix_Inverse());
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();

	/*Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);*/
}

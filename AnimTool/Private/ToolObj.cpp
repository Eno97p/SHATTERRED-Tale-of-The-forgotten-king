#include "ToolObj.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "ToolObj_Manager.h"
#include "Bone_Sphere.h"
#include "Bone.h"


CToolObj::CToolObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CToolObj::CToolObj(const CToolObj& rhs)
	: CGameObject{ rhs }
{
}

_uint CToolObj::Get_CurKeyFrame(_uint iAnimIdx, _uint iChannelIdx)
{
	return m_pModelCom->Get_CurKeyFrame(iAnimIdx, iChannelIdx);
}

_double CToolObj::Get_Duration(_uint iAnimIdx)
{
	return m_pModelCom->Get_Duration(iAnimIdx);
}

const _double* CToolObj::Get_CurrentPos(_uint iAnimIdx) const
{
	return m_pModelCom->Get_CurrentPos(iAnimIdx);
}

string CToolObj::Get_BoneName(_uint iBoneIdx)
{
	vector<string>* pBoneName = m_pModelCom->Get_BoneNameVec_MemVar();
	vector<string>::iterator boneName = pBoneName->begin();

	for (size_t i = 0; i < iBoneIdx; ++i)
		++boneName;

	return *boneName;
}

HRESULT CToolObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	TOOLOBJ_DESC* pDesc = (TOOLOBJ_DESC*)pArg;

	strcpy_s(m_szName, pDesc->szObjName);
	strcpy_s(m_szLayer, pDesc->szLayer);
	strcpy_s(m_szModelName, pDesc->szModelName);
	m_eModelType = CModel::TYPE_ANIM;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	Setting_WorldMatrix(pDesc);

	CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj

	// 생성 목록에 리스트 번호 매기기
	string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
	strcat_s(m_szListName, strSize.c_str());
	strcat_s(m_szListName, m_szName);

	Setting_AnimList();
	Setting_BoneList();

	Create_BoneSphere();

	m_eAnimState = ANIM_PLAY;

	return S_OK;
}

void CToolObj::Priority_Tick(_float fTimeDelta)
{
}

void CToolObj::Tick(_float fTimeDelta)
{
	switch (m_eAnimState)
	{
	case AnimTool::CToolObj::ANIM_PLAY:
	{
		m_pModelCom->StopAnimation(false);
		m_pModelCom->Play_Animation(fTimeDelta);
		// 여기에서 Channel의 keyframe 할당하기?
		//m_pModelCom->Set_CurKeyFrame(0, 0); // Imgui에서 받아온 값으로 넣어주어야 함


		if (m_pModelCom->Check_CurDuration(2.0)) // Test
			int i = 1;
	}

	break;
	case AnimTool::CToolObj::ANIM_PAUSE:
		m_pModelCom->StopAnimation(true);
		break;
	default:
		break;
	}

	for (auto& pBoneSphere : m_vecBoneSphere)
		pBoneSphere->Tick(fTimeDelta);
}

void CToolObj::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	for (auto& pBoneSphere : m_vecBoneSphere)
		pBoneSphere->Late_Tick(fTimeDelta);
}

HRESULT CToolObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CToolObj::Add_Component()
{
	std::wstring wstr(m_szName, m_szName + strlen(m_szName));

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(Check_AnimType()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolObj::Check_AnimType()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

void CToolObj::Setting_WorldMatrix(void* pArg)
{
	TOOLOBJ_DESC* pDesc = (TOOLOBJ_DESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);
	m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));
}

void CToolObj::Setting_AnimList()
{
	CImgui_Manager::GetInstance()->Clear_vecAnim();

	// Model로부터 Anim List를 읽어와 세팅
	vector<string>* pAnimName = m_pModelCom->Get_AnimNameVec_MemVar();

	for (auto& pAnimName : *pAnimName)
	{
		size_t length = pAnimName.length() + 1;
		char* szAnimName = new char[length];
		strcpy_s(szAnimName, length, pAnimName.c_str());

		CImgui_Manager::GetInstance()->Add_vecAnim(szAnimName); // m_szAnimName
	}
}

void CToolObj::Setting_BoneList()
{
	CImgui_Manager::GetInstance()->Clear_vecBone();

	vector<string>* pBoneName = m_pModelCom->Get_BoneNameVec_MemVar();

	for (auto& pBoneName : *pBoneName)
	{
		size_t length = pBoneName.length() + 1;
		char* szBoneName = new char[length];
		strcpy_s(szBoneName, length, pBoneName.c_str());

		CImgui_Manager::GetInstance()->Add_vecBone(szBoneName);
	}
}

void CToolObj::Create_BoneSphere()
{
	// Bone마다 Sphere 객체 생성
	CBone_Sphere::BONESPHERE_DSC pDesc{};

	vector<CBone*>* pVecBone = m_pModelCom->Get_Bones();

	_uint iIdx = { 0 };
	for (auto& pBone : *pVecBone)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));

		pDesc.iBoneIdx = iIdx++;
		pDesc.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrix();
		pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

		CBone_Sphere* pBoneSphere = dynamic_cast<CBone_Sphere*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BoneSphere"), &pDesc));
		if (nullptr == pBoneSphere)
			return;
		
		m_vecBoneSphere.emplace_back(pBoneSphere);
	}
}

void CToolObj::Setting_ChannelList()
{
	// 현재 선택된 Anim의 Channel List를 갱신
	//_uint iListSize = CImgui_Manager::GetInstance()->Get_ChannelList_Size();

	vector<string>* pChannelName = m_pModelCom->Get_ChannelNameVec_MemVar(CImgui_Manager::GetInstance()->Get_AnimIdx());

	for (auto& pChannelName : *pChannelName)
	{
		size_t length = pChannelName.length() + 1;
		char* szChannelName = new char[length];
		strcpy_s(szChannelName, length, pChannelName.c_str());

		CImgui_Manager::GetInstance()->Add_vecChannel(szChannelName);
	}
}

void CToolObj::Change_AnimState(_uint iAnimState)
{
	switch (iAnimState)
	{
	case 0:
		m_eAnimState = ANIM_PLAY;
		break;
	case 1:
		m_eAnimState = ANIM_PAUSE;
		break;
	default:
		break;
	}
}

void CToolObj::Set_Animation(_uint iAnimIdx, _bool isLoop)
{
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(iAnimIdx, isLoop));
}

void CToolObj::Setting_KeyFrameSliders(_uint iAnimIdx, _uint iChannelIdx)
{
	// Model이 가지는 keyframe 활용

	// Imgui의 값 변경?
	CImgui_Manager::GetInstance()->Set_NumKeyFrame(m_pModelCom->Get_NumKeyFrame(iAnimIdx, iChannelIdx));
}

CToolObj* CToolObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolObj* pInstance = new CToolObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CToolObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolObj::Clone(void* pArg)
{
	CToolObj* pInstance = new CToolObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CToolObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolObj::Free()
{
	__super::Free();

	for (auto& pBoneSphere : m_vecBoneSphere)
		Safe_Release(pBoneSphere);
	m_vecBoneSphere.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

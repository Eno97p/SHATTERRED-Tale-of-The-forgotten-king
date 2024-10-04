#include "NPC_Rlya.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "Body_Rlya.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"
#include "ItemData.h"

CNPC_Rlya::CNPC_Rlya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc{ pDevice, pContext }
{
}

CNPC_Rlya::CNPC_Rlya(const CNPC_Rlya& rhs)
	: CNpc{ rhs }
{
}

HRESULT CNPC_Rlya::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Rlya::Initialize(void* pArg)
{
	NPC_DESC Desc;

	Desc.eLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	 
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(277.f, 8.6f, -25.8f, 1.f)); // Test

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_iDialogCnt = 3;

	Create_Script();

	return S_OK;
}

void CNPC_Rlya::Priority_Tick(_float fTimeDelta)
{
	m_pBody->Priority_Tick(fTimeDelta);
}

void CNPC_Rlya::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	m_pActivateUI->Tick(fTimeDelta);

	if (m_isScriptOn)
	{
		m_pScriptUI->Tick(fTimeDelta);

		Key_Input();
	}
}

void CNPC_Rlya::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);

	if (Check_Distance())
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_F) && !m_isScriptOn)
		{
			m_pScriptUI->Set_Rend(true);
			if(m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("오랜 세월 동안 이 어둠 속을 밝혀주는 것은 희미한 빛 뿐이었지."));
			m_isScriptOn = true;
		}
	}

	if (m_isScriptOn)
		m_pScriptUI->Late_Tick(fTimeDelta);
}

HRESULT CNPC_Rlya::Render()
{
	return S_OK;
}

HRESULT CNPC_Rlya::Add_PartObjects()
{
	// Body
	CPartObject::PARTOBJ_DESC pDesc{};
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pDesc.fSpeedPerSec = 0.f;
	pDesc.fRotationPerSec = 0.f;
	pDesc.eLevel = m_eLevel;

	m_pBody = dynamic_cast<CBody_Rlya*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Rlya"), &pDesc));

	return S_OK;
}

void CNPC_Rlya::Key_Input()
{
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		switch (m_iDialogCnt)
		{
		case 3:
		{
			m_pScriptUI->Set_DialogText(TEXT("기억해라, 방황하는 자여.\n빛이 어둠을 물리치듯 너 또한 네 앞에 놓인 모든 어둠을 헤쳐나갈 수 있을 거다."));
			--m_iDialogCnt;
			break;
		}
		case 2:
		{
			m_pScriptUI->Set_DialogText(TEXT("자, 받아라. 그리고 이 빛을 따라 나아가라."));
			--m_iDialogCnt;
			break;
		}
		case 1:
		{
			m_pScriptUI->Set_DialogText(TEXT("FireFly를 사용하면 길을 밝힐 수 있어."));
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			--m_iDialogCnt;

			CInventory::GetInstance()->Add_Item(CItemData::ITEMNAME_FIREFLY); // Inventory에 Firefly 추가

			break;
		}
		case 0:
		{
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			break;
		}
		default:
			break;
		}
	}
}

HRESULT CNPC_Rlya::Create_Script()
{
	CUIGroup_Script::UIGROUP_SCRIPT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eNpcType = CUIGroup_Script::NPC_RLYA;

	m_pScriptUI = dynamic_cast<CUIGroup_Script*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Script"), &pDesc));
	if (nullptr == m_pScriptUI)
		return E_FAIL;

	return S_OK;
}

CNPC_Rlya* CNPC_Rlya::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Rlya* pInstance = new CNPC_Rlya(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNPC_Rlya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Rlya::Clone(void* pArg)
{
	CNPC_Rlya* pInstance = new CNPC_Rlya(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNPC_Rlya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Rlya::Free()
{
	__super::Free();

	Safe_Release(m_pBody);
}

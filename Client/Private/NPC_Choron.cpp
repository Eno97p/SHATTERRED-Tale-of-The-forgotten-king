#include "NPC_Choron.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "Body_Choron.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"
#include "ItemData.h"

CNPC_Choron::CNPC_Choron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CNpc{ pDevice, pContext }
{
}

CNPC_Choron::CNPC_Choron(const CNPC_Choron& rhs)
    : CNpc{ rhs }
{
}

HRESULT CNPC_Choron::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Choron::Initialize(void* pArg)
{
	NPC_DESC Desc;

	Desc.eLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	_matrix vMat = { 0.750f, 0.f, -1.299f, 0.f,
			0.f, 1.5f, 0.f, 0.f,
			1.299f, 0.f, 0.750f, 0.f,
			-261.643f, 20.761f, -180.369f, 1.f };

	m_pTransformCom->Set_WorldMatrix(vMat);


	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_iDialogCnt = 4;

	Create_Script();

	return S_OK;
}

void CNPC_Choron::Priority_Tick(_float fTimeDelta)
{
}

void CNPC_Choron::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	m_pActivateUI->Tick(fTimeDelta);

	if (m_isScriptOn)
	{
		m_pScriptUI->Tick(fTimeDelta);

		Key_Input();
	}
}

void CNPC_Choron::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);

	if (Check_Distance())
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_F) && !m_isScriptOn)
		{
			m_pScriptUI->Set_Rend(true);
			if (m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("방랑하는 망령을 쓰러뜨렸군. 새로운 여정의 시작이라고 볼 수도 있겠지."));
			m_isScriptOn = true;
		}
	}

	if (m_isScriptOn)
		m_pScriptUI->Late_Tick(fTimeDelta);
}

HRESULT CNPC_Choron::Render()
{
	return S_OK;
}

HRESULT CNPC_Choron::Add_PartObjects()
{
	// Body
	CPartObject::PARTOBJ_DESC pDesc{};
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pDesc.fSpeedPerSec = 0.f;
	pDesc.fRotationPerSec = 0.f;
	pDesc.eLevel = m_eLevel;

	m_pBody = dynamic_cast<CBody_Choron*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Choron"), &pDesc));

	return S_OK;
}

HRESULT CNPC_Choron::Create_Script()
{
	CUIGroup_Script::UIGROUP_SCRIPT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eNpcType = CUIGroup_Script::NPC_CHORON;

	m_pScriptUI = dynamic_cast<CUIGroup_Script*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Script"), &pDesc));
	if (nullptr == m_pScriptUI)
		return E_FAIL;

	return S_OK;
}

void CNPC_Choron::Key_Input()
{
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		switch (m_iDialogCnt)
		{
		case 4:
		{
			m_pScriptUI->Set_DialogText(TEXT("그대가 과연 어디까지 도달할 수 있을지 궁금하군. 도움을 주마."));
			--m_iDialogCnt;
			break;
		}
		case 3:
		{
			m_pScriptUI->Set_DialogText(TEXT("이 검은 그대가 마주할 모든 어둠을 밝히는 불꽃이 될 것이고 그 불꽃은 그대가 지닌 용기를\n끝없이 시험할 것이다."));
			--m_iDialogCnt;
			break;
		}
		case 2:
		{
			m_pScriptUI->Set_DialogText(TEXT("왕국의 운명을 짊어진 자여, 행운을 빈다. 그대의 여정이 불꽃처럼 밝게 빛나기를."));
			--m_iDialogCnt;
			break;
		}
		case 1:
		{
			m_pScriptUI->Set_DialogText(TEXT("잊지 마라. 좋은 무기는 오직 진정한 용사만이 다룰 수 있다는 것을."));
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			--m_iDialogCnt;

			CInventory::GetInstance()->Add_Weapon(CItemData::ITEMNAME_WHISPERSWORD);

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

CNPC_Choron* CNPC_Choron::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Choron* pInstance = new CNPC_Choron(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNPC_Choron");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Choron::Clone(void* pArg)
{
	CNPC_Choron* pInstance = new CNPC_Choron(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNPC_Choron");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Choron::Free()
{
	__super::Free();

	Safe_Release(m_pBody);
}

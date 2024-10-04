#include "ItemData.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "UI_Manager.h"
#include "EffectManager.h"

#include "UIGroup_Inventory.h"
#include "UIGroup_InvSub.h"
#include "UIGroup_Quick.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_BuffTimer.h"

#include "Player.h"

CItemData::CItemData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CItemData::CItemData(const CItemData& rhs)
	: CGameObject{rhs}
{
}

HRESULT CItemData::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemData::Initialize(void* pArg)
{
	ITEMDATA_DESC* pDesc = static_cast<ITEMDATA_DESC*>(pArg);

	if (pDesc->isDropTem)
		Set_DropItem_Data(pDesc->eDropItemName);
	else
	{
		m_eItemName = pDesc->eItemName;
		Set_Item_Data();
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iCount = 1;
	m_iLevel = 0;

	return S_OK;
}

void CItemData::Priority_Tick(_float fTimeDelta)
{
}

void CItemData::Tick(_float fTimeDelta)
{
}

void CItemData::Late_Tick(_float fTimeDelta)
{
}

HRESULT CItemData::Render()
{
	return S_OK;
}

void CItemData::Use_Item(_uint iInvenIdx)
{
	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_CATALYST:
	{
		// ��� 0�� �Ǿ ������� �ʱ�
		if (m_iCount > 0)
		{
			m_iCount -= 1;
		}
		else
			break;

		// ü�� ȸ��
		list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Add_Hp(100.f);

		dynamic_cast<CPlayer*>(PlayerList.front())->Set_isBuffState(true);
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_BuffNum(0);

		break;
	}
	case Client::CItemData::ITEMNAME_HOVERBOARD:
	{
		list<CGameObject*> PlayerList = CGameInstance::GetInstance()->Get_GameObjects_Ref(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Generate_HoverBoard();

		break;
	}
	case Client::CItemData::ITEMNAME_FIREFLY:
	{
		list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(dynamic_cast<CPlayer*>(PlayerList.front())->Get_Component(TEXT("Com_Transform")));
		CEffectManager::GetInstance()->Generate_FireFly(pPlayerTransform->Get_WorldFloat4x4());
		break;
	}
	case Client::CItemData::ITEMNAME_BUFF1:
	{
		Apply_UseCount(iInvenIdx);
		dynamic_cast<CUIGroup_BuffTimer*>(CUI_Manager::GetInstance()->Get_UIGroup("BuffTimer"))->Create_BuffTimer(TEXT("Prototype_Component_Texture_Icon_Item_Buff0"));
		
		list<CGameObject*> PlayerList = CGameInstance::GetInstance()->Get_GameObjects_Ref(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_isBuffState(true);
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_BuffNum(1);

		break;
	}
	case Client::CItemData::ITEMNAME_BUFF2:
	{
		Apply_UseCount(iInvenIdx);
		dynamic_cast<CUIGroup_BuffTimer*>(CUI_Manager::GetInstance()->Get_UIGroup("BuffTimer"))->Create_BuffTimer(TEXT("Prototype_Component_Texture_Icon_Item_Buff1"));
		
		list<CGameObject*> PlayerList = CGameInstance::GetInstance()->Get_GameObjects_Ref(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_isBuffState(true);
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_BuffNum(2);

		break;
	}
	case Client::CItemData::ITEMNAME_BUFF3:
	{
		Apply_UseCount(iInvenIdx);
		dynamic_cast<CUIGroup_BuffTimer*>(CUI_Manager::GetInstance()->Get_UIGroup("BuffTimer"))->Create_BuffTimer(TEXT("Prototype_Component_Texture_Icon_Item_Buff2"));
		
		list<CGameObject*> PlayerList = CGameInstance::GetInstance()->Get_GameObjects_Ref(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_isBuffState(true);
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_BuffNum(3);

		break;
	}
	case Client::CItemData::ITEMNAME_BUFF4:
	{
		Apply_UseCount(iInvenIdx);
		dynamic_cast<CUIGroup_BuffTimer*>(CUI_Manager::GetInstance()->Get_UIGroup("BuffTimer"))->Create_BuffTimer(TEXT("Prototype_Component_Texture_Icon_Item_Buff3"));
		
		list<CGameObject*> PlayerList = CGameInstance::GetInstance()->Get_GameObjects_Ref(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"));
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_isBuffState(true);
		dynamic_cast<CPlayer*>(PlayerList.front())->Set_BuffNum(4);
		
		break;
	}
	case Client::CItemData::ITEMNAME_ESSENCE:
	{
		_uint iRand = rand() % 300 + 100;
		CInventory::GetInstance()->Calcul_Soul(iRand);
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Rend_Calcul(iRand);

		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_ETHER:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	default:
		break;
	}
}

void CItemData::Apply_UseCount(_uint iInvenIdx)
{
	if (m_iCount > 1)
	{
		m_iCount -= 1;
	}
	else // �ϳ� ���� ��� ���� �ʿ�
	{
		// UI Inventory�� Sub Quick���� ����
		dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Delete_InvSub_QuickSlot_ToInvIdx(iInvenIdx);

		// Quick�� Slot���� ����
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_QuickSlot_Delete(iInvenIdx);

		// UI Inventory������ ����
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory_Delete(iInvenIdx);

		// Quick SubInv������ ����
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_Delete(iInvenIdx);

		// HUD������ ����   
		dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_QUICK);

		CInventory::GetInstance()->Delete_Item(this);

		// Slot�� ���ܿ���
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory(iInvenIdx);
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_Inventory(iInvenIdx);
	}
}

void CItemData::Use_Skill()
{
	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_OPH:
	{

		break;
	}
	case Client::CItemData::ITEMNAME_AKSHA:
	{
		list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

		pPlayer->Set_Cloaking();
		break;
	}
	default:
		break;
	}
}

void CItemData::Set_DropItem_Data(CItem::ITEM_NAME eItemName)
{
	switch (eItemName)
	{
	case Client::CItem::ITEM_BUFF1:
	{
		m_eItemType = ITEMTYPE_USABLE; //  ITEMTYPE_BUFF
		m_eItemName = ITEMNAME_BUFF1;
		m_wszItemName = TEXT("SIGIL OF STRENGTH");
		m_wszItemExplain = TEXT("�ݼ� ������ ū Ȩ���� ������\n�������� ���α�\n������ ���� �ɷ���\n�Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("�ݼ� ������ ū Ȩ�� ����� �������� ���α�\n������ ���� �ɷ��� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("���� ������ ������\n���� ���α�\n������ ��� �ɷ���\n�Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("���� ������ ������ ���� ���α�\n������ ��� �ɷ��� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("�ϼ��� ������\n������ ���α�\nü�� ȸ���� �Ͻ������� ����");
		m_wszItemExplain_quick = TEXT("�ϼ��� ������ ������ ���α�\nü�� ȸ���� �Ͻ������� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("����� ���� ���� ������ ������\n���������� ���α�\n�������� �Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("����� ���� ���� ������ ������ ���������� ���α�\n�������� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_ESSENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ESSENCE;
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Essence");
		m_wszItemName = TEXT("GRACEFUL ESSENCE");
		m_wszItemExplain = TEXT("�������� ������ [�����ϸ� �Ϸ�]\n�������� �޸� �м�");
		m_wszItemExplain_quick = TEXT("�������� ������ [�����ϸ� �Ϸ�]\n�������� �޸� �м�");
		break;
	}
	case Client::CItem::ITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ETHER;
		m_wszItemName = TEXT("RADIANT ETHER");
		m_wszItemExplain = TEXT("�����ϰ� ������ ������ ���� ����\nǳ���� õ�� �������� �߻�");
		m_wszItemExplain_quick = TEXT("�����ϰ� ������ ������ ���� ����\nǳ���� õ�� �������� �߻�");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItem::ITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE1;
		m_wszItemName = TEXT("HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszItemExplain_quick = TEXT("Astyr ��ü ��κ��� �����ϴ� ������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0");
		break;
	}
	case Client::CItem::ITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE2;
		m_wszItemName = TEXT("GRACEFUL HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszItemExplain_quick = TEXT("Astyr ��ü ��κ��� �����ϴ� ������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade1");
		break;
	}
	default:
		break;
	}
}

void CItemData::Set_Item_Data()
{
	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_DURGASWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Durga's Claymore");
		m_wszItemExplain = TEXT("�߱�� Durga�� ��\n��ȭ ��Ʈ��ũ ������\n�߿� �ܼ�\n���� �ٽ� �Ϻ� ����ǰ�� ���Ե�\n\n���յ� ��� Ư���� �����Ѵ�.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Durgas_Claymore");
		// Upgrade ���� �����͵�
		m_iPrice = 425;
		m_iValue = 5;
		m_iAddDamage = 10;
		break;
	}
	case Client::CItemData::ITEMNAME_PRETORIANSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Pretorian Sword");
		m_wszItemExplain = TEXT("�����뽺�� ����߳��� ��ȣ�ϱ� ����\n�������� ����ϴ� ��Ƽ ����,\n�����丮�� �������� ��\n\n�׵��� ������ �ڸ�����\nħ���� ���㸸�� ���´�.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Pretorian");
		// Upgrade ���� �����͵�
		m_iPrice = 400;
		m_iValue = 3;
		m_iAddDamage = 6;
		break;
	}
	case Client::CItemData::ITEMNAME_RADAMANTHESWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Radamanthe's Dagger"); 
		m_wszItemExplain = TEXT("�߱�� Radamanthe�� ��\n��ȭ ��Ʈ��ũ ������ �߿� �ܼ�\n���� �ٽ� �Ϻ� ����ǰ�� ���Ե�\n\n���յ� ��� Ư���� �����Ѵ�.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Radamanthes");
		// Upgrade ���� �����͵�
		m_iPrice = 400;
		m_iValue = 5;
		m_iAddDamage = 8;
		break;
	}
	case Client::CItemData::ITEMNAME_WHISPERSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Elish");
		m_wszItemExplain = TEXT("�ջ�� ������. �б� ���� �߻�.\n��ü �ݸ� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Elish");
		// Upgrade ���� �����͵�
		m_iPrice = 325;
		m_iValue = 2;
		m_iAddDamage = 3;
		break;
	}
	case Client::CItemData::ITEMNAME_OPH:
	{
		m_eItemType = ITEMTYPE_SKILL;
		m_wszItemName = TEXT("Oph");
		m_wszItemExplain = TEXT("�������� �޴�ũ �ٴϸ���\n���� ���� �� �ϳ�\n�Ͻ������� ���⿡ ���Ե�\n�ϵ�ξƴ�Ʈ�� Ȱ��ȭ�Ͽ�\n���ظ� ũ�� ������Ų��.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Oph");
		break;
	}
	case Client::CItemData::ITEMNAME_AKSHA:
	{
		m_eItemType = ITEMTYPE_SKILL;
		m_wszItemName = TEXT("Aksha");
		m_wszItemExplain = TEXT("Niphur�� Sentient����\n������ ������Ÿ��\n�����ڰ� �Ͻ�������\n���� ��󿡼� �����.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Aksha");
		break;
	}
	case Client::CItemData::ITEMNAME_CATALYST:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_wszItemName = TEXT("Catalyst");
		m_wszItemExplain = TEXT("���� �ູ�� ���\n���� ��õ\nü���� ȸ����\n�� �ִ�.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Catalyst");
		break;
	}
	case Client::CItemData::ITEMNAME_HOVERBOARD:
	{
		m_eItemType = ITEMTYPE_ETC;
		m_wszItemName = TEXT("HOVERBOARD");
		m_wszItemExplain = TEXT("�ձ��� �����\n������ ���յ� �����\n������ ������ ���� �ʴ�\n���� Ż ��");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_HoverBoard");
		break;
	}
	case Client::CItemData::ITEMNAME_FIREFLY:
	{
		m_eItemType = ITEMTYPE_ETC;
		m_wszItemName = TEXT("FIREFLY");
		m_wszItemExplain = TEXT("���谡�� �ձ��� �����ִ�\n����� ��¡\n�ƹ��� ����� £�\n���� �׻� ���� ã�Ƴ���.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Firefly");
		break;
	}
	default:
		break;
	}
}

CItemData* CItemData::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemData* pInstance = new CItemData(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItemData");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemData::Clone(void* pArg)
{
	CItemData* pInstance = new CItemData(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItemData");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemData::Free()
{
	__super::Free();
}

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
		// 얘는 0이 되어도 사라지지 않기
		if (m_iCount > 0)
		{
			m_iCount -= 1;
		}
		else
			break;

		// 체력 회복
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
	else // 하나 남은 경우 삭제 필요
	{
		// UI Inventory의 Sub Quick에서 삭제
		dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Delete_InvSub_QuickSlot_ToInvIdx(iInvenIdx);

		// Quick의 Slot에서 삭제
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_QuickSlot_Delete(iInvenIdx);

		// UI Inventory에서도 삭제
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory_Delete(iInvenIdx);

		// Quick SubInv에서도 삭제
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_Delete(iInvenIdx);

		// HUD에서도 삭제   
		dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_QUICK);

		CInventory::GetInstance()->Delete_Item(this);

		// Slot들 땡겨오기
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
		m_wszItemExplain = TEXT("금속 조각에 큰 홈으로 새겨진\n기하학적 봉인구\n무기의 공격 능력을\n일시적으로 강화");
		m_wszItemExplain_quick = TEXT("금속 조각에 큰 홈을 새기는 기하학적 봉인구\n무기의 공격 능력을 일시적으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("유리 구슬에 장착된\n원형 봉인구\n갑옷의 방어 능력을\n일시적으로 강화");
		m_wszItemExplain_quick = TEXT("유리 구슬에 장착된 원형 봉인구\n갑옷의 방어 능력을 일시적으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("암석에 조각된\n정교한 봉인구\n체력 회복을 일시적으로 증가");
		m_wszItemExplain_quick = TEXT("암석에 조각된 정교한 봉인구\n체력 회복을 일시적으로 증가");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("희미한 빛의 수정 조각에 장착된\n수수께끼의 봉인구\n지구력을 일시적으로 강화");
		m_wszItemExplain_quick = TEXT("희미한 빛의 수정 조각에 장착된 수수께끼의 봉인구\n지구력을 일시정으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_ESSENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ESSENCE;
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Essence");
		m_wszItemName = TEXT("GRACEFUL ESSENCE");
		m_wszItemExplain = TEXT("안정적인 데이터 [컴파일링 완료]\n보류중인 메모리 분석");
		m_wszItemExplain_quick = TEXT("안정적인 데이터 [컴파일링 완료]\n보류중인 메모리 분석");
		break;
	}
	case Client::CItem::ITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ETHER;
		m_wszItemName = TEXT("RADIANT ETHER");
		m_wszItemExplain = TEXT("섬세하게 조각된 반투명 수정 조각\n풍부한 천상 에너지를 발산");
		m_wszItemExplain_quick = TEXT("섬세하게 조각된 반투명 수정 조각\n풍부한 천상 에너지를 발산");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItem::ITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE1;
		m_wszItemName = TEXT("HADRONITE");
		m_wszItemExplain = TEXT("Astyr 신체 대부분을 구성하는\n잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszItemExplain_quick = TEXT("Astyr 신체 대부분을 구성하는 잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0");
		break;
	}
	case Client::CItem::ITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE2;
		m_wszItemName = TEXT("GRACEFUL HADRONITE");
		m_wszItemExplain = TEXT("Astyr 신체 대부분을 구성하는\n잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszItemExplain_quick = TEXT("Astyr 신체 대부분을 구성하는 잠재적 물질\n무기 업그레이드 시 사용 가능");
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
		m_wszItemExplain = TEXT("추기경 Durga의 검\n심화 네트워크 접속의\n중요 단서\n신의 핵심 일부 복제품이 포함됨\n\n결합된 모든 특권을 제공한다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Durgas_Claymore");
		// Upgrade 관련 데이터들
		m_iPrice = 425;
		m_iValue = 5;
		m_iAddDamage = 10;
		break;
	}
	case Client::CItemData::ITEMNAME_PRETORIANSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Pretorian Sword");
		m_wszItemExplain = TEXT("히프노스와 나라야나를 보호하기 위해\n전적으로 헌신하는 안티 기계들,\n프리토리아 근위대의 검\n\n그들이 지나간 자리에는\n침묵의 폐허만이 남는다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Pretorian");
		// Upgrade 관련 데이터들
		m_iPrice = 400;
		m_iValue = 3;
		m_iAddDamage = 6;
		break;
	}
	case Client::CItemData::ITEMNAME_RADAMANTHESWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Radamanthe's Dagger"); 
		m_wszItemExplain = TEXT("추기경 Radamanthe의 검\n심화 네트워크 접속의 중요 단서\n신의 핵심 일부 복제품이 포함됨\n\n결합된 모든 특권을 제공한다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Radamanthes");
		// Upgrade 관련 데이터들
		m_iPrice = 400;
		m_iValue = 5;
		m_iAddDamage = 8;
		break;
	}
	case Client::CItemData::ITEMNAME_WHISPERSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Elish");
		m_wszItemExplain = TEXT("손상된 데이터. 읽기 오류 발생.\n전체 격리 권장");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Elish");
		// Upgrade 관련 데이터들
		m_iPrice = 325;
		m_iValue = 2;
		m_iAddDamage = 3;
		break;
	}
	case Client::CItemData::ITEMNAME_OPH:
	{
		m_eItemType = ITEMTYPE_SKILL;
		m_wszItemName = TEXT("Oph");
		m_wszItemExplain = TEXT("전설적인 메다크 바니르가\n만든 유물 중 하나\n일시적으로 무기에 포함된\n하드로아니트를 활성화하여\n피해를 크게 증가시킨다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Oph");
		break;
	}
	case Client::CItemData::ITEMNAME_AKSHA:
	{
		m_eItemType = ITEMTYPE_SKILL;
		m_wszItemName = TEXT("Aksha");
		m_wszItemExplain = TEXT("Niphur의 Sentient에서\n개발한 프로토타입\n소유자가 일시적으로\n감지 대상에서 벗어난다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Aksha");
		break;
	}
	case Client::CItemData::ITEMNAME_CATALYST:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_wszItemName = TEXT("Catalyst");
		m_wszItemExplain = TEXT("왕의 축복이 깃든\n힘의 원천\n체력을 회복할\n수 있다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Catalyst");
		break;
	}
	case Client::CItemData::ITEMNAME_HOVERBOARD:
	{
		m_eItemType = ITEMTYPE_ETC;
		m_wszItemName = TEXT("HOVERBOARD");
		m_wszItemExplain = TEXT("왕국의 기술과\n마법이 결합된 결과물\n지형의 제약을 받지 않는\n고성능 탈 것");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_HoverBoard");
		break;
	}
	case Client::CItemData::ITEMNAME_FIREFLY:
	{
		m_eItemType = ITEMTYPE_ETC;
		m_wszItemName = TEXT("FIREFLY");
		m_wszItemExplain = TEXT("모험가의 앞길을 밝혀주는\n희망의 상징\n아무리 어둥이 짙어도\n빛은 항상 길을 찾아낸다.");
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

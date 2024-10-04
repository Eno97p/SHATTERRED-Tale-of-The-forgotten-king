#include "UI_WPEquipSlot.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "UI_Manager.h"
#include "CMouse.h"
#include "UI_WPEquipNone.h"
#include "UI_ItemIcon.h"

#include "UIGroup_Weapon.h"
#include "UIGroup_WeaponSlot.h"

CUI_WPEquipSlot::CUI_WPEquipSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_WPEquipSlot::CUI_WPEquipSlot(const CUI_WPEquipSlot& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_WPEquipSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WPEquipSlot::Initialize(void* pArg)
{
	UI_EQUIPSLOT_DESC* pDesc = static_cast<UI_EQUIPSLOT_DESC*>(pArg);

	m_eSlotNum = pDesc->eSlotNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 170.6f; // 256
	m_fSizeY = 170.6f;

	Setting_Position();

	Create_Frame();

	return S_OK;
}

void CUI_WPEquipSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WPEquipSlot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.1f), LONG(m_fY - m_fSizeY * 0.3f),
			LONG(m_fX + m_fSizeX * 0.1f) ,LONG(m_fY + m_fSizeY * 0.3f) };

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect)
	{
		m_fY = SELECT_Y;
		m_pNoneFrame->Set_PosY(SELECT_Y);

		if (m_pGameInstance->Mouse_Down(DIM_LB))
			Click_Event();
	}
	else
	{
		m_fY = DEFAULT_Y;
		m_pNoneFrame->Set_PosY(DEFAULT_Y);
	}

	Setting_Position();

	if (nullptr != m_pNoneFrame)
		m_pNoneFrame->Tick(fTimeDelta);

	if (nullptr != m_pItemIcon)
	{
		m_pItemIcon->Update_Pos(m_fX - 3.f, m_fY - 6.f);
		m_pItemIcon->Tick(fTimeDelta);
	}
}

void CUI_WPEquipSlot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, ELEVENTH);
	 
	if (nullptr != m_pNoneFrame && !m_isSelect)
		m_pNoneFrame->Late_Tick(fTimeDelta);

	if (nullptr != m_pItemIcon && Check_GroupRenderOnAnim())
		m_pItemIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_WPEquipSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (m_isSelect)
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("SLOT TO EQUIP:"), _float2(25.f, m_fY - 30.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Create_ItemIcon(_bool isWeapon)
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX - 3.f;
	pDesc.fY = m_fY - 6.f;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = TWELFTH;

	if (isWeapon)
	{
		if (nullptr != m_pItemIcon) // ItemIcon이 이미 있다면 제거
		{
			vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
			for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize(); ++i)
			{
				if (m_pItemIcon->Get_TextureName() == (*weapon)->Get_TextureName())
				{
					(*weapon)->Set_isEquip(false);

					// Equip Sign 제거
					dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(false, i);
					break;
				}
				else
					++weapon;
			}

			Safe_Release(m_pItemIcon);
			m_pItemIcon = nullptr;
		}

		vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
		for (size_t i = 0; i < dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_CurSlotIdx(); ++i)
			++weapon;

		pDesc.wszTexture = (*weapon)->Get_TextureName();
	}
	else
	{
		if (nullptr != m_pItemIcon) // ItemIcon이 이미 있다면 제거
		{
			vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
			for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize(); ++i)
			{
				if (m_pItemIcon->Get_TextureName() == (*skill)->Get_TextureName())
				{
					(*skill)->Set_isEquip(false);

					// Equip Sign 제거
					dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(false, i);
					break;
				}
				else
					++skill;
			}

			Safe_Release(m_pItemIcon);
			m_pItemIcon = nullptr;
		}

		vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
		for (size_t i = 0; i < dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_CurSlotIdx(); ++i)
			++skill;

		pDesc.wszTexture = (*skill)->Get_TextureName();
	}

	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Delete_ItemIcon()
{
	Safe_Release(m_pItemIcon);
	m_pItemIcon = nullptr;

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Change_ItemIcon(_bool isWeapon, _uint iSlotIdx)
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX - 3.f;
	pDesc.fY = m_fY - 6.f;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = TWELFTH;

	if (isWeapon)
	{
		if (nullptr != CInventory::GetInstance()->Get_EquipWeapon(iSlotIdx))
		{
			pDesc.wszTexture = CInventory::GetInstance()->Get_EquipWeapon(iSlotIdx)->Get_TextureName();

			m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));
		}
	}
	else
	{
		if (nullptr != CInventory::GetInstance()->Get_EquipSkill(iSlotIdx))
		{
			pDesc.wszTexture = CInventory::GetInstance()->Get_EquipSkill(iSlotIdx)->Get_TextureName();

			m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));
		}
	}

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponEquipSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Create_Frame()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 170.6f; // 256
	pDesc.fSizeY = 170.6f;
	m_pNoneFrame = dynamic_cast<CUI_WPEquipNone*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPEquipNone"), &pDesc));

	return S_OK;
}

void CUI_WPEquipSlot::Setting_XY()
{
	switch (m_eSlotNum)
	{
	case Client::CUI_WPEquipSlot::NUM_ONE:
		m_fX = 270.f;
		break;
	case Client::CUI_WPEquipSlot::NUM_TWO:
		m_fX = 380.f;
		break;
	case Client::CUI_WPEquipSlot::NUM_THREE:
		m_fX = 490.f;

		break;
	default:
		break;
	}

	m_fY = DEFAULT_Y;
}

void CUI_WPEquipSlot::Click_Event()
{
	_bool isAlphaBG_On = dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_EquipMode();	
	_uint iCurSlotIdx = dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_CurSlotIdx();

	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();

	// Inventory의 WeaponList에 있는 Weapon(or Skill)을 Index 활용하여 접근
	for (size_t i = 0; i < iCurSlotIdx; ++i)
	{
		if (CInventory::GetInstance()->Get_WeaponSize() > iCurSlotIdx)
			++weapon;

		if (CInventory::GetInstance()->Get_SkillSize() > iCurSlotIdx)
			++skill;
	}

	if (isAlphaBG_On) // 장착
	{
		_uint iCount = { 0 };

		if (CUIGroup_Weapon::TAB_L == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
			if ((*weapon)->Get_isEquip()) // 이미 Equip Slot에 장착 중이었던 경우
			{
				// Inventory의 Equip Arr 에서 제거
				for (size_t i = 0; i < 3; ++i)
				{
					if ((*weapon) == CInventory::GetInstance()->Get_EquipWeapon(i))
					{
						(*weapon)->Set_isEquip(false);
						CInventory::GetInstance()->Delete_EquipWeapon(i);
						break;
					}
				}
				dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(false, iCount);
			}
		}
		else if(CUIGroup_Weapon::TAB_R == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
			if ((*skill)->Get_isEquip())
			{
				for (size_t i = 0; i < 3; ++i)
				{
					if ((*skill) == CInventory::GetInstance()->Get_EquipSkill(i))
					{
						(*skill)->Set_isEquip(false);
						CInventory::GetInstance()->Delete_EquipSkill(i);
						break;
					}
				}
				dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(false, iCount);
			}
		}

		// Equip Slot에 장착 등록
		weapon = CInventory::GetInstance()->Get_Weapons()->begin();
		skill = CInventory::GetInstance()->Get_Skills()->begin();

		if (CUIGroup_Weapon::TAB_L == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
			for (size_t i = 0; i < iCurSlotIdx; ++i)
				++weapon;

			if (!(*weapon)->Get_isEquip()) // 장착 중이 아닌 지에 대한 정보는 필요 없지 않을까 어차피 장착 중이어도 제거햇을 테니
			{
				CInventory::GetInstance()->Add_EquipWeapon((*weapon), m_eSlotNum);
				(*weapon)->Set_isEquip(true);

				// Quick에 Weapon을 추가한 순간 size에 맞게 m_iWeaponIdx 초기화
				dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotIdx(CUIGroup_WeaponSlot::SLOT_WEAPON);
			}
		}
		else if (CUIGroup_Weapon::TAB_R == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
			for (size_t i = 0; i < iCurSlotIdx; ++i)
				++skill;
			
			if (!(*skill)->Get_isEquip())
			{
				CInventory::GetInstance()->Add_EquipSkill((*skill), m_eSlotNum);
				(*skill)->Set_isEquip(true);

				// Quick에 Skill을 추가한 순간 size에 맞게 m_iSkillIdx 초기화
				dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotIdx(CUIGroup_WeaponSlot::SLOT_SKILL);
			}
		}

		dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true); // Equip Sign 활성화
		dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Set_EquipMode(false); // AlphaBG 비활성화
	}
	else // 장착 해제 >>>>>>> 이때도 Reset 해주야 하는 거 아님?
	{
		_uint iCount = { 0 };

		if (CUIGroup_Weapon::TAB_L == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
 			weapon = CInventory::GetInstance()->Get_Weapons()->begin();

			for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize(); ++i)
			{
				if ((*weapon) == CInventory::GetInstance()->Get_EquipWeapon(m_eSlotNum))
				{
					(*weapon)->Set_isEquip(false);
					CInventory::GetInstance()->Delete_EquipWeapon(m_eSlotNum);
					break;
				}
				else
				{
					++weapon;
					++iCount;
				}
			}
		}
		else if (CUIGroup_Weapon::TAB_R == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
		{
			skill = CInventory::GetInstance()->Get_Skills()->begin();

			for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize(); ++i)
			{
				if ((*skill) == CInventory::GetInstance()->Get_EquipSkill(m_eSlotNum))
				{
					(*skill)->Set_isEquip(false);
					CInventory::GetInstance()->Delete_EquipSkill(m_eSlotNum);
					break;
				}
				else
				{
					++skill;
					++iCount;
				}
			}
		}
		dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(false, iCount);
	}
}

_bool CUI_WPEquipSlot::Check_GroupRenderOnAnim()
{
	return 	CUI_Manager::GetInstance()->Get_UIGroup("Weapon")->Get_RenderOnAnim();
}

CUI_WPEquipSlot* CUI_WPEquipSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WPEquipSlot* pInstance = new CUI_WPEquipSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WPEquipSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WPEquipSlot::Clone(void* pArg)
{
	CUI_WPEquipSlot* pInstance = new CUI_WPEquipSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WPEquipSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WPEquipSlot::Free()
{
	__super::Free();

	Safe_Release(m_pNoneFrame);
	Safe_Release(m_pItemIcon);
}

#include "UI_InvSub_Btn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "CMouse.h"
#include "UI_InvSub_BtnSelect.h"

#include "UIGroup_Inventory.h"
#include "UIGroup_Quick.h"
#include "UIGroup_WeaponSlot.h"

CUI_InvSub_Btn::CUI_InvSub_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Interaction{ pDevice, pContext }
{
}

CUI_InvSub_Btn::CUI_InvSub_Btn(const CUI_InvSub_Btn& rhs)
    : CUI_Interaction{ rhs }
{
}

HRESULT CUI_InvSub_Btn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_InvSub_Btn::Initialize(void* pArg)
{
	UI_BTN_DESC* pDesc = static_cast<UI_BTN_DESC*>(pArg);

	m_iSlotIdx = pDesc->iSlotIdx;
    m_eBtnType = pDesc->eBtnType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Create_SelectBtn();

	Setting_Position();

	return S_OK;
}

void CUI_InvSub_Btn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_InvSub_Btn::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    __super::Tick(fTimeDelta);

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (nullptr != m_pSelectBtn)
		m_pSelectBtn->Tick(fTimeDelta);

	if (m_isSelectEnd)
	{
		m_fSelectedTimer += fTimeDelta;
		if (0.1f <= m_fSelectedTimer)
		{
			m_isSelectEnd = false;
			m_fSelectedTimer = 0.f;
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_AnimFinished(false);
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_RenderOnAnim(false);
		}
	}
}

void CUI_InvSub_Btn::Late_Tick(_float fTimeDelta)
{
	Mouse_Input();

    CGameInstance::GetInstance()->Add_UI(this, FIFTEENTH); // THIRTEENTH

	if (nullptr != m_pSelectBtn && m_isSelect)
		m_pSelectBtn->Late_Tick(fTimeDelta);
}

HRESULT CUI_InvSub_Btn::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();

	_vector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (m_isSelect)
	{
		vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	}
	else
	{
		m_pVIBufferCom->Render();
		vColor = XMVectorSet(0.5f, 0.5f, 0.5f, 1.f);
	}

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo"), Settiing_BtnText(), _float2(m_fX - 80.f, m_fY - 8.f), vColor)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InvSub_Btn::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InvSub_Btn_None"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InvSub_Btn::Bind_ShaderResources()
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

HRESULT CUI_InvSub_Btn::Create_SelectBtn()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = m_fSizeX;
	pDesc.fSizeY = m_fSizeY;

	m_pSelectBtn = dynamic_cast<CUI_InvSub_BtnSelect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_InvSub_BtnSelect"), &pDesc));
	if (nullptr == m_pSelectBtn)
		return E_FAIL;

    return S_OK;
}

_tchar* CUI_InvSub_Btn::Settiing_BtnText()
{
	switch (m_eBtnType)
	{
	case Client::CUI_InvSub_Btn::BTN_SET:
		return TEXT("SET IN QUICK ACCESS");
	case Client::CUI_InvSub_Btn::BTN_USE:
		return TEXT("                USE");
	case Client::CUI_InvSub_Btn::BTN_CANCEL:
		return TEXT("             CANCEL");
	default:
		return TEXT("");
	}
}

void CUI_InvSub_Btn::Mouse_Input()
{
	if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
	{
		switch (m_eBtnType)
		{
		case Client::CUI_InvSub_Btn::BTN_SET:
		{
			// 중복의 경우 예외 처리 필요
			CItemData* item = CInventory::GetInstance()->Get_ItemData(m_iSlotIdx);
			if (item->Get_isEquip())
			{

			}
			else
			{
				CInventory::GetInstance()->Add_QuickAccess(CInventory::GetInstance()->Get_ItemData(m_iSlotIdx), m_iSlotIdx);
				m_isSelectEnd = true;

				// Equip Sign 활성화
				item->Set_isEquip(true);
				dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Slot_EquipSign(m_iSlotIdx, true);

				// Quick Acess의 InvSlot도 Equip Sign 활성화
				dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_EquipSign(m_iSlotIdx, true);

				// Quick에 Item을 추가한 순간 Quick~ size에 맞게 m_iQuickIdx 초기화
				dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotIdx(CUIGroup_WeaponSlot::SLOT_QUICK);
				
				CUI_Manager::GetInstance()->Delete_RedDot_Slot(true);
			}
			break;
		}
		case Client::CUI_InvSub_Btn::BTN_USE:
		{
			CItemData* pItem = CInventory::GetInstance()->Get_ItemData(m_iSlotIdx);

			// ETC는 사용하는 경우 창이 꺼지는 것도 ㄱㅊ을듯 (바로 HUD로 돌아가도록)
			if (CItemData::ITEMTYPE_USABLE == pItem->Get_ItemType()
				|| CItemData::ITEMTYPE_BUFF == pItem->Get_ItemType()
				|| CItemData::ITEMTYPE_ETC == pItem->Get_ItemType())
			{
				pItem->Use_Item(m_iSlotIdx);

				CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_AnimFinished(false);
				CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_RenderOnAnim(false);
				CUI_Manager::GetInstance()->Delete_RedDot_Slot(true);

				if (CItemData::ITEMTYPE_ETC == pItem->Get_ItemType())
				{
					// 창 꺼지고 HUD로 돌아가도록? 할지 말지
				}
			}

			break;
		}
		case Client::CUI_InvSub_Btn::BTN_CANCEL:
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_AnimFinished(false);
			CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_RenderOnAnim(false);
			CUI_Manager::GetInstance()->Delete_RedDot_Slot(true);
			break;
		default:
			break;
		}
	}
}

CUI_InvSub_Btn* CUI_InvSub_Btn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_InvSub_Btn* pInstance = new CUI_InvSub_Btn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_InvSub_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_InvSub_Btn::Clone(void* pArg)
{
	CUI_InvSub_Btn* pInstance = new CUI_InvSub_Btn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_InvSub_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_InvSub_Btn::Free()
{
	__super::Free();

	Safe_Release(m_pSelectBtn);
}

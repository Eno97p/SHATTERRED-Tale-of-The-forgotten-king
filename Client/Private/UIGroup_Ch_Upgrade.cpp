#include "UIGroup_Ch_Upgrade.h"

#include "GameInstance.h"
#include "Player.h"

#include "UI_CharacterBG.h"
#include "UI_CharacterTop.h"
#include "UI_StateSoul.h"
#include "UI_Ch_UpgradeBtn.h"
#include "UI_Ch_Upgrade_OkBtn.h"

CUIGroup_Ch_Upgrade::CUIGroup_Ch_Upgrade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Ch_Upgrade::CUIGroup_Ch_Upgrade(const CUIGroup_Ch_Upgrade& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Ch_Upgrade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Ch_Upgrade::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Ch_Upgrade::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Ch_Upgrade::Tick(_float fTimeDelta)
{
    _bool isRender_End = false;
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
        {
            if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
            {
                pUI->Resset_Animation(true);
            }
            else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
            {
                pUI->Resset_Animation(false);
            }

            pUI->Tick(fTimeDelta);

            isRender_End = pUI->isRender_End();
        }
        if (isRender_End)
            m_isRend = false;

        for (auto& pBtn : m_vecBtn)
        {
            /*if (!m_isRenderOnAnim && !(pBtn->Get_RenderOnAnim()))
            {
                pBtn->Resset_Animation(true);
            }
            else if (m_isRenderOnAnim && pBtn->Get_RenderOnAnim())
            {
                pBtn->Resset_Animation(false);
            }*/

            pBtn->Tick(fTimeDelta);
        }
    }
}

void CUIGroup_Ch_Upgrade::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);

        for (auto& pBtn : m_vecBtn)
            pBtn->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_Ch_Upgrade::Render()
{
    return S_OK;
}

void CUIGroup_Ch_Upgrade::Resset_OriginData()
{
    list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

    // 해당 함수가 호출 되면(Ch Upgrade Page가 Render On 되었을 때 최초로 호출) 현재 Player가 가지고 있는 원본 값들을 전부 받아옴
    m_tOriginData.iOriginLv = pPlayer->Get_Level();
    m_tOriginData.iOriginVitalityLv = pPlayer->Get_VitalityLv();
    m_tOriginData.iOriginStaminaLv = pPlayer->Get_StaminaLv();
    m_tOriginData.iOriginStrenghtLv = pPlayer->Get_StrenghtLv();
    m_tOriginData.iOriginMysticismLv = pPlayer->Get_MysticismLv();
    m_tOriginData.iOriginKnowledgeLv = pPlayer->Get_KnowledgeLv();
    m_tOriginData.iOriginHealth = pPlayer->Get_MaxHP();
    m_tOriginData.iOriginStamina_State = pPlayer->Get_MaxStamina();
    m_tOriginData.iOriginEther = pPlayer->Get_MaxMP();
    m_tOriginData.iOriginPhysicalDmg = pPlayer->Get_PhysicalDmg();
    m_tOriginData.iOriginEtherDmg = pPlayer->Get_EtherDmg();
}

HRESULT CUIGroup_Ch_Upgrade::Create_UI()
{
    CUI::UI_DESC pDesc{};

    // Character BG 
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_CharacterBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_CharacterBG"), &pDesc)));

    // Character Top 
    m_vecUI.emplace_back(dynamic_cast<CUI_CharacterTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_CharacterTop"), &pDesc)));

    // Soul
    CUI_StateSoul::UI_SOUL_DESC pSoulDesc{};
    pSoulDesc.eLevel = LEVEL_STATIC;
    pSoulDesc.isSoulCntRend = false;
    pSoulDesc.isNextlevel = true;
    pSoulDesc.fX = 450.f; // 500
    pSoulDesc.fY = 207.f;
    pSoulDesc.fSizeX = 24.f;
    pSoulDesc.fSizeY = 24.f;
    pSoulDesc.eUISort = NINETH;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc)));

    pSoulDesc.isSoulCntRend = true;
    pSoulDesc.isNextlevel = false;
    pSoulDesc.fX = 450.f;
    pSoulDesc.fY = 227.f;
    pSoulDesc.fSizeX = 24.f;
    pSoulDesc.fSizeY = 24.f;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc)));

    if (FAILED(Create_Btn()))
        return E_FAIL;

    // OkBtn 
    m_vecUI.emplace_back(dynamic_cast<CUI_Ch_Upgrade_OkBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_OKBtn"), &pDesc)));

    return S_OK;
}

HRESULT CUIGroup_Ch_Upgrade::Create_Btn()
{
    CUI_Ch_UpgradeBtn::ABILITY_TYPE arrAbility[CUI_Ch_UpgradeBtn::ABILITY_END] =
    { CUI_Ch_UpgradeBtn::ABILITY_VITALITY, CUI_Ch_UpgradeBtn::ABILITY_STAMINA, CUI_Ch_UpgradeBtn::ABILITY_STRENGHT,
      CUI_Ch_UpgradeBtn::ABILITY_MYSTICISM, CUI_Ch_UpgradeBtn::ABILITY_KNOWLEDGE };
    
    CUI_Ch_UpgradeBtn::UI_CHUP_BTN_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;

    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 2; ++j)
        {
            pDesc.fX = (g_iWinSizeX >> 1) - 50.f + (j * 30.f);
            pDesc.fY = (g_iWinSizeY >> 1) - 53.f + (i * 30.f);
            pDesc.fSizeX = 25.f;
            pDesc.fSizeY = 25.f;
            pDesc.eAbilityType = arrAbility[i];
            if (0 == j)
                pDesc.isPlus = false;
            else
                pDesc.isPlus = true;

            m_vecBtn.emplace_back(dynamic_cast<CUI_Ch_UpgradeBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_Btn"), &pDesc)));
        }
    }

    return S_OK;
}

CUIGroup_Ch_Upgrade* CUIGroup_Ch_Upgrade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_Ch_Upgrade* pInstance = new CUIGroup_Ch_Upgrade(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_Ch_Upgrade");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_Ch_Upgrade::Clone(void* pArg)
{
    CUIGroup_Ch_Upgrade* pInstance = new CUIGroup_Ch_Upgrade(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_Ch_Upgrade");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_Ch_Upgrade::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);

    for (auto& pBtn : m_vecBtn)
        Safe_Release(pBtn);
}

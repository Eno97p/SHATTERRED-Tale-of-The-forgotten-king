#include "UI_FadeInOut.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Level_Loading.h"

#include "UI_Memento.h"
#include "UI_AeonsLost.h"

CUI_FadeInOut::CUI_FadeInOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_FadeInOut::CUI_FadeInOut(const CUI_FadeInOut& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_FadeInOut::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_FadeInOut::Initialize(void* pArg)
{
	UI_FADEINOUT_DESC* pDesc = static_cast<UI_FADEINOUT_DESC*>(pArg);

	m_isFadeIn = pDesc->isFadeIn;
	m_isLevelChange = pDesc->isLevelChange;
	m_eFadeType = pDesc->eFadeType;
	m_eNetLevel = pDesc->eNetLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	if (m_isLevelChange)
		m_fAlphaTimerMul = 0.7f;
	else
	{
		if(m_isFadeIn)
			m_fAlphaTimerMul = 0.2f;
		else
			m_fAlphaTimerMul = 0.4f;
	}

	if (!m_isLevelChange && m_isFadeIn && m_eFadeType == TYPE_ALPHA)
		Create_Memento();
	else if (m_isFadeIn && m_eFadeType == TYPE_DISSOLVE)
		Create_AeonsLost();
	

	return S_OK;
}

void CUI_FadeInOut::Priority_Tick(_float fTimeDelta)
{
}

void CUI_FadeInOut::Tick(_float fTimeDelta)
{
	if (TYPE_ALPHA == m_eFadeType)
	{
		m_fAlphaTimer += fTimeDelta * m_fAlphaTimerMul;
		if (m_fAlphaTimer >= 1.f)
		{
			m_fAlphaTimer = 1.f;

			if (!m_isFadeIn) // Fade Out
			{
				if (m_isLevelChange) // Level Change
				{
					// 다음 Level로 넘어감
					m_pGameInstance->Scene_Change(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, Check_NextLevel()));
				}
				else // Memeto
				{
					UI_FADEINOUT_DESC pDesc{};
					pDesc.isFadeIn = true;
					pDesc.eFadeType = TYPE_ALPHA;
					pDesc.isLevelChange = false; // false

					if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
						return;

					m_pGameInstance->Erase(this);
				}
			}
			else
				m_pGameInstance->Erase(this);
		}
	}
	else if (TYPE_DISSOLVE == m_eFadeType)
	{
		if (!m_isFadeIn || (m_isFadeIn && m_pAeonsLost->Get_isEnd())) // 이러면 AeonsLost의 렌더가 끝나고 나서 Tick이 돌텐데 왜 바로 nullptr이 된 것처럼 생성을 하지?

		{
			m_fDisolveValue += fTimeDelta * 0.3f;
		}

		if (m_fDisolveValue >= 1.f)
		{
			m_isFadeAnimEnd = true;

			if (!m_isFadeIn)
				CUI_Manager::GetInstance()->Create_FadeInOut_Dissolve(true);
			else
				return;
		}
	}

	if (nullptr != m_pMemento)
		m_pMemento->Tick(fTimeDelta);

	if (nullptr != m_pAeonsLost)
		m_pAeonsLost->Tick(fTimeDelta);
}

void CUI_FadeInOut::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH); // 더 추가될 수 있음

	if (nullptr != m_pMemento)
		m_pMemento->Late_Tick(fTimeDelta);

	if (nullptr != m_pAeonsLost)
		m_pAeonsLost->Late_Tick(fTimeDelta);
}

HRESULT CUI_FadeInOut::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (TYPE_ALPHA == m_eFadeType)
	{
		m_pShaderCom->Begin(3);
	}
	else if (TYPE_DISSOLVE == m_eFadeType)
	{
		m_pShaderCom->Begin(4);
	}

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_FadeInOut::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FadeInOut"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (TYPE_DISSOLVE == m_eFadeType)
	{
		/* For.Com_DissolveTexture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
			TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_FadeInOut::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (TYPE_ALPHA == m_eFadeType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaTimer, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isFadeIn, sizeof(_bool))))
			return E_FAIL;
	}
	else if (TYPE_DISSOLVE == m_eFadeType)
	{
		if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 29)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isFadeIn, sizeof(_bool))))
			return E_FAIL;
	}

	return S_OK;
}

//HRESULT CUI_FadeInOut::Create_FadeIn()
//{
//	UI_FADEINOUT_DESC pDesc{};
//
//	pDesc.isFadeIn = true;
//	pDesc.isLevelChange = false;
//
//	if (TYPE_DISSOLVE == m_eFadeType)
//	{
//		pDesc.eFadeType = TYPE_DISSOLVE;
//	}
//	else
//	{
//		pDesc.eFadeType = TYPE_ALPHA;
//	}
//
//	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CUI_FadeInOut::Create_Memento()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	m_pMemento = dynamic_cast<CUI_Memento*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Memento"), &pDesc));

	return S_OK;
}

HRESULT CUI_FadeInOut::Create_AeonsLost()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel - LEVEL_STATIC;

	m_pAeonsLost = dynamic_cast<CUI_AeonsLost*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_AeonsLost"), &pDesc));

	return S_OK;
}

LEVEL CUI_FadeInOut::Check_NextLevel()
{
	switch (m_pGameInstance->Get_CurrentLevelIndex())
	{
	case LEVEL_GAMEPLAY:
		return LEVEL_GRASSLAND;
	case LEVEL_ACKBAR:
		return LEVEL_GRASSLAND;
	case LEVEL_JUGGLAS:
		return LEVEL_GRASSLAND;
	case LEVEL_ANDRASARENA:
		return LEVEL_GRASSLAND;
	case LEVEL_GRASSLAND:
	{
		if (m_eNetLevel == LEVEL_ACKBAR)
			return LEVEL_ACKBAR;
		else if (m_eNetLevel == LEVEL_JUGGLAS)		
			return LEVEL_JUGGLAS;
		else if (m_eNetLevel == LEVEL_ANDRASARENA)
			return LEVEL_ANDRASARENA;
		else
		{
			__debugbreak();
			return LEVEL_END;
		}
	}
	default:
		return LEVEL_END;
	}
}

CUI_FadeInOut* CUI_FadeInOut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_FadeInOut* pInstance = new CUI_FadeInOut(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_FadeInOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_FadeInOut::Clone(void* pArg)
{
	CUI_FadeInOut* pInstance = new CUI_FadeInOut(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_FadeInOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_FadeInOut::Free()
{
	__super::Free();

	Safe_Release(m_pAeonsLost);
	Safe_Release(m_pMemento);
	Safe_Release(m_pDisolveTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

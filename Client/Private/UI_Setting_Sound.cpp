#include "UI_Setting_Sound.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

#include "UI_Setting_SoundBar.h"

CUI_Setting_Sound::CUI_Setting_Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_Setting_Sound::CUI_Setting_Sound(const CUI_Setting_Sound& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_Setting_Sound::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Setting_Sound::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1) + fRANGE; // MAX·Î ½ÃÀÛ
	m_fY = (g_iWinSizeY >> 1) + 170.f;
	m_fSizeX = 60.f; // 800
	m_fSizeY = 31.f; // 32

	Setting_Position();

	Create_Bar();

	return S_OK;
}

void CUI_Setting_Sound::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_Sound::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	__super::Tick(fTimeDelta);

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect && m_pGameInstance->Mouse_Pressing(DIM_LB))
	{
		Update_Position();
	}

	if (nullptr != m_pBar)
		m_pBar->Tick(fTimeDelta);

	
	_float fMin = (g_iWinSizeX >> 1) - fRANGE;
	_float fMax = (g_iWinSizeX >> 1) + fRANGE;

	_float fOffSet = (m_fX - fMin) / (fMax - fMin);

	//m_pGameInstance->SetChannelVolume(SOUND_BGM, fOffSet);

}

void CUI_Setting_Sound::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, ELEVENTH);

	if (nullptr != m_pBar)
		m_pBar->Late_Tick(fTimeDelta);
}

HRESULT CUI_Setting_Sound::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("SOUND"),
		_float2((g_iWinSizeX >> 1) - 45.f, (g_iWinSizeY >> 1) + 110.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return S_OK;

	return S_OK;
}

HRESULT CUI_Setting_Sound::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Sound"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Sound::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Sound::Create_Bar()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = (g_iWinSizeX >> 1);
	pDesc.fY = (g_iWinSizeY >> 1) + 170.f;
	pDesc.fSizeX = 600.f;
	pDesc.fSizeY = 31.f;
	
	m_pBar = dynamic_cast<CUI_Setting_SoundBar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_SoundBar"), &pDesc));
	if (nullptr == m_pBar)
		return E_FAIL;

	return S_OK;
}

void CUI_Setting_Sound::Update_Position()
{
	POINT mouse = m_pGameInstance->Get_Mouse_Point();
	m_fX = mouse.x;

	if (m_fX < (g_iWinSizeX >> 1) - fRANGE)
	{
		m_fX = (g_iWinSizeX >> 1) - fRANGE;
	}
	else if (m_fX > (g_iWinSizeX >> 1) + fRANGE)
	{
		m_fX = (g_iWinSizeX >> 1) + fRANGE;
	}

	Setting_Position();
}

CUI_Setting_Sound* CUI_Setting_Sound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Setting_Sound* pInstance = new CUI_Setting_Sound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Setting_Sound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Setting_Sound::Clone(void* pArg)
{
	CUI_Setting_Sound* pInstance = new CUI_Setting_Sound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Setting_Sound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Setting_Sound::Free()
{
	__super::Free();

	Safe_Release(m_pBar);
}

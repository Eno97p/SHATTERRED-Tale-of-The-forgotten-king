#include "UI_LoadingCircle.h"

#include "GameInstance.h"

CUI_LoadingCircle::CUI_LoadingCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LoadingCircle::CUI_LoadingCircle(const CUI_LoadingCircle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LoadingCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LoadingCircle::Initialize(void* pArg)
{
	UI_CIRCLE_DESC* pDesc = static_cast<UI_CIRCLE_DESC*>(pArg);

	m_eCircleType = pDesc->eCircleType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	m_vFontColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	Setting_Data();

	return S_OK;
}

void CUI_LoadingCircle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LoadingCircle::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Turn_Animation(fTimeDelta);

	Change_FontColor(fTimeDelta);

}

void CUI_LoadingCircle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH);
}

HRESULT CUI_LoadingCircle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if(CIRCLE_TWO == m_eCircleType)
		Render_Font();

	return S_OK;
}

HRESULT CUI_LoadingCircle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingCircle"),  
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingCircle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCircleType)))
		return E_FAIL;

	_float4x4 rotationMatrix;
	XMStoreFloat4x4(&rotationMatrix, m_RotationMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RotationMatrix", &rotationMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;*/

	return S_OK;
}

void CUI_LoadingCircle::Turn_Animation(_float fTimeDelta)
{
	if(CIRCLE_ONE == m_eCircleType)
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(5.f);
	else
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-5.f);

	if (m_fRotationAngle > XM_2PI)
		m_fRotationAngle -= XM_2PI;

	m_RotationMatrix = XMMatrixRotationZ(m_fRotationAngle); // XMMatrix
}

void CUI_LoadingCircle::Render_Font()
{
	//Setting_Text();
	// 여기로 돌아왔을 때 어째서!!!!!!! 다시 m_wstrTex로 돌아가는가?
	// 어째서어째서어째서? 왜 분명 Setting_Data( )에서 제대로 된 값으로 넘어가는 걸 확인했는데
	// 로딩 화면으로 넘어가면 아래의 코드에서는 그대로인가???

	// 값 바꾸고 나서 Level Loading이 되어서 여기로 들어오면 1로 초기화가 되어있는?ㅠㅠ

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight15"), m_wstrText,
		_float2((g_iWinSizeX>> 1) - 340.f, (g_iWinSizeY >> 1) - 65.f), XMVectorSet(m_fFontRGB, m_fFontRGB, m_fFontRGB, 1.f))))
		return;
}

void CUI_LoadingCircle::Change_FontColor(_float fTimeDelta)
{
	m_fFontTimer += fTimeDelta;


	if (m_isFontOn)
	{
		m_fFontRGB += 0.02f;
		if (m_fFontRGB >= 1.f)
			m_fFontRGB = 1.f;
		if (5.f <= m_fFontTimer)
		{
			m_isFontOn = false;
			m_fFontTimer = 0.f;
		}
	}
	else
	{
		m_fFontRGB -= 0.02f;
		if (m_fFontRGB <= 0.f)
			m_fFontRGB = 0.f;
		if (0.4f <= m_fFontTimer)
		{
			m_isFontOn = true;
			m_fFontTimer = 0.f;
		}
	}
}

void CUI_LoadingCircle::Setting_Data()
{
	// 여기서 값을 바꾸는 거 자체는 잘 됨.... 근데 문제는 ^^^
	if (0 == m_iTextNum)
	{
		m_wstrText = TEXT("              자칭 GEODESIAN이라는 자에 대해 알려진 것은 거의 없다.\n"
			TEXT("                               그는 과학자라고도, 학자라고도 불린다.\n")
			TEXT("그는 GEODESIC 게이트라는 복잡한 구조물의 창조를 인정할 수 있을 것인가?\n")
			TEXT("                                        스스로를 창조자로 여길 것인가."));
	}
	else if (1 == m_iTextNum)
	{
		m_wstrText = TEXT("                프로메테우스는 우주의 비밀을 풀어냈으며 모든 것을 깨우쳤다.\n"
			TEXT("                               히프노스의 가장 먼 경치 너머까지도.\n")
			TEXT("자신의 지식에 대한 사색에 잠긴 그녀는 주변에 대한 인식을 서서히 잃어갔다.\n"));
	}
	else if (2 == m_iTextNum)
	{
		m_wstrText = TEXT("그대의 발걸음이 오래된 성곽의 잔해를 밟고 신비한 왕의 유산을 찾아가는 순간,\n"
			TEXT("                                    어둠 속에서도 빛을 발견하리라.\n")
			TEXT("               왕의 전설을 되살려 이 고대의 땅에서 새로운 시작을 선언하라.\n")
			TEXT("           깨어진 세계의 비밀을 풀어라. 왕의 기억이 그대를 기다리고 있다.\n\n")
			TEXT("                            어둠 속에서 진실을 밝혀낼 용기가 있는가?"));
	}
	else if (3 == m_iTextNum)
	{
		m_wstrText = TEXT("                                                역사적인 여정의 시작.\n"
			TEXT("잊혀진 왕의 황금 시대를 되찾기 위해서는 과거의 비밀과 현재의 용기가 필요하다.\n")
			TEXT("    전설의 미래를 위한 결정적인 순간에 그대의 선택이 모든 것을 바꿀 것이다.\n"));
	}

	m_iTextNum++;
}

void CUI_LoadingCircle::Setting_Text()
{
	//if (0 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("              자칭 GEODESIAN이라는 자에 대해 알려진 것은 거의 없다.\n"
	//		TEXT("                               그는 과학자라고도, 학자라고도 불린다.\n")
	//		TEXT("그는 GEODESIC 게이트라는 복잡한 구조물의 창조를 인정할 수 있을 것인가?\n")
	//		TEXT("                                        스스로를 창조자로 여길 것인가."));
	//}
	//else if (1 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("                프로메테우스는 우주의 비밀을 풀어냈으며 모든 것을 깨우쳤다.\n"
	//		TEXT("                               히프노스의 가장 먼 경치 너머까지도.\n")
	//		TEXT("자신의 지식에 대한 사색에 잠긴 그녀는 주변에 대한 인식을 서서히 잃어갔다.\n"));
	//}
	//else if (2 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("그대의 발걸음이 오래된 성곽의 잔해를 밟고 신비한 왕의 유산을 찾아가는 순간,\n"
	//		TEXT("                                    어둠 속에서도 빛을 발견하리라.\n")
	//		TEXT("               왕의 전설을 되살려 이 고대의 땅에서 새로운 시작을 선언하라.\n")
	//		TEXT("           깨어진 세계의 비밀을 풀어라. 왕의 기억이 그대를 기다리고 있다.\n\n")
	//		TEXT("                            어둠 속에서 진실을 밝혀낼 용기가 있는가?"));
	//}
	//else if (3 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("                                                역사적인 여정의 시작.\n"
	//		TEXT("잊혀진 왕의 황금 시대를 되찾기 위해서는 과거의 비밀과 현재의 용기가 필요하다.\n")
	//		TEXT("    전설의 미래를 위한 결정적인 순간에 그대의 선택이 모든 것을 바꿀 것이다.\n"));
	//}
}

CUI_LoadingCircle* CUI_LoadingCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LoadingCircle::Clone(void* pArg)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingCircle::Free()
{
	__super::Free();
}

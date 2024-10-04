#include "LineTest.h"
#include "GameInstance.h"
CLineTest::CLineTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CLineTest::CLineTest(const CLineTest& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CLineTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLineTest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLineTest::Priority_Tick(_float fTimeDelta)
{
}

void CLineTest::Tick(_float fTimeDelta)
{
}

void CLineTest::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLineTest::Render()
{
	return S_OK;
}

HRESULT CLineTest::Add_Components()
{
	return S_OK;
}

HRESULT CLineTest::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CLineTest::Bind_BlurResources()
{
	return S_OK;
}

CLineTest* CLineTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLineTest* pInstance = new CLineTest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLineTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLineTest::Clone(void* pArg)
{
	CLineTest* pInstance = new CLineTest(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLineTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLineTest::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
}

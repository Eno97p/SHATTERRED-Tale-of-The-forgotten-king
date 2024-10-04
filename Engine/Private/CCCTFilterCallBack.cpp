#include "CCCTFilterCallBack.h"

CCCTFilterCallBack::CCCTFilterCallBack()
{
    
}

HRESULT CCCTFilterCallBack::Initialize_Prototype(const FilterCallBack& callback)
{
    m_FilterCallBack = callback;
    return S_OK;
}

bool CCCTFilterCallBack::filter(const PxController& a, const PxController& b)
{
    if (m_FilterCallBack)
        return m_FilterCallBack(a, b);

    return true;
    
}

CCCTFilterCallBack* CCCTFilterCallBack::Create(const FilterCallBack& callback)
{
    CCCTFilterCallBack* pInstance = new CCCTFilterCallBack();
    if(FAILED(pInstance->Initialize_Prototype(callback)))
	{
        MSG_BOX("Failed to Created : CCCTFilterCallBack");
        Safe_Release(pInstance);
        return nullptr;
	}
    



    return pInstance;
}

void CCCTFilterCallBack::Free()
{

}

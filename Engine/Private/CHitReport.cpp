#include "CHitReport.h"



CHitReport* CHitReport::m_pInstance = nullptr;
std::mutex CHitReport::m_mutex;



CHitReport::CHitReport()
	:PxUserControllerHitReport()
{
}


HRESULT CHitReport::Initialize_Prototype()
{
	return S_OK;
}

void CHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
	std::lock_guard<std::mutex> lock(m_mutex); // lock_guard¸¦ »ç¿ëÇÏ¿© mutex ¶ôÀ» È¹µæ
	if (m_ShapeHitCallBack)
	{

		m_ShapeHitCallBack(hit);
	}
	
}

void CHitReport::onControllerHit(const PxControllersHit& hit)
{
	std::lock_guard<std::mutex> lock(m_mutex); // lock_guard¸¦ »ç¿ëÇÏ¿© mutex ¶ôÀ» È¹µæ
	if (m_ControllerHitCallBack)
	{
		m_ControllerHitCallBack(hit);

	}
}

void CHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
	std::lock_guard<std::mutex> lock(m_mutex); // lock_guard¸¦ »ç¿ëÇÏ¿© mutex ¶ôÀ» È¹µæ
		if(m_ObstacleHitCallBack)
		m_ObstacleHitCallBack(hit);
}



CHitReport* CHitReport::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (nullptr == m_pInstance)
			m_pInstance = new CHitReport;

	}
	return m_pInstance;
}

void CHitReport::DestroyInstance()
{

	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

}


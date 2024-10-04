#include "CRenderWorker.h"


CRenderWorker::CRenderWorker(size_t iNumThreadPool, ID3D11Device* pDevice)
	:m_iNumThreadPool(iNumThreadPool)
{

}

void CRenderWorker::Shutdown()
{
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_stop = true;
	}
	m_condition.notify_all();
	for (std::thread& worker : m_workers)
		worker.join();
}





HRESULT CRenderWorker::Initialize()
{
	for (size_t i = 0; i < m_iNumThreadPool; ++i)
	{
		m_workers.emplace_back(&CRenderWorker::RenderWorkerThread, this);
	}

	return S_OK;
}

void CRenderWorker::RenderWorkerThread()
{
	while (true)
	{
		function<void()> job;
		{
			unique_lock<std::mutex> lock(m_queueMutex);
			m_condition.wait(lock, [this] { return m_stop || !m_jobs.empty(); });
			if (m_stop && m_jobs.empty())
				return;
			job = std::move(m_jobs.front());
			m_jobs.pop();
		}
		job();
	}
}




CRenderWorker* CRenderWorker::Create(size_t iNumThreadPool, ID3D11Device* pDevice)
{
	CRenderWorker* pInstance = new CRenderWorker(iNumThreadPool, pDevice);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderWorker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderWorker::Free()
{
	Shutdown();



}

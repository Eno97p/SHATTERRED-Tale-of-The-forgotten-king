#include "CWorker.h"


CWorker::CWorker(size_t iNumThreadPool)
	: m_ThreadCount(iNumThreadPool)
	, m_bIsShutdown(false)
{
	
}


//
//template<typename T, typename... Args>
//auto CWorker::Add_Job(T&& Func, Args&&...args) -> future<decltype(Func(args...))>
//{
//	using return_type = decltype(Func(args...));
//	auto job=make_shared<packaged_task<return_type()>>(bind(forward<T>(Func), forward<Args>(args)...));
//	future<return_type> result = job->get_future();
//
//	{
//		lock_guard<mutex> lock(m_JobMutex);
//		m_Jobs.push([job]() {(*job)(); });
//
//
//	}
//
//	m_condition.notify_one();
//
//	return result;
//}


void CWorker::Shutdown()
{

	
	{
		unique_lock<mutex> lock(m_JobMutex);
		m_bIsShutdown = true;
	}
	m_condition.notify_all();

	for (auto& worker : m_Workers)
	{
		if(worker.joinable())
			worker.join();
	}


}

HRESULT CWorker::Initialize()
{
	m_Workers.reserve(m_ThreadCount);
	for(size_t i = 0; i < m_ThreadCount; ++i)
	{
		m_Workers.emplace_back(&CWorker::WorkerThread, this);


	}



	return S_OK;

}

void CWorker::WorkerThread()
{

	std::thread::id this_id = std::this_thread::get_id();
	std::cerr << "Thread " << this_id << " started." << std::endl;

	
		std::unique_lock<std::mutex> lock(m_JobMutex);
		while (true)
		{
			std::function<void()> job;
			{
				m_condition.wait(lock, [this] { return m_bIsShutdown || !m_Jobs.empty(); });
				if (m_bIsShutdown && m_Jobs.empty())
					break;
				if (!m_Jobs.empty())
				{
					job = std::move(m_Jobs.front());
					m_Jobs.pop();
				}
					
				
			}

			if (job)
			{
				try
				{
					job();
				}
				catch (const std::exception& e)
				{
					std::cerr << "Exception in WorkerThread " << this_id << ": " << e.what() << std::endl;
				}
				catch (...)
				{
					std::cerr << "Unknown exception in WorkerThread " << this_id << "." << std::endl;
				}
			}
		}
	
		
		std::cerr << "Thread " << this_id << " finished." << std::endl;

	//while (true)
	//{
	//	unique_lock<mutex> lock(m_JobMutex);
	//	function<void()> job;
	//	{	//중괄호가 왜 있어야 하는가? 
	//		//lock을 걸어서 m_Jobs에 접근하는 부분을 보호한다.
	//		m_condition.wait(lock, [this] {return m_bIsShutdown || !m_Jobs.empty(); });
	//		if (m_bIsShutdown && m_Jobs.empty())
	//			return;
	//		job = move(m_Jobs.front());
	//		m_Jobs.pop();
	//		m_JobMutex.unlock();
	//	}
	//	job();
	//
	//}

}

CWorker* CWorker::Create(size_t iNumThreadPool)
{
	CWorker* pInstance = new CWorker(iNumThreadPool);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : Calculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorker::Free()
{
	Shutdown();
	


}

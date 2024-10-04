#pragma once
#include "Base.h"


/*
https://github.com/progschj/ThreadPool
https://koreanfoodie.me/945


*/


BEGIN(Engine)
class ENGINE_DLL CRenderWorker final : public CBase
{
private:
	CRenderWorker(size_t iNumThreadPool, ID3D11Device* pDevice);
	virtual ~CRenderWorker() = default;

public:
	void Shutdown();
	void RenderWorkerThread();
	void Add_WorkerThread(function<void()> job);
public:
	template<class F>
	void EnqueueJob(F&& f)
	{
		{
			unique_lock<mutex> lock(m_queueMutex);
			m_jobs.emplace(std::forward<F>(f));
		}
		m_condition.notify_one();
	}

private:
	HRESULT Initialize();
	


private:
	size_t m_iNumThreadPool = 0;


	vector<thread> m_workers;
	queue<function<void()>> m_jobs;
	mutex m_queueMutex;
	condition_variable m_condition;
	bool m_stop;

public:
	static CRenderWorker* Create(size_t iNumThreadPool, ID3D11Device* pDevice);
	virtual void Free() override;



};

END

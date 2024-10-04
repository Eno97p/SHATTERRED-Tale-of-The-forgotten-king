#pragma once
#include "Base.h"


/*
https://github.com/progschj/ThreadPool
https://koreanfoodie.me/945


*/


BEGIN(Engine)
class ENGINE_DLL CWorker final : public CBase
{
private:
	CWorker(size_t iNumThreadPool);
	virtual ~CWorker() = default;



public:
	//move()�Լ��� ã�� �� ��
	template<typename T, typename... Args>
	auto Add_Job(T&& Func, Args&&... args) -> future<decltype(Func(args...))>	//���� ��ȯ Ÿ�� ����		//�Լ��� ��ȯ Ÿ���� �Լ� ����ڿ� ��� �� �� �ְ� ����
	{
		using return_type = decltype(Func(args...));
		auto job = make_shared<packaged_task<return_type()>>(bind(forward<T>(Func), forward<Args>(args)...));
		future<return_type> result = job->get_future();

		{
			lock_guard<mutex> lock(m_JobMutex);
			m_Jobs.push([job]() {(*job)(); });


		}

		m_condition.notify_one();

		return result;
	}


	//������ ���� //�Ϻ��� ���� ����	
	//future: �񵿱� ���α׷����� ���� ���ø� Ŭ����
	//������ Ǯ�̳� �۾� ť�ּ� �����ϰ� ���
	// 
	//�񵿱������� ����Ǵ� �۾��� ����� ����
	//�پ��� �񵿱� �۾��� ���� ���� 
	// - td::async, std::packaged_task, std::promise ��� �Բ� ���
	//�񵿱� �۾����� �߻��� ���ܸ� ���� ������� ������ �� ����
	//����Ȯ�� 
	// - valid(), get_state(), wait(), wait_for(), wait_until() ���� ��� �Լ��� ����
	//��� ���
	// - get() ��� �Լ��� ���� �񵿱� �۾��� ����� ��ٸ��� �޾ƿ� �� ����




	void Shutdown();
	//void WorkTask();


private:
	HRESULT Initialize();
	void WorkerThread();


private:
	size_t m_ThreadCount = 0;

	vector<thread> m_Workers;

	//�ؾ��� �ϵ��� �����ϴ� ť
	queue<function<void()>> m_Jobs;
	mutex m_JobMutex;
	condition_variable m_condition;

	_bool m_bIsShutdown = false;
public:
	static CWorker* Create(size_t iNumThreadPool);
	virtual void Free() override;



};

END

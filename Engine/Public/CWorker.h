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
	//move()함수를 찾아 볼 것
	template<typename T, typename... Args>
	auto Add_Job(T&& Func, Args&&... args) -> future<decltype(Func(args...))>	//후행 반환 타입 선언		//함수의 반환 타입을 함수 선언뒤에 명시 할 수 있게 해줌
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


	//우측값 참조 //완벽한 전달 패턴	
	//future: 비동기 프로그래밍을 위한 템플릿 클래스
	//스레드 풀이나 작업 큐애서 유용하게 사용
	// 
	//비동기적으로 실행되는 작업의 결과를 저장
	//다양한 비동기 작업과 연동 가능 
	// - td::async, std::packaged_task, std::promise 등과 함께 사용
	//비동기 작업에서 발생한 예외를 메인 스레드로 전파할 수 있음
	//상태확인 
	// - valid(), get_state(), wait(), wait_for(), wait_until() 등의 멤버 함수를 제공
	//결과 대기
	// - get() 멤버 함수를 통해 비동기 작업의 결과를 기다리고 받아올 수 있음




	void Shutdown();
	//void WorkTask();


private:
	HRESULT Initialize();
	void WorkerThread();


private:
	size_t m_ThreadCount = 0;

	vector<thread> m_Workers;

	//해야할 일들을 저장하는 큐
	queue<function<void()>> m_Jobs;
	mutex m_JobMutex;
	condition_variable m_condition;

	_bool m_bIsShutdown = false;
public:
	static CWorker* Create(size_t iNumThreadPool);
	virtual void Free() override;



};

END

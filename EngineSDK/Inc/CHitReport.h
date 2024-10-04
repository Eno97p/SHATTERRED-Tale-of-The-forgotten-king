#pragma once
#include"Base.h"




//직접 물리적인 캐릭터 컨트롤러 충돌을 제어해야 할까?? 한다면 해당 클래스를 사용하면 될 듯.
BEGIN(Engine)
class ENGINE_DLL CHitReport :
	public PxUserControllerHitReport


{
private:
	static CHitReport* m_pInstance;
	//Physx에서 내부적으로 멀티스레드를 사용할 수 있으므로 이를 위한 뮤텍스를 사용한다.
	static std::mutex m_mutex;

public:
	using ShapeHitCallBack= std::function<void(const PxControllerShapeHit& hit)>;
	using ControllerHitCallBack = std::function<void(const PxControllersHit& hit)>;
	using ObstacleHitCallBack = std::function<void(const PxControllerObstacleHit& hit)>;


private:
	 CHitReport();
	 CHitReport(const CHitReport& rhs) = delete;
	 //CHitReport& operator=(const CHitReport& rhs) = delete;

	 //PxPairFlag
public:
	virtual ~CHitReport() = default;


public:
	HRESULT Initialize_Prototype();


public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

public:
	void SetShapeHitCallback(ShapeHitCallBack callback) { 
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard를 사용하여 mutex 락을 획득
		m_ShapeHitCallBack = callback; }
	void SetControllerHitCallback(ControllerHitCallBack callback) {
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard를 사용하여 mutex 락을 획득
		m_ControllerHitCallBack = callback; }
	void SetObstacleHitCallback(ObstacleHitCallBack callback) { 
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard를 사용하여 mutex 락을 획득
		m_ObstacleHitCallBack = callback; }


private:
	ShapeHitCallBack m_ShapeHitCallBack;
	ControllerHitCallBack m_ControllerHitCallBack;
	ObstacleHitCallBack m_ObstacleHitCallBack;





public:
	static CHitReport* GetInstance();
	static void DestroyInstance();





};

END

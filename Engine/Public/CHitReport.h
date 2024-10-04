#pragma once
#include"Base.h"




//���� �������� ĳ���� ��Ʈ�ѷ� �浹�� �����ؾ� �ұ�?? �Ѵٸ� �ش� Ŭ������ ����ϸ� �� ��.
BEGIN(Engine)
class ENGINE_DLL CHitReport :
	public PxUserControllerHitReport


{
private:
	static CHitReport* m_pInstance;
	//Physx���� ���������� ��Ƽ�����带 ����� �� �����Ƿ� �̸� ���� ���ؽ��� ����Ѵ�.
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
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard�� ����Ͽ� mutex ���� ȹ��
		m_ShapeHitCallBack = callback; }
	void SetControllerHitCallback(ControllerHitCallBack callback) {
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard�� ����Ͽ� mutex ���� ȹ��
		m_ControllerHitCallBack = callback; }
	void SetObstacleHitCallback(ObstacleHitCallBack callback) { 
		std::lock_guard<std::mutex> lock(m_mutex); // lock_guard�� ����Ͽ� mutex ���� ȹ��
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

#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	/*const*/ list<class CLight*> Get_Lights() { return m_Lights; }
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

	//Light On Off 기능 : Light Off하면 렌더 연산 안함. 씬 전환시마다 이전 씬에 사용 중인 조명들 꺼줘야함
	//Add_Light으로 추가하면 일단 On으로 해놓음
	void LightOff(_uint iIndex);
	void LightOn(_uint iIndex);

	void LightOff_All();
	//Point Light 포지션 옮기는만 함수. 몇번째 점조명을 옮길건지 인덱스 필요함
	void Update_LightPos(_uint iIndex, _vector LightPos);

	//맵툴용 조명 작업 함수
	
	//Dir
	void Edit_Light(_uint iIndex, LIGHT_DESC* desc);

public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Light_Clear();

private:
	list<class CLight*>			m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END
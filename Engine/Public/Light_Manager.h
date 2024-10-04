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

	//Light On Off ��� : Light Off�ϸ� ���� ���� ����. �� ��ȯ�ø��� ���� ���� ��� ���� ����� �������
	//Add_Light���� �߰��ϸ� �ϴ� On���� �س���
	void LightOff(_uint iIndex);
	void LightOn(_uint iIndex);

	void LightOff_All();
	//Point Light ������ �ű�¸� �Լ�. ���° �������� �ű���� �ε��� �ʿ���
	void Update_LightPos(_uint iIndex, _vector LightPos);

	//������ ���� �۾� �Լ�
	
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
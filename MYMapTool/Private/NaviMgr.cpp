#include "NaviMgr.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"

#include "Cell.h"

IMPLEMENT_SINGLETON(CNaviMgr)

CNaviMgr::CNaviMgr()
{
}

HRESULT CNaviMgr::Load_NaviData(CNavigation* pNavigationCom)
{
	// ó�� MapTool ������ �� �ε�� �׺���̼� ������ ��ƿ���
	//for (auto& pCell : pNavigationCom->Get_Cells())
	//	m_vecCell.emplace_back(pCell);

	//Update_NaviName();

	return S_OK;
}

void CNaviMgr::Input_NaviData(CNavigation* pNavigationCom, _vector vPos)
{
	//_float3 fNaviPos = _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);

	//m_NaviList[m_iCount] = fNaviPos;
	//Check_Around(m_NaviList[m_iCount]);
	//++m_iCount;

	//if (3 == m_iCount)
	//{ 
	//	pNavigationCom->Update_Navi_InMapTool(m_NaviList); // NaviCom�� Cell �߰�
	//	m_vecCell.emplace_back(pNavigationCom->Get_Cells().back()); // NaviCom�� ������ ���Ҹ� Navi Mgr�� Cell vector�� �߰�

	//	//Update_NaviName(); // Imgui�� Cell ��� ����
	//	CImgui_Manager::GetInstance()->Add_vecCreateCell(m_vecCell.size());

	//	m_iCount = 0;
	//}
}

void CNaviMgr::Check_Around(_float3& vPos)
{
	//for (auto& pCell : m_vecCell)
	//{
	//	_vector vOriginPosA = pCell->Get_Point(CCell::POINT_A);
	//	_vector vOriginPosB = pCell->Get_Point(CCell::POINT_B);
	//	_vector vOriginPosC = pCell->Get_Point(CCell::POINT_C);

	//	_float fDistanceA = Calculate_Distance(vPos, vOriginPosA);
	//	_float fDistanceB = Calculate_Distance(vPos, vOriginPosB);
	//	_float fDistanceC = Calculate_Distance(vPos, vOriginPosC);

	//	// �� �� �� 3.f ������ �Ÿ��� �ִٸ� �ش� �������� ����
	//	if (3.f >= fDistanceA)
	//		vPos = _float3(vOriginPosA.m128_f32[0], vOriginPosA.m128_f32[1], vOriginPosA.m128_f32[2]);
	//	else if (3.f >= fDistanceB)
	//		vPos = _float3(vOriginPosB.m128_f32[0], vOriginPosB.m128_f32[1], vOriginPosB.m128_f32[2]);
	//	else if (3.f >= fDistanceC)
	//		vPos = _float3(vOriginPosC.m128_f32[0], vOriginPosC.m128_f32[1], vOriginPosC.m128_f32[2]);
	//}
}

_float CNaviMgr::Calculate_Distance(_float3 vNewPos, _vector vOriginPos)
{
	/*_float fDistanceX = vNewPos.x - vOriginPos.m128_f32[0];
	_float fDistanceZ = vNewPos.z - vOriginPos.m128_f32[2];
	
	return sqrt((fDistanceX*fDistanceX) + (fDistanceZ * fDistanceZ));*/
	return 0.f;
}

void CNaviMgr::Update_NaviName()
{
	//CImgui_Manager::GetInstance()->Clear_vecCell(); // Imgui�� Cell ��� �ʱ�ȭ >>>> ���� �߻���

	for (size_t i = 0; i < m_vecCell.size(); ++i) // NaviMgr�� Cell ������ŭ for�� ���� Imgui�� vecCellName ����Ʈ ����
	{
		CImgui_Manager::GetInstance()->Add_vecCreateCell(i);
	}
}

HRESULT CNaviMgr::Save_NaviData()
{
	// Imgui���� ���� ������ Stage �� ���ͼ� ���� �̸� ���ϱ�
	/*_int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();
	_tchar wszStage[MAX_PATH] = L"";
	_ulong dwByte = {};

	switch (iStageIdx)
	{
	case STAGE_HOME:
		wcscpy_s(wszStage, L"../../Data/Navi_Home.dat");
		break;
	case STAGE_ONE:
		wcscpy_s(wszStage, L"../../Data/Navi_Stage1.dat");
		break;
	case STAGE_TWO:
		wcscpy_s(wszStage, L"../../Data/Navi_Stage2.dat");
		break;
	case STAGE_THREE:
		wcscpy_s(wszStage, L"../../Data/Navi_Stage3.dat");
		break;
	case STAGE_BOSS:
		wcscpy_s(wszStage, L"../../Data/Navi_Boss.dat");
		break;
	default:
		MSG_BOX("Setting Navi File Name is Failed");
		break;
	}

	HANDLE hFile = CreateFile(wszStage, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	for (auto& NaviPos : m_vecCell)
	{
		_float3 vPosA = _float3(NaviPos->Get_Point(CCell::POINT_A).m128_f32[0], NaviPos->Get_Point(CCell::POINT_A).m128_f32[1], NaviPos->Get_Point(CCell::POINT_A).m128_f32[2]);
		_float3 vPosB = _float3(NaviPos->Get_Point(CCell::POINT_B).m128_f32[0], NaviPos->Get_Point(CCell::POINT_B).m128_f32[1], NaviPos->Get_Point(CCell::POINT_B).m128_f32[2]);
		_float3 vPosC = _float3(NaviPos->Get_Point(CCell::POINT_C).m128_f32[0], NaviPos->Get_Point(CCell::POINT_C).m128_f32[1], NaviPos->Get_Point(CCell::POINT_C).m128_f32[2]);

		WriteFile(hFile, &vPosA, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &vPosB, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &vPosC, sizeof(_float3), &dwByte, nullptr);
	}

	CloseHandle(hFile);*/

	return S_OK;
}

void CNaviMgr::Check_Cell(_int iCellIndex)
{
	// Idex�� �ش��ϴ� Cell ����
	/*vector<_float3>::iterator iter = m_vecNavi.begin();

	for (size_t i = 0; i < iCellIndex; ++i)
	{
		++iter;
	}*/
	// Imgui���� �޾ƿ� �ε����� 
	// ���� �� �Լ��� ��ĥ ������ ������?

}

void CNaviMgr::Delete_Cell( _int iCellIndex) // ���ڷ� ���� Index�� Cell�� ����
{
	vector<CCell*>::iterator iter = m_vecCell.begin();

	for (size_t i = 0; i < iCellIndex; ++i)
	{
		++iter;
	}
	Safe_Release(*iter);
	m_vecCell.erase(iter);
}

void CNaviMgr::Clear_Cell()
{
	for (auto& pCell : m_vecCell)
		Safe_Release(pCell);
	m_vecCell.clear();
}

void CNaviMgr::Update_Cell()
{
}

void CNaviMgr::Free()
{
}

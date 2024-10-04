#include "EventTrigger.h"
#include "Map_Element.h"
#include "GameInstance.h"
#include "Player.h"
#include "Elevator.h"
#include "Level_Loading.h"

#include "SideViewCamera.h"
#include "TransitionCamera.h"
#include "CutSceneCamera.h"

#include "UI_Manager.h"
#include "UI_FadeInOut.h"
#include "LandObject.h"
#include "EffectManager.h"

#include "Boss_Juggulus.h"
#include "Andras.h"
#include "Malkhel.h"

CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{

		CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		//m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		
		//TRIGGER STATE SET
		m_eTRIGState = (TRIGGER_TYPE)pDesc->TriggerType;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CEventTrigger::Tick(_float fTimeDelta)
{
	

//	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Collider")));
	if (nullptr != pPlayerCollider)
	{
		if (CCollider::COLL_START == m_pColliderCom->Intersect(pPlayerCollider))
		{
			if (m_eTRIGState == TRIG_SCENE_CHANGE)
			{
				m_bSceneChange = true;
				CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

				pDesc.isFadeIn = false;
				pDesc.isLevelChange = true;
				pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

				list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"));
				if (objs.empty())
				{
					{
						if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
							return;
					}
				}

				// Portal UI 제거
				CUI_Manager::GetInstance()->Delete_PortalUI();
			}
			else if (m_eTRIGState == TRIG_SCENE_CHANGE_FOR_ACKBAR)
			{
				m_bSceneChange = true;
				CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

				pDesc.isFadeIn = false;
				pDesc.isLevelChange = true;
				pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;
				pDesc.eNetLevel = LEVEL_ACKBAR;
				list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"));
				if (objs.empty())
				{
					{
						if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
							return;
					}
				}


			}
			else if (m_eTRIGState == TRIG_SCENE_CHANGE_FOR_JUGGLAS)
			{
				m_bSceneChange = true;
				CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

				pDesc.isFadeIn = false;
				pDesc.isLevelChange = true;
				pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;
				pDesc.eNetLevel = LEVEL_JUGGLAS;
				list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"));
				if (objs.empty())
				{
					{
						if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
							return;
					}
				}



			}
			else if (m_eTRIGState == TRIG_SCENE_CHANGE_FOR_ANDRASARENA)
			{
				m_bSceneChange = true;
				CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

				pDesc.isFadeIn = false;
				pDesc.isLevelChange = true;
				pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;
				pDesc.eNetLevel = LEVEL_ANDRASARENA;
				list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"));
				if (objs.empty())
				{
					{
						if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
							return;
					}
				}

				list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
				pPlayer->OnWater(false);
			}

			else
			{
				switch (m_eTRIGState)
				{
				case TRIG_TUTORIAL_BOSSENCOUNTER:
				{
					CMap_Element::MAP_ELEMENT_DESC pDesc{};
					_matrix vMat = { 1.4f, 0.f, 0.f, 0.f,
					0.f, 10.f, 0.f, 0.f,
					0.f, 0.f, 1.4f, 0.f,
					154.009f, 531.828f, 96.989f, 1.f };
					XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_BasicDonut");
					m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_FakeWall"), &pDesc);

					//컷씬 트렌지션
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_CUTSCENE;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					m_pGameInstance->Set_MainCamera(CAM_TRANSITION);
				}
				break;
				case TRIG_JUGGLAS_SPAWNSECONDROOM:
				{
					m_pGameInstance->Clear_Layer(LEVEL_JUGGLAS, TEXT("Layer_Trap"));

					CMap_Element::MAP_ELEMENT_DESC pDesc{};
					_float4x4* vMat = new _float4x4();
					XMStoreFloat4x4(vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
					pDesc.iInstanceCount = 1;
					pDesc.WorldMats.emplace_back(vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle2");

					// Add_CloneObject 함수가 pDesc의 내용을 복사한다고 가정
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);

					// pDesc.Cleanup() 대신 직접 메모리 해제
					for (auto& mat : pDesc.WorldMats)
					{
						delete mat;
					}
					pDesc.WorldMats.clear();

					//Safe_Delete(pDesc.WorldMats.front());

				}
				break;
				case TRIG_JUGGLAS_SPAWNTHIRDROOM:
				{
					//m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), 0));`
					m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), 6));
					CMap_Element::MAP_ELEMENT_DESC pDesc;
					_float4x4* vMat = new _float4x4();
					XMStoreFloat4x4(vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
					pDesc.iInstanceCount = 1;
					pDesc.WorldMats.emplace_back(vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle3");
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);

					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle4");
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);

					pDesc.Cleanup();  // 명시적으로 정리 작업 수행
					//Safe_Delete(pDesc.WorldMats.front());
				}
				break;
				case TRIG_VIEWCHANGE_TTOS:
				{
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_SIDEVIEW;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}
					//m_pGameInstance->Set_MainCamera(3);
				}
				break;
				case TRIG_VIEWCHANGE_STOT:
				{
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					//pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
					//pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_SIDEVIEW;
					pTCDesc.iEndCam = CAM_THIRDPERSON;
					pTCDesc.fTransitionTime = 1.f;

					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					//m_pGameInstance->Set_FogOption({ 0.272f, 0.252f, 0.367f, 1.f }, 160.f, 0.08f, 0.005f, 10.f, 1.f, 0.5f);

					//m_pGameInstance->Set_MainCamera(1);
				}
				break;
				case TRIG_ASCEND_ELEVATOR:
				{
					// FOG_DESC 설정
					CRenderer::FOG_DESC fogDesc{};
					fogDesc.vFogColor = { 0.154f, 0.115f, 0.211f, 1.f };
					fogDesc.vFogColor2 = { 0.814f, 0.814f, 0.814f, 1.f };
					fogDesc.fFogRange = 288.5f;
					fogDesc.fFogHeightFalloff = 5.f;
					fogDesc.fFogGlobalDensity = 0.351f;
					fogDesc.fFogTimeOffset = 0.577f;
					fogDesc.fFogTimeOffset2 = 2.019f;
					fogDesc.fNoiseIntensity = 1.010f;
					fogDesc.fNoiseIntensity2 = 3.125f;
					fogDesc.fNoiseSize = 0.008654f;
					fogDesc.fNoiseSize2 = 0.003365f;
					fogDesc.fFogBlendFactor = 0.457f;

					// FOG_DESC 적용 (구체적인 적용 방법은 m_pGameInstance의 구현에 따라 다를 수 있음)
					m_pGameInstance->Set_FogOption(fogDesc);

					//m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), 8));

					//// 보스 석상 소환
					//const _float4x4 statueMatrices[3] = {
					//	{ 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -410.189f, 67.966f, -2.195f, 1.f },
					//	{ -0.91f, 0.f, -0.415f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.415f, 0.f, -0.91f, 0.f, -420.326f, 67.976f, -17.686f, 1.f },
					//	{ 0.845f, 0.f, -0.536f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.536f, 0.f, 0.845f, 0.f, -420.068f, 67.932f, 13.209f, 1.f }
					//};

					//for (const auto& matrix : statueMatrices)
					//{
					//	CMap_Element::MAP_ELEMENT_DESC StatueDesc{};
					//	StatueDesc.mWorldMatrix = matrix;
					//	m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_BossStatue"), &StatueDesc);
					//}

					// 보스 소환
					auto pElevator = dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0));
					if (pElevator)
					{
						pElevator->Ascend(XMVectorSet(-310.f, 69.f, -1.5f, 1.f));
					}

					CLandObject::LANDOBJ_DESC desc{};
					XMStoreFloat4x4(&desc.mWorldMatrix, XMMatrixTranslation(-8.3f, 0.f, -2.4f));

					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Juggulus"), &desc)))
					{
						MSG_BOX("Failed to add Boss_Juggulus");
						return;
					}
				}
				break;
				case TRIG_DESCEND_ELEVATOR:
				{
					dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0))->Descend(XMVectorSet(-310.f, 5.6f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
				}
				break;
				case TRIG_VIEWCHANGE_TTOBS:
				{
					m_pGameInstance->StopAll();
					m_pGameInstance->Disable_Echo();
					//m_pGameInstance->PlayBGM(TEXT("BGM_Boss_Juggulus.mp3"));

					dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_CutSceneIdx(1);
					dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Set_BossScene(true);

					//컷씬 트렌지션
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_SIDEVIEW;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					m_pGameInstance->Set_MainCamera(CAM_TRANSITION);

					list<CGameObject*> juggulus = m_pGameInstance->Get_GameObjects_Ref(LEVEL_JUGGLAS, TEXT("Layer_Boss"));

					if (!juggulus.empty())
					{
						dynamic_cast<CBoss_Juggulus*>(juggulus.front())->Juggulus_Activate();
					}

				}
				break;
				case TRIG_CUTSCENE_GRASSLAND:
				{
					dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_CutSceneIdx(CCutSceneCamera::SCENE_GRASSLAND_HANGAROUND);

					//컷씬 트렌지션
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 10000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_CUTSCENE;
					pTCDesc.fTransitionTime = 2.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GRASSLAND, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}


					m_pGameInstance->Set_MainCamera(CAM_TRANSITION);

			
				}
				break;
				case TRIG_CUTSCENE_MALKHEL:
				{
					m_pGameInstance->StopAll();
					m_pGameInstance->Disable_Echo();
					//m_pGameInstance->PlayBGM(TEXT("BGM_Boss_Malkhel.mp3"));

					//플레이어 컷씬위치로
					CPhysXComponent_Character* playerTrans = dynamic_cast<CPhysXComponent_Character*>(m_pGameInstance->Get_Component(LEVEL_GRASSLAND, TEXT("Layer_Player"), TEXT("Com_PhysX")));
					playerTrans->Set_Position(XMVectorSet(-460.111f, 350.980f, -503.035f, 1.f));

					//풀 삭제
					m_pGameInstance->Clear_Layer(LEVEL_GRASSLAND, TEXT("Layer_Grass"));
					dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_CutSceneIdx(CCutSceneCamera::SCENE_BLOODMOON);

					//컷씬 트렌지션
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_CUTSCENE;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					m_pGameInstance->Set_MainCamera(CAM_TRANSITION);

					list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
					CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
					pPlayer->OnWater(true);
				}
					break;
				case TRIG_CUTSCENE_JUGGULUS:
				{
					dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Set_CutSceneIdx(CCutSceneCamera::SCENE_JUGGULAS);

					//컷씬 트렌지션
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]->Get_Far();

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_CUTSCENE;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					m_pGameInstance->Set_MainCamera(CAM_TRANSITION);
				}
					break;
				case TRIG_CUTSCENE_ANDRAS:
				{
					CAndras* andras = dynamic_cast<CAndras*>(m_pGameInstance->Get_GameObjects_Ref(LEVEL_ANDRASARENA, TEXT("Layer_Monster")).front());

					if (andras)
					{
						andras->Activate_Andras();
					}

					m_pGameInstance->StopAll();
					m_pGameInstance->Disable_Echo();
					//m_pGameInstance->PlayBGM(TEXT("BGM_Boss_Andras.mp3"), 0.3f);
					//블랙홀 생성
					_float4 vStartPosition = { 94.368f, 70.f, 343.791f, 1.f };
					EFFECTMGR->Generate_BlackHole(1, vStartPosition, LEVEL_ANDRASARENA);

					CUI_Manager::GetInstance()->Create_BossText(true);

				}
					break;

				case TRIG_RACING_START:
				{
					m_pGameInstance->StopAll();
					m_pGameInstance->Disable_Echo();
					//m_pGameInstance->PlayBGM(TEXT("NeedForSpeed.mp3"), 0.3f);
				}
				break;
				case TRIG_RACING_WIN:
				{

					m_pGameInstance->StopAll();
					m_pGameInstance->Disable_Echo();
					//m_pGameInstance->PlayBGM(TEXT("BGM_Grassland.mp3"));
					m_pGameInstance->Play_Effect_Sound(TEXT("fireworks-1.wav"), SOUND_EFFECT);
					//m_pGameInstance->Play_Effect_Sound(TEXT("fireworks-2.wav"), SOUND_EFFECT);
					//m_pGameInstance->Play_Effect_Sound(TEXT("fireworks-3.wav"), SOUND_EFFECT);
					m_pGameInstance->Play_Effect_Sound(TEXT("fireworks-4.wav"), SOUND_EFFECT);
					m_pGameInstance->Play_Effect_Sound(TEXT("tada-fanfare.mp3"), SOUND_EFFECT, 0.0f, 0.0f,0.8f);


				}
				break;
				default:
					break;
				}

				m_pGameInstance->Erase(this);
			}

		}
	}




}

HRESULT CEventTrigger::Render()
{
	return S_OK;
}

HRESULT CEventTrigger::Add_Components(void* pArg)
{

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_AABB;

	// 월드 매트릭스에서 스케일 추출
	_float3 vScale;
	vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	// 기본 크기에 스케일 적용
	ColliderDesc.vExtents = _float3(3.f * vScale.x, 3.f * vScale.y, 3.f * vScale.z);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{


	return S_OK;
}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
	CEventTrigger* pInstance = new CEventTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}

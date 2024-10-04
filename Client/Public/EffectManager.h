#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Particle_Trail.h"
#include "Particle_STrail.h"
#include "Distortion_Effect.h"
#include "Electronic.h"
#include "TornadoEffect.h"
#include "HealEffect.h"
#include "AndrasLazer.h"
#include "SwingEffect.h"
#include "Meteor.h"
#include "FirePillar.h"
#include "NeedleSpawner.h"
#include "GroundSlash.h"
#include "HammerSpawn.h"
#include "HexaShield.h"
#include "FireFlyCube.h"
#include "BlackHole.h"
#include "WellCylinder.h"
#include "MagicCast.h"
#include "CutSceneAndras.h"
BEGIN(Client)
class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)
private:
	CEffectManager();
	virtual ~CEffectManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Generate_Trail(const _int iIndex, const _float4x4* BindMat);	//1. 인덱스, 2.대상의 행렬
	CGameObject* Member_Trail(const _int iIndex, const _float4x4* BindMat);
	HRESULT Generate_SwordTrail(const _int iIndex, const _float4x4* Swordmat);  //1. 인덱스, 2. 칼 행렬
	CGameObject* Generate_Particle(const _int iIndex, const _float4 vStartpos,  //1. 인덱스, 2. 위치, 3. 타겟, 4. 회전축, 5.회전각도
		CGameObject* pTarget = nullptr,
		const _vector vAxis = XMVectorZero(),
		const _float fRadians = 0.f,
		const _vector vLook = XMVectorZero());
	HRESULT Generate_Distortion(const _int iIndex, const _float4 vStartpos);
	HRESULT Generate_Lightning(const _int iIndex, const _float4 vStartpos);
	HRESULT Generate_Tornado(const _int iIndex, const _float4 vStartpos, CGameObject* pTarget = nullptr);
	HRESULT Generate_HealEffect(const _int iIndex , const _float4x4* BindMat);
	HRESULT Generate_Magic_Cast(const _int iIndex, const _float4x4* BindMat);
	HRESULT Generate_Lazer(const _int iIndex, const _float4x4* BindMat);
	HRESULT Generate_Swing(const _int iIndex, const _float4x4* BindMat);
	HRESULT Generate_Meteor(const _float4 vStartPos);
	HRESULT Generate_FirePillar(const _float4 vStartPos);
	HRESULT Generate_Needle(const _float4 vStartPos);
	HRESULT Generate_GroundSlash(const _float4 vStartPos, const _float4 vDirection);
	HRESULT Generate_HammerSpawn(const _float4 vStartPos);
	HRESULT Generate_BlackHole(const _int iIndex, const _float4 vStartPos, const _uint Level);
	HRESULT Generate_WellCylinder(const _float4x4* BindMat);
	HRESULT Generate_CutSceneAndras(const _float4 vStartPos);


	CGameObject* Generate_HexaShield(const _float4x4* BindMat);	//CHexaShield로 캐스팅해서 맞을땐 Set_Shield_Hit() 호출 , 삭제할땐  Set_Delete()호출
	HRESULT Generate_FireFly(const _float4x4* BindMat);		//Player 월드행렬 주소 넣으면 됨

private:		//Load Values
	HRESULT Load_Trails();
	HRESULT Load_SwordTrails();
	HRESULT Load_Particles();
	HRESULT Load_Distortions();
	HRESULT Load_Lightnings();
	HRESULT Load_Tornados();
	HRESULT Load_Heals();
	HRESULT Load_Lazers();
	HRESULT Load_Swing();
	HRESULT Load_BlackHole();
	HRESULT Load_Meteor();
	HRESULT Load_FirePillar();
	HRESULT Load_Needle();
	HRESULT Load_GroundSlash();
	HRESULT Load_HammerSpawn();
	HRESULT Load_HexaShield();
	HRESULT Load_FireFly();
	HRESULT	Load_WellCylinder();
	HRESULT Load_Magic_Cast();
	HRESULT Load_CutSceneAndras();


	HRESULT Ready_GameObjects();
	HRESULT	Add_Texture_Prototype(const wstring& path, const wstring& name);
private:		//Free
	void Dynamic_Deallocation();

private:
	vector<pair<PARTICLETYPE, void*>>							m_Particles;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>>				Trailes;
	vector<shared_ptr<CSTrail::STRAIL_DESC>>					m_pSwordTrailes;
	vector<shared_ptr<CDistortionEffect::DISTORTIONEFFECT>>		m_pDistortions;
	vector<shared_ptr<CElectronic::ELECTRONICDESC>>				m_pLightnings;
	vector<shared_ptr<CTornadoEffect::TORNADODESC>>				m_Tornados;
	vector<shared_ptr<CHealEffect::HEALEFFECT>>					m_Heals;
	vector<shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>>	m_Lazers;
	vector<shared_ptr<CSwingEffect::SWINGEFFECT>>				m_Swings;
	vector<shared_ptr<CBlackHole::BLACKHOLE>>					m_BlackHoles;
	vector<shared_ptr<CMagicCast::MAGIC_CAST>>					m_MagicCast;

	shared_ptr<CMeteor::METEOR_DESC>							m_Meteor;
	shared_ptr<CFirePillar::FIREPILLAR>							m_FirePillar;
	shared_ptr<CNeedleSpawner::NEEDLESPAWNER>					m_Needle;
	shared_ptr<CGroundSlash::GROUNDSLASH>						m_GroundSlash;
	shared_ptr<CHammerSpawn::HAMMERSPAWN>						m_HammerSpawn;
	shared_ptr<CHexaShield::HEXASHIELD>							m_HexaShield;
	shared_ptr<CFireFlyCube::FIREFLYCUBE>						m_FireFly;
	shared_ptr<CWellCylinder::WELLCYLINDER>						m_WellCylinder;
	shared_ptr<CutSceneAndras::CUTSCENEANDRAS>					m_CutSceneAndras;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
public:
	virtual void Free() override;
};
END

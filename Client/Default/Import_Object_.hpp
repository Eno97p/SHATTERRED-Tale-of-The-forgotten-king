#pragma once

#include"Import_Class.h"




HRESULT Load_Object(CGameInstance* pGameInstance, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FreeCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ThirdPersonCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThirdPersonCamera"),
		CThirdPersonCamera::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SideViewCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SideViewCamera"),
		CSideViewCamera::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TransitionCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TransitionCamera"),
		CTransitionCamera::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CutSceneCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CutSceneCamera"),
		CCutSceneCamera::Create(pDevice, pContext))))
		return E_FAIL;


	///* For.Prototype_GameObject_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster ::Create(pDevice, pContext))))
	//	return E_FAIL;


	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Distortion"),
		CDistortion::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Clone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Clone"),
		CClone::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WhisperSword_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword_Anim"),
		CWhisperSword_Anim::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DurgaSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DurgaSword"),
		CDurgaSword::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PretorianSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PretorianSword"),
		CPretorianSword::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RadamantheSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RadamantheSword"),
		CRadamantheSword::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Passive_Element */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Passive_Element"),
		CPassive_Element::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Deco_Element */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Deco_Element"),
		CDeco_Element::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Active_Element */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Active_Element"),
		CActive_Element::Create(pDevice, pContext))))
		return E_FAIL;



#pragma region Active Element
	// Prototype_GameObject_TutorialMapBridge
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TutorialMapBridge"),
		CTutorialMapBridge::Create(pDevice, pContext))))
		return E_FAIL;


	// Prototype_GameObject_FakeWall
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FakeWall"),
		CFakeWall::Create(pDevice, pContext))))
		return E_FAIL;

	// Prototype_GameObject_Elevator
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elevator"),
		CElevator::Create(pDevice, pContext))))
		return E_FAIL;

	// Prototype_GameObject_RotateGate
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotateGate"),
		CRotateGate::Create(pDevice, pContext))))
		return E_FAIL;


	// Prototype_GameObject_BossDeco
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossDeco"),
		CBossDeco::Create(pDevice, pContext))))
		return E_FAIL;

	// Prototype_GameObject_BossStatue
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossStatue"),
		CBossStatue::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_EventTrigger
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EventTrigger"),
		CEventTrigger::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_Tree
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_Grass
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_Cloud
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cloud"),
		CCloud::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_Lagoon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lagoon"),
		CLagoon::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_BackGround_Card
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Card"),
		CBackGround_Card::Create(pDevice, pContext))))
		return E_FAIL;

	//Prototype_GameObject_BackGround_Moon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Moon"),
		CBackGround_Moon::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FallPlatform */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FallPlatform"),
		CFallPlatform::Create(pDevice, pContext))))
		return E_FAIL;

#pragma endregion Active Element

#pragma region Monster
	/* For.Prototype_GameObject_Andras */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Andras"),
		CAndras::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Body_Andras */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Andras"),
		CBody_Andras::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Mask_Andras */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mask_Andras"),
		CMask_Andras::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon_Andras */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Andras"),
		CWeapon_Andras::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon_Andras2 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Andras2"),
		CWeapon_Andras2::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon_Andras3 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Andras3"),
		CWeapon_Andras3::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon_Andras4 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Andras4"),
		CWeapon_Andras4::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon_Andras5 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Andras5"),
		CWeapon_Andras5::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RushSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_RushSword"),
		CRushSword::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_KickSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_KickSword"),
		CKickSword::Create(pDevice, pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_AndrasHead */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AndrasHead"),
		CAndrasHead::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_Juggulus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Juggulus"),
		CBoss_Juggulus::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Juggulus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Juggulus"),
		CBody_Juggulus::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_Hammer */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_Hammer"),
		CJuggulus_Hammer::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandOne */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandOne"),
		CJuggulus_HandOne::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandTwo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandTwo"),
		CJuggulus_HandTwo::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandThree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandThree"),
		CJuggulus_HandThree::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Aspiration */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aspiration"),
		CAspiration::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sphere"),
		CSphere::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CircleSphere */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CircleSphere"),
		CCircleSphere::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lightning */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning"),
		CLightning::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Electronic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Electronic"),
		CElectronic::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Malkhel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Malkhel"),
		CMalkhel::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Malkhel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Malkhel"),
		CBody_Malkhel::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Malkhel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Malkhel"),
		CWeapon_Malkhel::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Mantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mantari"),
		CMantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Mantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Mantari"),
		CBody_Mantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Mantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Mantari"),
		CWeapon_Mantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yantari"),
		CYantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Yantari"),
		CBody_Yantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Mask_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mask_Yantari"),
		CMask_Yantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Yantari"),
		CWeapon_Yantari::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Legionnaire */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Legionnaire"),
		CLegionnaire::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Legionnaire */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Legionnaire"),
		CBody_Legionnaire::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Legionnaire */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Legionnaire"),
		CWeapon_Legionnaire::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Legionnaire_Gun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Legionnaire_Gun"),
		CLegionnaire_Gun::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_LGGun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_LGGun"),
		CBody_LGGun::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Gun_LGGun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Gun_LGGun"),
		CWeapon_Gun_LGGun::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Sword_LGGun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sword_LGGun"),
		CWeapon_Sword_LGGun::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Arrow_LGGun */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Arrow_LGGun"),
		CWeapon_Arrow_LGGun::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ghost */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ghost"),
		CGhost::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Ghost */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Ghost"),
		CBody_Ghost::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Ghost */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Ghost"),
		CWeapon_Ghost::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Homonculus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Homonculus"),
		CHomonculus::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Homonculus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Homonculus"),
		CBody_Homonculus::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Homonculus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Homonculus"),
		CWeapon_Homonculus::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TargetLock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TargetLock"),
		CTargetLock::Create(pDevice, pContext))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	/* For.Prototype_GameObject_Item*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(pDevice, pContext))))
		return E_FAIL;
#pragma endregion ITEM

#pragma region DECAL
	/* For.Prototype_GameObject_Decal*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(pDevice, pContext))))
		return E_FAIL;
#pragma endregion DECAL

#pragma region Npc
	/* For.Prototype_GameObject_Npc_Rlya*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Npc_Rlya"),
		CNPC_Rlya::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Rlya*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Rlya"),
		CBody_Rlya::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Npc_Valnir*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Npc_Valnir"),
		CNpc_Valnir::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Valnir*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Valnir"),
		CBody_Valnir::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Npc_Choron*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Npc_Choron"),
		CNPC_Choron::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Choron*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Choron"),
		CBody_Choron::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Npc_Yaak*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Npc_Yaak"),
		CNPC_Yaak::Create(pDevice, pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Yaak*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Yaak"),
		CBody_Yaak::Create(pDevice, pContext))))
		return E_FAIL;


#pragma endregion Npc

	/* For.Prototype_GameObject_HoverBoard */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HoverBoard"),
		CHoverboard::Create(pDevice, pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Trap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trap"),
		CTrap::Create(pDevice, pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_TreasureChest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureChest"),
		CTreasureChest::Create(pDevice, pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_TestPhysxCollider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestPhysxCollider"),
		CTestPhysXCollider::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BlastWall"),
		CBlastWall::Create(pDevice, pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SavePoint"),
		CSavePoint::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}



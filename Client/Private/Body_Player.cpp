#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"
#include "EffectManager.h"


CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	BODY_DESC* pDesc = (BODY_DESC*)pArg;
	m_pIsCloaking = pDesc->pIsCloaking;
	m_pOnWater = pDesc->pOnWater;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 5.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 5.f;
		if (m_fDisolveValue < 0.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 0.f;
		}
		break;
	default:
		break;
	}
}

void CBody_Player::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CPlayer::STATE_RUN || *m_pState == CPlayer::STATE_WALK)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.3f, 0.f, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	*m_pCanCombo = false;
	if (*m_pState == CPlayer::STATE_KNOCKBACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 31;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("PlayerHit.ogg"), SOUND_PLAYER);
		}
	}
	else if (*m_pState == CPlayer::STATE_SLOWHIT)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_float4 ParticlePos = { m_WorldMatrix._41,m_WorldMatrix._42 + 1.f,m_WorldMatrix._43,1.f };

			EFFECTMGR->Generate_Particle(11, ParticlePos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 13;
		fAnimSpeed = 0.1f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
	}
	else if (*m_pState == CPlayer::STATE_FIGHTIDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 14;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
	}
	else if (*m_pState == CPlayer::STATE_WALK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 38; // 44
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
		if (*m_pOnWater && (m_pModelCom->Check_CurDuration(0.3f) || m_pModelCom->Check_CurDuration(0.8f)) && m_pParentMatrix->_42 < 300.5f)
		{
			_float4 fPos = _float4(m_pParentMatrix->_41, m_pParentMatrix->_42 + 0.1f, m_pParentMatrix->_43, 1.f);
			EFFECTMGR->Generate_Particle(100, fPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		}
	}
	else if (*m_pState == CPlayer::STATE_LOCKON_STRAIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 45;
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_LOCKON_BACKWARD)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 45;
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_LOCKON_LEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 46;
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_LOCKON_RIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 47;
		fAnimSpeed = 1.2f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_USEITEM)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 199;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_BUFF)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 189;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_RUN)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 42;
		fAnimSpeed = 1.5f;
		m_pModelCom->Set_LerpTime(1.2);
		if (*m_pOnWater && (m_pModelCom->Check_CurDuration(0.3f) || m_pModelCom->Check_CurDuration(0.8f)) && m_pParentMatrix->_42 < 300.5f)
		{
			_float4 fPos = _float4(m_pParentMatrix->_41, m_pParentMatrix->_42 + 0.1f, m_pParentMatrix->_43, 1.f);
			EFFECTMGR->Generate_Particle(100, fPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		}
	}
	else if (*m_pState == CPlayer::STATE_SLIDE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 41;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_HIT)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_float4 ParticlePos = { m_WorldMatrix._41,m_WorldMatrix._42 + 1.f,m_WorldMatrix._43,1.f };

			EFFECTMGR->Generate_Particle(11, ParticlePos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 13;
		fAnimSpeed = 0.8f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_JUMPSTART)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 18;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DOUBLEJUMPSTART)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_float4 vPos = { m_WorldMatrix._41,m_WorldMatrix._42 ,m_WorldMatrix._43 ,1.f };
			EFFECTMGR->Generate_Particle(12, vPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		}

		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_JUMP)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_LAND)
	{


		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 17;
		fAnimSpeed = 0.6f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_PARRY)
	{
		if (m_iPastAnimIndex < 26 || m_iPastAnimIndex > 28)
		{
			m_iPastAnimIndex = 26;
		}
		if(m_iPastAnimIndex == 28)
			fAnimSpeed = 2.f;
		else
			fAnimSpeed = 3.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;

		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_JUMPATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 210;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Get_Ratio_Betwin(0.8f, 1.f))
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_JUMPATTACK_LAND)
	{
		if (m_pModelCom->Check_CurDuration(0.01))
		{
			_float4 PartPos = XM3TO4(m_pWeapon[*m_pCurWeapon]->Get_Collider_Center());
			PartPos.y = m_WorldMatrix._42;
			EFFECTMGR->Generate_Particle(2, PartPos);
			EFFECTMGR->Generate_Particle(42, PartPos);
			EFFECTMGR->Generate_Particle(43, PartPos);
			EFFECTMGR->Generate_Particle(16, PartPos, nullptr, XMVectorZero(), 0.f, XMVectorSet(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33, 0.f));
			PartPos.y += 0.2f;
			EFFECTMGR->Generate_Particle(21, PartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 209;
		fAnimSpeed = 0.8f;
		m_pModelCom->Set_LerpTime(0.5);
		m_pWeapon[*m_pCurWeapon]->Set_Active();
	}
	else if (*m_pState == CPlayer::STATE_ROLLATTACK)
	{
		if (m_iPastAnimIndex < 33 || m_iPastAnimIndex > 37)
		{
			m_iPastAnimIndex = 33;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if(m_iPastAnimIndex == 34 || m_iPastAnimIndex == 35)
			fAnimSpeed = 1.5f;
		else
			fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 33) // 34
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_BACKATTACK) // X
	{
		if (m_iPastAnimIndex < 70 || m_iPastAnimIndex > 74)
		{
			m_iPastAnimIndex = 70;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 70 || m_iPastAnimIndex == 71 && m_pModelCom->Get_Ratio_Betwin(0.01f, 0.5f))
		{
			fAnimSpeed = 1.f;
		}
		else
		{
			fAnimSpeed = 3.f;
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex == 71)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else if (m_iPastAnimIndex == 74 && m_pModelCom->Get_Ratio_Betwin(0.f, 0.2f))
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 143;
		if (m_fDamageTiming < 1.f)
		{
			fAnimSpeed = 0.1f;
		}
		else
		{
			fAnimSpeed = 2.f;
		}
		m_pModelCom->Set_LerpTime(1.2);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 1.f && m_fDamageTiming < 1.15f)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else if (m_fDamageTiming > 2.05f && m_fDamageTiming < 2.20f)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK2)
	{
		AnimDesc.isLoop = false;

		_matrix World = XMLoadFloat4x4(&m_WorldMatrix);
		_float4 ParticlePos;
		XMStoreFloat4(&ParticlePos, World.r[3]);
		
		if (m_iPastAnimIndex == 143)
		{
			ParticlePos.y += 1.f;
			if (m_pModelCom->Get_Ratio_Betwin(0.25f, 0.85f))
			{
				EFFECTMGR->Generate_Particle(31, ParticlePos);
				m_pWeapon[*m_pCurWeapon]->Set_Active();
			}
				
		}
		else if (m_iPastAnimIndex == 86)
		{
			ParticlePos.y += 1.f;
			if (m_pModelCom->Get_Ratio_Betwin(0.25f, 0.85f))
			{
				EFFECTMGR->Generate_Particle(31, ParticlePos);
				m_pWeapon[*m_pCurWeapon]->Set_Active();
			}
				
		}
		else if (m_iPastAnimIndex == 65)
		{
			ParticlePos.y += 1.f;
			if (m_pModelCom->Get_Ratio_Betwin(0.25f, 0.85f))
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active();
				EFFECTMGR->Generate_Particle(31, ParticlePos);
			}
		}
		else if (m_iPastAnimIndex == 58)
		{
			if (m_pModelCom->Check_CurDuration(0.4f))
			{
				EFFECTMGR->Generate_Particle(34, ParticlePos);
				EFFECTMGR->Generate_Particle(33, ParticlePos);
				EFFECTMGR->Generate_Particle(33, ParticlePos, nullptr, XMVectorSet(0.f,1.f,0.f,0.f), 90.f);
				ParticlePos.y += 1.f;
				EFFECTMGR->Generate_Particle(32, ParticlePos);
			}
			if (m_pModelCom->Get_Ratio_Betwin(0.3f, 0.5f))
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active(true);
			}
		}

		if (m_iPastAnimIndex != 143 && m_iPastAnimIndex != 86 && m_iPastAnimIndex != 58 && m_iPastAnimIndex != 65)
		{
			m_iPastAnimIndex = 143;
		}


		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(1.2f);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming < 1.f)
		{
			fAnimSpeed = 0.1f;
		}
		else
		{
			fAnimSpeed = 2.5f;
			if (m_iPastAnimIndex == 58 && m_pModelCom->Get_Current_Ratio() > 0.1f && m_pModelCom->Get_Current_Ratio() < 0.2f)
			{
				fAnimSpeed = 0.1f;
			}
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK3)
	{
		if (m_iPastAnimIndex < 131 || m_iPastAnimIndex > 136)
		{
			m_iPastAnimIndex = 131;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 132 || m_iPastAnimIndex == 133)
		{
			fAnimSpeed = 0.5f; // 2.5 
		}
		else
		{
			fAnimSpeed = 1.5f; // 4
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 133)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		if (m_iPastAnimIndex != 58 && m_iPastAnimIndex != 143 && m_iPastAnimIndex != 358)
		{
			m_iPastAnimIndex = 58;
		}

		AnimDesc.isLoop = false;

		_matrix World = XMLoadFloat4x4(&m_WorldMatrix);
		_float4 ParticlePos;
		XMStoreFloat4(&ParticlePos, World.r[3]);
		_float4 AdjustPos = ParticlePos;
		AdjustPos.y += 1.f;
		if (m_iPastAnimIndex == 58)
		{
			if (m_pModelCom->Check_CurDuration(0.15))
			{
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Special4.ogg"), SOUND_PLAYER);
				EFFECTMGR->Generate_Particle(57, ParticlePos);
				EFFECTMGR->Generate_Particle(59, ParticlePos);
				CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
				pThirdPersonCamera->Zoom(90.f, 0.65f, 0.602f);
			}

			if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.4f))
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active(true);
				if (m_pModelCom->Check_CurDuration(0.21))
				{
					EFFECTMGR->Generate_Particle(58, AdjustPos, nullptr, XMVectorSet(1.f,0.f,0.f,0.f), 180.f);
				}
			}
			else
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active(false);
			}
			if (m_pModelCom->Get_Ratio_Betwin(0.1f, 0.6f))
			{
				m_fGoStraight = true;
			}
			else
			{
				m_fGoStraight = false;
			}
			fAnimSpeed = 1.f;
		}
		else if (m_iPastAnimIndex == 143)
		{
			if (m_pModelCom->Check_CurDuration(0.15))
			{
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Special4.ogg"), SOUND_PLAYER);
				EFFECTMGR->Generate_Particle(57, ParticlePos);
				EFFECTMGR->Generate_Particle(59, ParticlePos);
				CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
				pThirdPersonCamera->Zoom(90.f, 0.65f, 0.602f);
			}
			if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.4f))
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active();
				if (m_pModelCom->Check_CurDuration(0.21))
				{
					EFFECTMGR->Generate_Particle(58, AdjustPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 180.f);
				}
			}
			else
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active(false);
			}
			if (m_pModelCom->Get_Ratio_Betwin(0.1f, 1.f))
			{
				m_fGoStraight = true;
			}
			else
			{
				m_fGoStraight = false;
			}
			fAnimSpeed = 1.5f;
		}
		else if (m_iPastAnimIndex == 358)
		{
			if (m_pModelCom->Check_CurDuration(0.15))
			{
				m_pGameInstance->Disable_Echo();
				m_pGameInstance->Play_Effect_Sound(TEXT("Special4.ogg"), SOUND_PLAYER);
				EFFECTMGR->Generate_Particle(57, ParticlePos);
				EFFECTMGR->Generate_Particle(59, ParticlePos);
				CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
				pThirdPersonCamera->Zoom(90.f, 0.65f, 0.602f);
			}

			if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.4f))
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active();
				if (m_pModelCom->Check_CurDuration(0.21))
				{
					EFFECTMGR->Generate_Particle(58, AdjustPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 180.f);
				}
			}
			else
			{
				m_pWeapon[*m_pCurWeapon]->Set_Active(false);
			}
			if (m_pModelCom->Get_Ratio_Betwin(0.1f, 1.f))
			{
				m_fGoStraight = true;
			}
			else
			{
				m_fGoStraight = false;
			}
			fAnimSpeed = 1.5f;
		}

		if (m_iPastAnimIndex >= 300)
		{
			AnimDesc.iAnimIndex = m_iPastAnimIndex - 300;
		}
		else
		{
			AnimDesc.iAnimIndex = m_iPastAnimIndex;
		}
		m_pModelCom->Set_LerpTime(1.2f);
	}
	else if (*m_pState == CPlayer::STATE_LCHARGEATTACK)
	{
		if (m_iPastAnimIndex < 150 || m_iPastAnimIndex > 153)
		{
			m_iPastAnimIndex = 150;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 150)
		{
			fAnimSpeed = 2.f;
		}
		else if (m_iPastAnimIndex == 151 && m_pModelCom->Get_Ratio_Betwin(0.f, 0.5f))
		{
			fAnimSpeed = 1.f;
		}
		else
		{
			fAnimSpeed = 1.5f; // 1.5
		}

		m_pModelCom->Set_LerpTime(1.2);
		if ((m_iPastAnimIndex == 151 && m_pModelCom->Get_Ratio_Betwin(0.f, 0.5f)) || (m_iPastAnimIndex == 151 && m_pModelCom->Get_Ratio_Betwin(0.6f, 1.f)))
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_RCHARGEATTACK)
	{
		if (m_iPastAnimIndex < 131 || m_iPastAnimIndex > 136)
		{
			m_iPastAnimIndex = 131;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 134)
		{
			if (m_pModelCom->Get_Ratio_Betwin(0.3f, 0.5f))
			{
				fAnimSpeed = 1.f;
			}
			else
			{
				fAnimSpeed = 3.f; // 2.5 
			}
		}
		else if (m_iPastAnimIndex == 131)
		{
			fAnimSpeed = 4.f; // 4
		}
		else
		{
			fAnimSpeed = 1.5f;
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex == 133)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
		else if (m_iPastAnimIndex > 131) 
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_LATTACK1)
	{
		if (m_iPastAnimIndex < 144 || m_iPastAnimIndex > 149)
		{
			m_iPastAnimIndex = 144;
		}
		if (m_iPastAnimIndex == 149) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if(m_iPastAnimIndex == 144)
			fAnimSpeed = 3.3f;
		else if (m_iPastAnimIndex == 146 && (m_pModelCom->Get_Ratio_Betwin(0.f, 0.5f) || m_pModelCom->Get_Ratio_Betwin(0.8f, 1.f))) // 이렇게 하면 다음 공격에서 삑살나는
		{
			fAnimSpeed = 1.f; // 2.2
		}
		else
		{
			fAnimSpeed = 2.2f;
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 145 && m_iPastAnimIndex < 149)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_LATTACK2)
	{
		if (m_iPastAnimIndex < 138 || m_iPastAnimIndex > 142)
		{
			m_iPastAnimIndex = 138;
		}
		if (m_iPastAnimIndex == 142) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 139 && m_pModelCom->Get_Ratio_Betwin(0.f, 0.3f))
		{
			fAnimSpeed = 1.f;
		}
		else
		{
			fAnimSpeed = 1.8f;
		}
		
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 138 && m_iPastAnimIndex < 142)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_LATTACK3)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 143;
		fAnimSpeed = 1.8f; // 1
		m_pModelCom->Set_LerpTime(1.2);
		if (m_pModelCom->Get_Ratio_Betwin(0.15f, 0.4f))
		{
			fAnimSpeed = 1.f;
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			fAnimSpeed = 1.8f;
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_RATTACK1)
	{
		if (m_iPastAnimIndex < 121 || m_iPastAnimIndex > 125)
		{
			m_iPastAnimIndex = 121;
		}
		if (m_iPastAnimIndex == 125) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 123)
		{
			fAnimSpeed = 1.f;
		}
		else
		{
			fAnimSpeed = 2.5f; // 2
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 122)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_RATTACK2)
	{
		if (m_iPastAnimIndex < 75 || m_iPastAnimIndex > 79)
		{
			m_iPastAnimIndex = 75;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		if (m_iPastAnimIndex == 76)
		{
			fAnimSpeed = 2.2f;
		}
		else if (m_iPastAnimIndex == 78 && m_pModelCom->Get_Ratio_Betwin(0.f, 0.5f))
		{
			fAnimSpeed = 1.f;
		}
		else
		{
			fAnimSpeed = 2.5f;
		}
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 77)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_RUNLATTACK1)
	{
		if (m_iPastAnimIndex < 139 || m_iPastAnimIndex > 142)
		{
			m_iPastAnimIndex = 139; // 137
		}
		if (m_iPastAnimIndex == 142) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.7f; // 1.5

		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 138)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_RUNLATTACK2)
	{
		if (m_iPastAnimIndex == 142)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		if (m_iPastAnimIndex < 82 || m_iPastAnimIndex > 85)
		{
			m_iPastAnimIndex = 82;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.7f; // 1.5
		m_pModelCom->Set_LerpTime(1.2);
		
	}
	else if (*m_pState == CPlayer::STATE_RUNRATTACK) // X
	{
		if (m_iPastAnimIndex < 75 || m_iPastAnimIndex > 79)
		{
			m_iPastAnimIndex = 75;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 2.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex > 77)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_COUNTER)
	{
		if (m_iPastAnimIndex < 48 || m_iPastAnimIndex > 53)
		{
			m_iPastAnimIndex = 48;
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
		if (m_iPastAnimIndex == 49 || m_iPastAnimIndex == 52)
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active();
		}
		else
		{
			m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		}
	}
	else if (*m_pState == CPlayer::STATE_ROLL)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Look = XMVector4Normalize(ThisMat.r[2]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Look);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 32;
		fAnimSpeed = 2.5f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DASH)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Look = XMVector4Normalize(ThisMat.r[2]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Look);
			EFFECTMGR->Generate_Particle(20, vStartPos);
			
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 165;
		fAnimSpeed = 3.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DASH_FRONT)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Look = XMVector4Normalize(ThisMat.r[2]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Look);
			EFFECTMGR->Generate_Particle(20, vStartPos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		fAnimSpeed = 2.5f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DASH_BACK)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Look = XMVector4Normalize(ThisMat.r[2]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Look);
			EFFECTMGR->Generate_Particle(20, vStartPos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		fAnimSpeed = 2.5f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DASH_LEFT)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Right = XMVector4Normalize(ThisMat.r[0]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Right);
			EFFECTMGR->Generate_Particle(20, vStartPos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 8;
		fAnimSpeed = 2.5f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DASH_RIGHT)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_matrix ThisMat = XMLoadFloat4x4(&m_WorldMatrix);
			_vector Right = XMVector4Normalize(ThisMat.r[0]);
			_vector Up = XMVector4Normalize(ThisMat.r[1]);
			_vector vPos = ThisMat.r[3];
			_float4 vStartPos;
			XMStoreFloat4(&vStartPos, vPos);
			vStartPos.y += 1.f;
			EFFECTMGR->Generate_Particle(12, vStartPos, nullptr, XMVectorZero(), 0.f, Right);
			EFFECTMGR->Generate_Particle(20, vStartPos);
		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 166;
		fAnimSpeed = 2.5f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CPlayer::STATE_DEAD)
	{
		if (m_pModelCom->Check_CurDuration(0.01f))
		{
			_float4 ParticlePos = { m_WorldMatrix._41,m_WorldMatrix._42 + 1.f,m_WorldMatrix._43,1.f };

			EFFECTMGR->Generate_Particle(11, ParticlePos);

		}
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}	  
	else if (*m_pState == CPlayer::STATE_REVIVE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 40;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	
	if (*m_pState == CPlayer::STATE_JUMPATTACK || *m_pState == CPlayer::STATE_JUMPATTACK_LAND ||
		*m_pState == CPlayer::STATE_RCHARGEATTACK || *m_pState == CPlayer::STATE_RUNLATTACK1 ||
		*m_pState == CPlayer::STATE_RUNLATTACK2 || *m_pState == CPlayer::STATE_ROLL || *m_pState == CPlayer::STATE_DASH ||
		*m_pState == CPlayer::STATE_DASH_FRONT || *m_pState == CPlayer::STATE_DASH_BACK || *m_pState == CPlayer::STATE_DASH_LEFT ||
		*m_pState == CPlayer::STATE_DASH_RIGHT)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}

	if (*m_pState == CPlayer::STATE_SPECIALATTACK || *m_pState == CPlayer::STATE_SPECIALATTACK2 ||
		*m_pState == CPlayer::STATE_SPECIALATTACK3 || *m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		m_pGameInstance->Set_MotionBlur(true);
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false;
	// 여러 애니메이션을 재생할 때 마지막 애니메이션은 보간 필요
	if (m_iPastAnimIndex == 37 || m_iPastAnimIndex == 28 || m_iPastAnimIndex == 53 || m_iPastAnimIndex == 149 || 
		m_iPastAnimIndex == 74 || m_iPastAnimIndex == 153 || m_iPastAnimIndex == 79 || m_iPastAnimIndex == 125 || 
		m_iPastAnimIndex == 136 || m_iPastAnimIndex == 142 || AnimDesc.iAnimIndex == 209 || m_iPastAnimIndex == 85 ||
		(m_iPastAnimIndex == 0 && *m_pState != CPlayer::STATE_LATTACK1 && *m_pState != CPlayer::STATE_LATTACK2
			&& *m_pState != CPlayer::STATE_LATTACK3 && *m_pState != CPlayer::STATE_RATTACK1))
	{
		isLerp = true;
	}
	
	if (*m_pState == CPlayer::STATE_RUNLATTACK1 || *m_pState == CPlayer::STATE_RUNLATTACK2 || *m_pState == CPlayer::STATE_ROLLATTACK ||
		*m_pState == CPlayer::STATE_LCHARGEATTACK || *m_pState == CPlayer::STATE_BACKATTACK || *m_pState == CPlayer::STATE_LATTACK1 ||
		*m_pState == CPlayer::STATE_LATTACK2 || *m_pState == CPlayer::STATE_LATTACK3 || *m_pState == CPlayer::STATE_RATTACK1 ||
		*m_pState == CPlayer::STATE_RATTACK2 || *m_pState == CPlayer::STATE_COUNTER)
	{
		switch (*m_pCurWeapon)
		{
		case CPlayer::WEAPON_DURGASWORD:
			fAnimSpeed *= 0.7f;
			break;
		case CPlayer::WEAPON_PRETORIANSWORD:
			fAnimSpeed *= 0.8f;
			break;
		case CPlayer::WEAPON_RADAMANTHESWORD:
			fAnimSpeed *= 1.f;
			break;
		case CPlayer::WEAPON_ELISH:
			fAnimSpeed *= 0.9f;
			break;
		}
	}

	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished && *m_pState != CPlayer::STATE_SPECIALATTACK2 && *m_pState != CPlayer::STATE_SPECIALATTACK4)
	{
		// rollattack
		if (AnimDesc.iAnimIndex >= 33 && AnimDesc.iAnimIndex < 37)
		{
			m_iPastAnimIndex++;
		}
		// 패링
		else if (AnimDesc.iAnimIndex >= 26 && AnimDesc.iAnimIndex < 28)
		{
			m_iPastAnimIndex++;
		}
		// 반격
		else if (AnimDesc.iAnimIndex >= 48 && AnimDesc.iAnimIndex < 53)
		{
			m_iPastAnimIndex++;
		}
		// 공격1
		else if (AnimDesc.iAnimIndex >= 144 && AnimDesc.iAnimIndex < 149)
		{
			m_iPastAnimIndex++;
		}
		// 공격2, 달리기공격2
		else if (AnimDesc.iAnimIndex >= 82 && AnimDesc.iAnimIndex < 85)
		{
			m_iPastAnimIndex++;
		}
		// 달리기공격1
		else if (AnimDesc.iAnimIndex >= 137 && AnimDesc.iAnimIndex < 142)
		{
			m_iPastAnimIndex++;
		}
		//// 스킬3
		//else if (*m_pState == CPlayer::STATE_SPECIALATTACK3 && AnimDesc.iAnimIndex == 124)
		//{
		//	m_iPastAnimIndex += 2;
		//}
		// 우공격1
		else if (AnimDesc.iAnimIndex >= 121 && AnimDesc.iAnimIndex < 125)
		{
			m_iPastAnimIndex++;
		}
		// 달리기 우공격, 우공격2
		else if (AnimDesc.iAnimIndex >= 75 && AnimDesc.iAnimIndex < 79)
		{
			m_iPastAnimIndex++;
		}
		// 뒤잡
		else if (AnimDesc.iAnimIndex >= 70 && AnimDesc.iAnimIndex < 74)
		{
			m_iPastAnimIndex++;
		}
		// 왼 차지공격
		else if (AnimDesc.iAnimIndex >= 150 && AnimDesc.iAnimIndex < 153)
		{
			m_iPastAnimIndex++;
		}
		// 우 차지공격
		else if (AnimDesc.iAnimIndex >= 131 && AnimDesc.iAnimIndex < 136)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 126 && AnimDesc.iAnimIndex < 130)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 0;
			m_bAnimFinished = true;
		}
	}

	if (*m_pState == CPlayer::STATE_SPECIALATTACK)
	{
		if (m_fDamageTiming > 2.1f)
		{
			m_bAnimFinished = true;
		}
		else
		{
			m_bAnimFinished = false;
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK2)
	{
		m_bAnimFinished = false;
		if (m_iPastAnimIndex == 143 && animFinished)
		{
			m_iPastAnimIndex = 86;
		}
		else if (m_iPastAnimIndex == 86 && animFinished)
		{
			m_iPastAnimIndex = 65;
		}
		else if (m_iPastAnimIndex == 65 && animFinished)
		{
			m_iPastAnimIndex = 58;
		}
		else if (m_iPastAnimIndex == 58 && animFinished)
		{
			m_iPastAnimIndex = 0;
			m_bAnimFinished = true;
		}
	}
	else if (*m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		m_bAnimFinished = false;
		if (m_iPastAnimIndex == 58 && animFinished)
		{
			m_iPastAnimIndex = 143;
		}
		else if (m_iPastAnimIndex == 143 && animFinished)
		{
			m_iPastAnimIndex = 358;
		}
		else if (m_iPastAnimIndex == 358 && animFinished)
		{
			m_iPastAnimIndex = 0;
			m_bAnimFinished = true;
		}
	}

	if (m_bAnimFinished)
	{
		m_pWeapon[*m_pCurWeapon]->Set_Active(false);
		m_fDamageTiming = 0.f;
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	if (*m_pIsCloaking)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
	}	
	else
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);
		if (m_pGameInstance->Get_MoveShadow())
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		}
	}
}

HRESULT CBody_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		else if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		if (!(*m_pIsCloaking))
		{
			if (i == 2)
			{
				if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
					return E_FAIL;
				m_pShaderCom->Begin(7);
			}
			else
			{
				m_pShaderCom->Begin(0);
			}
		}
		else
		{
			m_pShaderCom->Begin(12);
		}

		

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}


HRESULT CBody_Player::Render_Reflection()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_float4x4 ViewMatrix;
	const _float4x4* matCam = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	// 원래 뷰 행렬 로드
	XMMATRIX mOriginalView = XMLoadFloat4x4(matCam);

	// 카메라 위치 추출
	XMVECTOR vCamPos = XMVector3Transform(XMVectorZero(), mOriginalView);

	// 바닥 평면의 높이 (예: Y = 0)
	float floorHeight = 300.f;

	// 반사된 카메라 위치 계산 (Y 좌표만 반전)
	XMVECTOR vReflectedCamPos = vCamPos;
	vReflectedCamPos = XMVectorSetY(vReflectedCamPos, 2 * floorHeight - XMVectorGetY(vCamPos));

	// 카메라 방향 벡터 추출
	XMVECTOR vCamLook = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 0, 1, 0), mOriginalView) - vCamPos);
	XMVECTOR vCamUp = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 1, 0, 0), mOriginalView) - vCamPos);

	// 반사된 카메라 방향 벡터 계산
	XMVECTOR vReflectedCamLook = XMVectorSetY(vCamLook, -XMVectorGetY(vCamLook));
	XMVECTOR vReflectedCamUp = XMVectorSetY(vCamUp, -XMVectorGetY(vCamUp));

	// 반사된 뷰 행렬 생성
	XMMATRIX mReflectedView = XMMatrixLookToLH(vReflectedCamPos, vReflectedCamLook, vReflectedCamUp);

	// 변환된 행렬 저장
	XMStoreFloat4x4(&ViewMatrix, mReflectedView);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		else if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		if (i == 2)
		{
			if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
				return E_FAIL;
			m_pShaderCom->Begin(10);
		}
		else
		{
			m_pShaderCom->Begin(9);
		}


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4 fPos = m_pGameInstance->Get_PlayerPos();

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPos.x, fPos.y + 10.f, fPos.z - 10.f, 1.f), XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wander"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DisolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}


CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}
#include "stdafx.h"
#include "..\Public\Player_Naruto.h"
#include "Body_Player_Naruto.h"
#include "Weapon.h"
#include "Camera_Free.h"
#include "Rasengun.h"
#include "RasenShuriken.h"
#include "Rasengun_Super.h"
#include "Wood_Swap.h"
#include "Trail_Line.h"
#include "UI_Player_Status.h"
#include "UI_Player_Skills.h"
#include "Particle_Point.h"



CPlayer_Naruto::CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer_Naruto::CPlayer_Naruto(const CPlayer_Naruto& rhs)
	: CLandObject(rhs)
{
}

HRESULT CPlayer_Naruto::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Naruto::Initialize(void* pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	pGameObjectDesc->fSpeedPerSec = 7.f;
	pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Skills()))
		return E_FAIL;

	if (FAILED(Add_Trails()))
		return E_FAIL;

	if (FAILED(Add_UIs()))
		return E_FAIL;
	
	if (FAILED(Add_Particles()))
		return E_FAIL;
	
	_vector vStart_Pos = { -10.f, 0.f, -10.f, 1.f };
	m_pTransformCom->Set_Pos(vStart_Pos);
	m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

	return S_OK;
}

void CPlayer_Naruto::Priority_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < SKILL_END; i++)
	{
		m_fSkillCurrentCoolTime[i] -= fTimeDelta;

		if(m_fSkillCurrentCoolTime[i] <= 0)
			m_fSkillCurrentCoolTime[i] = 0.f;
	}

 	m_LockOnTargetLength = 99999.f;

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);

	m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider");
	m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderDetecting, L"Monster_Main_Collider");
	m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderAttack, L"Monster_Main_Collider", L"Player_Attack_Collider");
	
	Player_Dash(fTimeDelta);
	Key_Input(fTimeDelta);

	if (m_bSkillOn[SKILL_RASENGUN])
		m_PlayerSkills.find(L"Skill_Rasengun")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_PlayerSkills.find(L"Skill_RasenShuriken")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_PlayerSkills.find(L"Skill_Rasengun_Super")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Priority_Tick(fTimeDelta);

	Particles_Priority_Tick(fTimeDelta);

	if (m_bOnAir && (m_fGravity < -0.017f) && m_bCellisLand)
	{
		m_pTransformCom->Go_Custom_Direction(fTimeDelta, m_fJumpSpeed, m_vJumpDirection, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	}

	if (!Set_Gravity(m_pTransformCom, fTimeDelta) && m_bOnAir == true && m_bCellisLand)
	{
		m_bOnAir = false;
		m_iState = 0x0000000000000000;
		m_iState |= PLAYER_STATE_LAND;
	}

	TranslateRootAnimation();
	
}

void CPlayer_Naruto::Tick(_float fTimeDelta)
{
	m_fComboTime += fTimeDelta;

	Skill_Tick(fTimeDelta);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);


	if (m_bSkillOn[SKILL_RASENGUN])
		m_PlayerSkills.find(L"Skill_Rasengun")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_PlayerSkills.find(L"Skill_RasenShuriken")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_PlayerSkills.find(L"Skill_Rasengun_Super")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Tick(fTimeDelta);

	Particles_Tick(fTimeDelta);

	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderDetecting->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CPlayer_Naruto::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Late_Tick(fTimeDelta);

	if (m_bSkillOn[SKILL_RASENGUN])
		m_PlayerSkills.find(L"Skill_Rasengun")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_PlayerSkills.find(L"Skill_RasenShuriken")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_PlayerSkills.find(L"Skill_Rasengun_Super")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Late_Tick(fTimeDelta);

	Particles_Late_Tick(fTimeDelta);
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderMain);
	m_pGameInstance->Add_DebugComponent(m_pColliderDetecting);
	m_pGameInstance->Add_DebugComponent(m_pColliderAttack);
#endif

	for (auto& Pair : m_PlayerTrails)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, Pair.second);

	for (auto& Pair : m_PlayerUIs)
		Pair.second->Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Naruto::Render()
{
	if (m_bSkillOn[SKILL_RASENGUN])
		m_PlayerSkills.find(L"Skill_Rasengun")->second->Render();
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_PlayerSkills.find(L"Skill_RasenShuriken")->second->Render();
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_PlayerSkills.find(L"Skill_Rasengun_Super")->second->Render();
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Render();

	return S_OK;
}

void CPlayer_Naruto::Player_Dash(_float fTimeDelta)
{
	if (m_iState & PLAYER_STATE_DASH)
	{
		if (m_iState & PLAYER_STATE_DASH_FRONT)
			Dash_Move(0.f, 0.95f, fTimeDelta, DIR_FRONT);
		else if (m_iState & PLAYER_STATE_DASH_BACK)
			Dash_Move(0.f, 0.95f, fTimeDelta, DIR_BACK);
		else if (m_iState & PLAYER_STATE_DASH_LEFT)
			Dash_Move(0.f, 0.95f, fTimeDelta, DIR_LEFT);
		else if (m_iState & PLAYER_STATE_DASH_RIGHT)
			Dash_Move(0.f, 0.95f, fTimeDelta, DIR_RIGHT);
	}

	if (m_iState & PLAYER_STATE_STRUCK)
		Dash_Move(0.f, 0.75f, fTimeDelta, DIR_FRONT);

	if (m_iState & PLAYER_THROW)
		Dash_Move(0.f, 0.96f, fTimeDelta, DIR_FRONT);

	if (m_iState & PLAYER_BEATEN)
		Dash_Move(0.f, 0.96f, fTimeDelta, DIR_FRONT);

	if (m_iState & PLAYER_STATE_AERIAL_DASH_START)
		Dash_Move(0.f, 0.95f, fTimeDelta, DIR_FRONT);
}

void CPlayer_Naruto::Player_Moving(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
		Set_Direc_Lerf(DIR_FRONT_LEFT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
		Set_Direc_Lerf(DIR_FRONT_RIGHT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
		Set_Direc_Lerf(DIR_BACK_LEFT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
		Set_Direc_Lerf(DIR_BACK_RIGHT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
		Set_Direc_Lerf(DIR_FRONT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
		Set_Direc_Lerf(DIR_BACK, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
		Set_Direc_Lerf(DIR_LEFT, 0.1f);

	else if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
		Set_Direc_Lerf(DIR_RIGHT, 0.1f);
}

void CPlayer_Naruto::Key_Input(_float fTimeDelta)
{
	
	if (m_iState & PLAYER_HIT)
	{
		if(m_pGameInstance->Key_Down(DIK_Z))
		{		
			if (m_fSkillCurrentCoolTime[SKILL_WOOD_SWAP] <= 0.f)
			{
				m_fSkillCurrentCoolTime[SKILL_WOOD_SWAP] = m_fSkillCoolTime[SKILL_WOOD_SWAP];
				Use_Skill(L"Skill_Wood_Swap");
				return;
			}
		}
	}

	if (m_iState & PLAYER_STATE_COMBO_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_iState & PLAYER_STATE_COMBO_1)
		{
			if (m_ColliderDelay > 0.2f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_COMBO_2)
		{
			if (m_ColliderDelay > 0.2f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_COMBO_3)
		{
			if (m_ColliderDelay > 0.55f)
				Off_Attack_Collider();
			else if(m_ColliderDelay> 0.4f)
				On_Attack_Collider(0.7f);
			else if(m_ColliderDelay >0.25f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_COMBO_4)
		{
			if (m_ColliderDelay > 0.35f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f, HIT_THROW);
		}

		if (m_LockOnTargetLength < 99999.f)
		{
			_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector LockOnDir = TargetPos - MyPos;
			m_pTransformCom->Set_Look(LockOnDir);
		}
		Dash_Move(0.f, 0.85f, fTimeDelta, DIR_FRONT);
	}
	else if (m_iState & PLAYER_STATE_COMBO_STRONG_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;
		
		if (m_iState & PLAYER_STATE_COMBO_STRONG_1)
		{
			if (m_ColliderDelay > 0.25f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f, HIT_STRONG);
		}
		else if (m_iState & PLAYER_STATE_COMBO_STRONG_2)
		{
			if (m_ColliderDelay > 0.4f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f, HIT_STRONG);
		}
		else if (m_iState & PLAYER_STATE_COMBO_STRONG_3)
		{
			if (m_ColliderDelay > 0.4f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f, HIT_BEATEN);
		}

		if (m_LockOnTargetLength < 99999.f)
		{
			_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector LockOnDir = TargetPos - MyPos;
			m_pTransformCom->Set_Look(LockOnDir);
		}
		Dash_Move(0.f, 0.9f, fTimeDelta, DIR_FRONT);
	}
	else if (m_iState & PLAYER_STATE_AERIAL_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;
		
		if (m_iState & PLAYER_STATE_AERIAL_COMBO_1)
		{
			if (m_ColliderDelay > 0.25f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_AERIAL_COMBO_2)
		{
			if (m_ColliderDelay > 0.55f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.4f)
				On_Attack_Collider(0.7f);
			else if (m_ColliderDelay > 0.25f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.1f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_AERIAL_COMBO_3)
		{
			if (m_ColliderDelay > 0.4f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f);
		}

		if (m_LockOnTargetLength < 99999.f)
		{
			_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector LockOnDir = TargetPos - MyPos;
			m_pTransformCom->Set_Look(LockOnDir);
		}
		Dash_Move(0.f, 0.85f, fTimeDelta, DIR_FRONT);
	}
	else
	{
		Off_Attack_Collider();

		m_ColliderDelay = 0.f;
	}

	if ( !(m_iState & PLAYER_STATE_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop()) ) // 재생중에 멈추면 안되는 동작이 끝나지 않았음
		return;

	if (m_iState & PLAYER_BEATEN)
	{
		if (m_iState != PLAYER_BEATEN_END)
		{
			m_iState *= 2;
			return;
		}
		else
		{
			m_iState = PLAYER_GET_UP;
			return;
		}
	}

	if(Skill_State(fTimeDelta))
		return;


	if (m_pBodyModelCom->Get_Consecutive())
		return;

	m_iState = 0x00000000;


	if ((m_pGameInstance->Mouse_Down(DIM_LB)))
	{
		if (!m_bOnAir)
		{
			if (m_fComboTime > 1.0f)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 0.f;
				m_iCombo |= PLAYER_STATE_COMBO_1;
				m_iState |= PLAYER_STATE_COMBO_1;
				m_fDashSpeed = 15.f;
				return;
			}

			if (m_iCombo & PLAYER_STATE_COMBO_3)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 1.0f;
				m_iState |= PLAYER_STATE_COMBO_4;
				m_iCombo |= PLAYER_STATE_COMBO_4;
				m_fDashSpeed = 20.f;
				return;
			}
			else if (m_iCombo & PLAYER_STATE_COMBO_2)
			{
				m_fComboTime = 0.f;
				m_iState |= PLAYER_STATE_COMBO_3;
				m_iCombo |= PLAYER_STATE_COMBO_3;
				m_fDashSpeed = 20.f;
				return;

			}
			else if (m_iCombo == PLAYER_STATE_COMBO_1)
			{
				m_fComboTime = 0.f;
				m_iState |= PLAYER_STATE_COMBO_2;
				m_iCombo |= PLAYER_STATE_COMBO_2;
				m_fDashSpeed = 15.f;
				return;
			}
		}
		else
		{
			if (m_fComboTime > 1.0f)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 0.f;
				m_iCombo |= PLAYER_STATE_AERIAL_COMBO_1;
				m_iState |= PLAYER_STATE_AERIAL_COMBO_1;
				m_fDashSpeed = 20.f;
				return;
			}

			if (m_iCombo & PLAYER_STATE_AERIAL_COMBO_2)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 1.0f;
				m_iState |= PLAYER_STATE_AERIAL_COMBO_3;
				m_iCombo |= PLAYER_STATE_AERIAL_COMBO_3;
				m_fDashSpeed = 20.f;
				return;
			}
			else if (m_iCombo & PLAYER_STATE_AERIAL_COMBO_1)
			{
				m_fComboTime = 0.f;
				m_iState |= PLAYER_STATE_AERIAL_COMBO_2;
				m_iCombo |= PLAYER_STATE_AERIAL_COMBO_2;
				m_fDashSpeed = 20.f;
				return;
			}
		}
	}
	
	else if (m_pGameInstance->Mouse_Down(DIM_RB))
	{
		if (!m_bOnAir)
		{
			if (m_fComboTime > 1.0f)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 0.f;
				m_iCombo |= PLAYER_STATE_COMBO_STRONG_1;
				m_iState |= PLAYER_STATE_COMBO_STRONG_1;
				m_fDashSpeed = 15.f;
				return;
			}

			if (m_iCombo & PLAYER_STATE_COMBO_STRONG_2)
			{
				m_iCombo = 0x00000000;
				m_fComboTime = 1.0f;
				m_iState |= PLAYER_STATE_COMBO_STRONG_3;
				m_iCombo |= PLAYER_STATE_COMBO_STRONG_3;
				m_fDashSpeed = 20.f;
				return;

			}
			else if (m_iCombo == PLAYER_STATE_COMBO_STRONG_1)
			{
				m_fComboTime = 0.f;
				m_iState |= PLAYER_STATE_COMBO_STRONG_2;
				m_iCombo |= PLAYER_STATE_COMBO_STRONG_2;
				m_fDashSpeed = 20.f;
				return;
			}
		}
	}

	if (m_pGameInstance->Key_Up(DIK_LCONTROL))
	{
		if (!m_iJumpState)
		{
			m_iJumpState++;
			m_bOnAir = true;
			m_iState |= PLAYER_STATE_JUMP;
			if (m_fChargingtime >= 3.f)
				m_fChargingtime = 3.f;

			m_fGravity		= Lerp(-0.4f, -1.f, m_fChargingtime / 3.f);
			m_fJumpSpeed	= Lerp(7.f, 30.f, m_fChargingtime / 3.f);
			
			m_vJumpDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_fChargingtime = 0.f;
			return;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{ }
	
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta, &m_fChargingtime))
	{
		if (!m_iJumpState)
		{
			m_iState |= PLAYER_STATE_CHAKRAJUMP_LOOP;
		}
		return;
	}
	

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (!m_iJumpState)
		{
			m_iJumpState++;
			m_bOnAir = true;
			m_iState |= PLAYER_STATE_JUMP;
			m_fGravity = -0.35f;
			m_vJumpDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_fJumpSpeed = 7.f;
			return;
		}
		if (m_iJumpState == 1)
		{
			m_iJumpState++;
			m_bOnAir = true;
			m_iState |= PLAYER_STATE_JUMP_DOUBLE;
			m_vJumpDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_fGravity = -0.35f;
			m_fJumpSpeed = 7.f;
			return;
		}
	}

	else if (m_pGameInstance->Key_Down(DIK_1) )
	{
		if (m_fSkillCurrentCoolTime[SKILL_RASENGUN] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_RASENGUN] = m_fSkillCoolTime[SKILL_RASENGUN];
			Use_Skill(L"Skill_Rasengun");
			return;
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_2) )
	{
		if (m_fSkillCurrentCoolTime[SKILL_RASENSHURIKEN] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_RASENSHURIKEN] = m_fSkillCoolTime[SKILL_RASENSHURIKEN];
			Use_Skill(L"Skill_RasenShuriken");
			return;
		}
		
	}
	else if (m_pGameInstance->Key_Down(DIK_3))
	{
		if (m_fSkillCurrentCoolTime[SKILL_RASENGUN_SUPER] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_RASENGUN_SUPER] = m_fSkillCoolTime[SKILL_RASENGUN_SUPER];
			Use_Skill(L"Skill_Rasengun_Super");
			return;
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_iState |= PLAYER_STATE_DASH_LEFT;
		m_fDashSpeed = 20.f;
		return;
	}
	else if (m_pGameInstance->Key_Down(DIK_E))
	{
		m_iState |= PLAYER_STATE_DASH_RIGHT;
		m_fDashSpeed = 20.f;
		return;	
	}
	else if (m_pGameInstance->Key_Down(DIK_X))
	{
		m_iState |= PLAYER_STATE_DASH_BACK;
		m_fDashSpeed = 20.f;
		return;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) || m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta) || m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) || m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
	{
		if (!m_bOnAir)
		{
			if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
			{
				if(Front_Dash())
					return;
				Set_Direc_Lerf(DIR_FRONT_LEFT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_FRONT_RIGHT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_BACK_LEFT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_BACK_RIGHT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_FRONT, 0.1f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_BACK, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_LEFT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
			{
				if (Front_Dash())
					return;
				Set_Direc_Lerf(DIR_RIGHT, 0.1f);
			}

			m_iState |= PLAYER_STATE_RUN;
			m_pTransformCom->Go_Straight(fTimeDelta,m_pNavigationCom, m_bOnAir, &m_bCellisLand);
			return;
		}

		else
		{
			if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
				Set_Direc_Lerf(DIR_FRONT_LEFT, 0.1f);

			else if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
				Set_Direc_Lerf(DIR_FRONT_RIGHT, 0.1f);

			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
				Set_Direc_Lerf(DIR_BACK_LEFT, 0.1f);

			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) && m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
				Set_Direc_Lerf(DIR_BACK_RIGHT, 0.1f);

			 if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
			{
				if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				{
					m_iState |= PLAYER_STATE_AERIAL_DASH_START;
					_float4 vDirec = Cal_Direc_To_CameraBase(DIR_FRONT);
					m_pTransformCom->Set_Look(XMLoadFloat4(&vDirec));
					m_fDashSpeed = 18.f;
					return;
				}
				Set_Direc_Lerf(DIR_FRONT, 0.1f);
			}

			else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
			{
				if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				{
					m_iState |= PLAYER_STATE_AERIAL_DASH_START;
					_float4 vDirec = Cal_Direc_To_CameraBase(DIR_BACK);
					m_pTransformCom->Set_Look(XMLoadFloat4(&vDirec));
					m_fDashSpeed = 18.f;
					return;
				}
				Set_Direc_Lerf(DIR_BACK, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
			{
				if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				{
					m_iState |= PLAYER_STATE_AERIAL_DASH_START;
					_float4 vDirec = Cal_Direc_To_CameraBase(DIR_LEFT);
					m_pTransformCom->Set_Look(XMLoadFloat4(&vDirec));
					m_fDashSpeed = 18.f;
					return;
				}
				Set_Direc_Lerf(DIR_LEFT, 0.1f);
			}
			else if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
			{
				 if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				 {
					 m_iState |= PLAYER_STATE_AERIAL_DASH_START;
					 _float4 vDirec = Cal_Direc_To_CameraBase(DIR_RIGHT);
					 m_pTransformCom->Set_Look(XMLoadFloat4(&vDirec));
					 m_fDashSpeed = 18.f;
					 return;
				 }
				 Set_Direc_Lerf(DIR_RIGHT, 0.1f);
			}
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		}
	}



	if ((!m_iState && (m_pBodyModelCom->Get_Current_Animation_Index() == 135) || m_pBodyModelCom->Get_Current_Animation_Index() == 169 || m_pBodyModelCom->Get_Current_Animation_Index() == 118 ))
	{
		if(!m_bOnAir)
			m_iState |= PLAYER_STATE_IDLE;
		else
			m_iState |= PLAYER_STATE_FALL_LOOP;

	}

	if(!m_iState && m_pBodyModelCom->Get_Current_Animation()->Get_Finished())
	{
		if(!m_bOnAir)
			m_iState |= PLAYER_STATE_IDLE;
		else
			m_iState |= PLAYER_STATE_FALL_LOOP;

	}
	
}

_float4 CPlayer_Naruto::Cal_Direc_To_CameraBase(MOVEDIRECTION eMoveDirec)
{
	_vector	CameraPos = m_pGameInstance->Get_CameraPos();
	_vector PlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector Direction = PlayerPos - CameraPos;
	_float4 fDirection;
	XMStoreFloat4(&fDirection, Direction);
	fDirection.y = 0;
	fDirection.w = 0;

	if (eMoveDirec == DIR_FRONT)
	{
		XMStoreFloat4(&fDirection, XMVector3Normalize(XMLoadFloat4(&fDirection)));
		return fDirection;
	}

	else if (eMoveDirec == DIR_BACK)
	{
		fDirection.x = -fDirection.x;
		fDirection.z = -fDirection.z;
	}

	else if (eMoveDirec == DIR_LEFT)
	{
		_float fSave_x = fDirection.x;
		fDirection.x = -fDirection.z;
		fDirection.z = fSave_x;
	}

	else if (eMoveDirec == DIR_RIGHT)
	{
		_float fSave_x = fDirection.x;
		fDirection.x = fDirection.z;
		fDirection.z = -fSave_x;
	}
	else if (eMoveDirec == DIR_FRONT_LEFT)
	{
		_float4 front_dir = fDirection;
		_float fSave_x = fDirection.x;
		fDirection.x = -fDirection.z;
		fDirection.z = fSave_x;
		XMStoreFloat4(&fDirection,XMLoadFloat4(&front_dir) + XMLoadFloat4(&fDirection));
	}
	else if (eMoveDirec == DIR_FRONT_RIGHT)
	{
		_float4 front_dir = fDirection;
		_float fSave_x = fDirection.x;
		fDirection.x = fDirection.z;
		fDirection.z = -fSave_x;
		XMStoreFloat4(&fDirection, XMLoadFloat4(&front_dir) + XMLoadFloat4(&fDirection));
	}
	else if (eMoveDirec == DIR_BACK_LEFT)
	{
		fDirection.x = -fDirection.x;
		fDirection.z = -fDirection.z;
		_float4 back_dir = fDirection;
		_float fSave_x = fDirection.x;
		fDirection.x = fDirection.z;
		fDirection.z = -fSave_x;
		XMStoreFloat4(&fDirection, XMLoadFloat4(&back_dir) + XMLoadFloat4(&fDirection));
	}
	else if (eMoveDirec == DIR_BACK_RIGHT)
	{
		fDirection.x = -fDirection.x;
		fDirection.z = -fDirection.z;
		_float4 back_dir = fDirection;
		_float fSave_x = fDirection.x;
		fDirection.x = -fDirection.z;
		fDirection.z = fSave_x;
		XMStoreFloat4(&fDirection, XMLoadFloat4(&back_dir) + XMLoadFloat4(&fDirection));
	}

	XMStoreFloat4(&fDirection, XMVector3Normalize(XMLoadFloat4(&fDirection)) );
	return fDirection;
}

void CPlayer_Naruto::Set_Direc_Lerf(MOVEDIRECTION eMoveDirec, _float ratio)
{
	_float4 Target_Dir = Cal_Direc_To_CameraBase(eMoveDirec);
	_vector	vTarget_Dir =	XMLoadFloat4(&Target_Dir);
	_vector vPlayer_Dir =	m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector Lerf_Dir;

	if( 0 > XMVectorGetX(XMVector4Dot(vPlayer_Dir, vTarget_Dir)))
		Lerf_Dir = XMVectorLerp(vPlayer_Dir, vTarget_Dir, ratio * 3);

	else
		Lerf_Dir = XMVectorLerp(vPlayer_Dir, vTarget_Dir, ratio);

	m_pTransformCom->Set_Look(Lerf_Dir);
}

void CPlayer_Naruto::Dash_Move(_float End_Speed, _float ratio, _float fTimeDelta, MOVEDIRECTION DashDir)
{
	m_fDashSpeed = Lerp(End_Speed, m_fDashSpeed, ratio);
	if(DashDir == DIR_FRONT)
		m_pTransformCom->Go_Straight_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_BACK)
		m_pTransformCom->Go_Backward_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_LEFT)
		m_pTransformCom->Go_Left_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_RIGHT)
		m_pTransformCom->Go_Right_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
}

void CPlayer_Naruto::TranslateRootAnimation()
{
	//// 움직임을 World이동으로치환
	//_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	//_float3 fInversemovevalue;
	//fInversemovevalue.x = -fmovevalue.x;
	//fInversemovevalue.y = 0.f;
	//fInversemovevalue.z = -fmovevalue.y;
	//m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

_bool CPlayer_Naruto::Front_Dash()
{
	if (m_pGameInstance->Key_Down(DIK_LSHIFT))
	{
		m_iState |= PLAYER_STATE_DASH_FRONT;
		m_fDashSpeed = 20.f;
		return true;
	}
	return false;
}

void CPlayer_Naruto::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	if (strColliderLayerTag == L"Monster_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = pMyCollider->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = pMyCollider->Get_Radius() + pTargetCollider->Get_Radius();
			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));

			m_pTransformCom->Go_Custom_Direction(0.016f, 3, Dir, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		}
	}
	else if (strColliderLayerTag == L"Monster_Attack_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();

			_vector vParPos = m_MyPos;
			vParPos.m128_f32[1] += 0.7f;
			for (auto iter : m_BasicParticles)
			{
				if (iter->Trigger(vParPos))
					break;
			}

			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);
			m_fSkillDurTime = 0.f;

			if (pTargetCollider->Get_HitType() == HIT_THROW)
			{
				m_iState = PLAYER_THROW;
				m_fDashSpeed = -15.f;
				m_CurrentHp -= 20;
			}
			else if (pTargetCollider->Get_HitType() == HIT_NORMAL)
			{
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STATE_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_CurrentHp -= 10;
			}
			else if (pTargetCollider->Get_HitType() == HIT_BEATEN)
			{
				m_iState = (PLAYER_BEATEN_START);
				m_fDashSpeed = -15.f;
				m_CurrentHp -= 25;
			}
		}
	}
	else if (strColliderLayerTag == L"FlameBomb_Collider" || strColliderLayerTag == L"Rasengun_Collider")
	{	
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
	
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);
			m_fSkillDurTime = 0.f;
	
			m_iState = PLAYER_BEATEN_START;
			m_fDashSpeed = -15.f;
			m_CurrentHp -= 30;
		}
	}
	else if (strColliderLayerTag == L"Wood_Dragon_Collider" || strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			m_iGetAttackFrameCount = 0;

			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);
		}
	}
}

void CPlayer_Naruto::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{	
	if (strColliderLayerTag == L"Monster_Main_Collider")
	{
		if (pMyCollider == m_pColliderDetecting)
		{
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_float		Length = XMVectorGetX(XMVector3Length(TargetPos - MyPos));
			_vector		Dir = XMVector3Normalize((TargetPos - MyPos));

			if (m_LockOnTargetLength > Length)
			{
				m_LockOnTargetLength = Length;
				m_pLockOnTarget = pTargetCollider->Get_Collider_GameObject();
			}
		}

		if (m_bInvincible)
			return;

		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = pMyCollider->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = pMyCollider->Get_Radius() + pTargetCollider->Get_Radius();
	
			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));
	
			m_pTransformCom->Go_Custom_Direction(0.016f, 3, Dir, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		}
	
		
	
	}
	else if (strColliderLayerTag == L"Wood_Dragon_Collider" || strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (m_bInvincible)
			return;
	
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
	
			if (m_iGetAttackFrameCount == 0)
			{
				m_iGetAttackFrameCount = 10;
				m_fSkillDurTime = 0.f;

				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STATE_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -4.f;
				m_CurrentHp -= 10;
			}
			m_iGetAttackFrameCount--;
		}
	}
}

void CPlayer_Naruto::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	if (strColliderLayerTag == L"Monster_Main_Collider")
	{

	}
	else if (strColliderLayerTag == L"Monster_Attack_Collider")
	{

	}
	else if (strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (m_bSkillOn[SKILL_WOOD_SWAP])
			return;

		if (pMyCollider == m_pColliderMain)
		{
			m_iGetAttackFrameCount = 0;
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);
			
			m_iState = PLAYER_BEATEN_START;
			m_fDashSpeed = -15.f;
			m_CurrentHp -= 20;
		}
	}
}

void CPlayer_Naruto::On_Attack_Collider(_float radius, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, 0.7f, radius });
	m_pColliderAttack->Set_HitType(HitType);
}

void CPlayer_Naruto::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f});
}

void CPlayer_Naruto::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_Rasengun")
	{
		m_pCamera->Set_Camera_radius(2.f, 0.02f);

		m_Skill_Animation_State = SKILL_RASENGUN;

		dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second)->Set_State();

		if(!m_bOnAir)
			m_iState |= PLAYER_STATE_RASENGUN_CHARGE;
		else 
			m_iState |= PLAYER_STATE_AERIAL_RASENGUN_CHARGE;

		m_bSkillOn[SKILL_RASENGUN] = true;
	}
	else if (strSkillName == L"Skill_RasenShuriken")
	{
		m_pCamera->Set_Camera_radius(2.f);

		m_Skill_Animation_State = SKILL_RASENSHURIKEN;

		dynamic_cast<CRasenShuriken*>(m_PlayerSkills.find(L"Skill_RasenShuriken")->second)->Set_State();

		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_RASENSHURIKEN;
		else
			m_iState |= PLAYER_STATE_AERIAL_RASENSHURIKEN;

		m_bSkillOn[SKILL_RASENSHURIKEN] = true;

	}
	else if (strSkillName == L"Skill_Rasengun_Super")
	{
		m_pCamera->Set_Camera_radius_Immediate(2.f);
		m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_FREE);
		m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_FRONT);
		m_pCamera->Set_Camera_radius(4.f, 0.03f);

		m_Skill_Animation_State = SKILL_RASENGUN_SUPER;
	
		dynamic_cast<CRasengun_Super*>(m_PlayerSkills.find(L"Skill_Rasengun_Super")->second)->Set_State();
	
		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_RASENSGUN_SUPER;
		else
			m_iState |= PLAYER_STATE_AERIAL_RASENGUN_SUPER;
	
		m_bSkillOn[SKILL_RASENGUN_SUPER] = true;
		m_bInvincible = true;
	}

	else if (strSkillName == L"Skill_Wood_Swap")
	{
		m_Skill_Animation_State = SKILL_WOOD_SWAP;

		CWood_Swap* WoodSwap=  dynamic_cast<CWood_Swap*>(m_PlayerSkills.find(L"Skill_Wood_Swap")->second);

		WoodSwap->Set_State();

		m_iState = 0x0000000000000000;
		m_iState |= PLAYER_STATE_FALL_LOOP;
		m_bOnAir = true;
		m_iJumpState = 1;
		m_fGravity = -0.25;

		m_bSkillOn[SKILL_WOOD_SWAP] = true;
	}
}

_bool CPlayer_Naruto::Skill_State(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_RASENGUN])
	{
		if (m_iState & PLAYER_STATE_RASENGUN_CHARGE)
		{
			m_pCamera->Set_Camera_radius();
			m_iState = PLAYER_STATE_RASENGUN_RUN_LOOP;		
			dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second)->Set_Next_State();
			m_bInvincible = true;
			return true;
		}
		else if (m_iState & PLAYER_STATE_RASENGUN_RUN_LOOP)
		{
			m_pTransformCom->Go_Straight_Custom(fTimeDelta, 15.f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
			Player_Moving(fTimeDelta);

			if (dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second)->Get_IsHit())
			{
				m_iState = PLAYER_STATE_RASENGUN_ATTACK;
			}
			return true;
		}
		else if (m_iState & PLAYER_STATE_RASENGUN_ATTACK)
		{
			m_bInvincible = false;
			m_bSkillOn[SKILL_RASENGUN] = false;
			m_Skill_Animation_State = SKILL_END;
			return false;
		}

		else if (m_iState & PLAYER_STATE_AERIAL_RASENGUN_CHARGE)
		{
			m_pCamera->Set_Camera_radius();
			m_iState = PLAYER_STATE_AERIAL_RASENGUN_RUN_LOOP;
			dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second)->Set_Next_State();
			m_bInvincible = true;
			return true;
		}
		else if (m_iState & PLAYER_STATE_AERIAL_RASENGUN_RUN_LOOP)
		{
			if (m_LockOnTargetLength < 99999.f)
			{
				_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector LockOnDir = TargetPos - MyPos;
				m_pTransformCom->Set_Look(LockOnDir);
				m_pTransformCom->MoveTo(TargetPos, 20.f, fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

			}
			else
			{
				Player_Moving(fTimeDelta);
				m_pTransformCom->Go_Straight_Custom(fTimeDelta, 20.f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
			}

			if (dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second)->Get_IsHit())
			{
				m_iState = PLAYER_STATE_AERIAL_RASENGUN_ATTACK;
			}
			return true;
		}
		else if (m_iState & PLAYER_STATE_AERIAL_RASENGUN_ATTACK)
		{
			m_bInvincible = false;
			m_bSkillOn[SKILL_RASENGUN] = false;
			m_Skill_Animation_State = SKILL_END;
			return false;
		}
		
		
	}

	if (m_bSkillOn[SKILL_RASENSHURIKEN])
	{
		if (m_iState & PLAYER_STATE_RASENSHURIKEN)
		{
			m_pCamera->Set_Camera_radius();

			m_Skill_Animation_State = SKILL_END;	
			return false;
		}
		
		
		else if (m_iState & PLAYER_STATE_AERIAL_RASENSHURIKEN)
		{
			m_pCamera->Set_Camera_radius();

			m_Skill_Animation_State = SKILL_END;	
			return false;
		}

	}

	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
	{

		if (m_iState & PLAYER_STATE_RASENSGUN_SUPER)
		{
			m_Skill_Animation_State = SKILL_END;
			m_bInvincible = false;
			return false;
		}


		else if (m_iState & PLAYER_STATE_AERIAL_RASENGUN_SUPER)
		{
			m_Skill_Animation_State = SKILL_END;
			m_bInvincible = false;
			return false;
		}

	}
	return false;

}

void CPlayer_Naruto::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_RASENGUN])
	{
		//위치 잡아주기
	}

	if (m_bSkillOn[SKILL_RASENSHURIKEN])
	{
		CRasenShuriken* pRasenShuriken = dynamic_cast<CRasenShuriken*>(m_PlayerSkills.find(L"Skill_RasenShuriken")->second);

		if (pRasenShuriken->Get_State() == CRasenShuriken::STATE_MAKING)
		{	
			m_fSkillDurTime += fTimeDelta;
			
			if (m_fSkillDurTime > 1.5f)
			{
				pRasenShuriken->Set_Next_State();
				pRasenShuriken->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

				if (m_LockOnTargetLength < 99999.f)
				{
					_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					_vector SkillPos = pRasenShuriken->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					_vector LockOnDir = TargetPos - SkillPos;
					pRasenShuriken->Get_TranformCom()->Set_Look(LockOnDir);
					pRasenShuriken->Set_Targeting(TargetPos);
				}
			}
		}

		else if (pRasenShuriken->Get_State() == CRasenShuriken:: STATE_DETECTING)
		{			
		}

		else if (pRasenShuriken->Get_State() == CRasenShuriken::STATE_FINISH)
		{
			m_bSkillOn[SKILL_RASENSHURIKEN] = false;
			m_fSkillDurTime = 0.f;
		}

	}
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
	{
		CRasengun_Super* pRasengun_Super= dynamic_cast<CRasengun_Super*>(m_PlayerSkills.find(L"Skill_Rasengun_Super")->second);

		//위치 잡아주기

		if (pRasengun_Super->Get_State() == CRasengun_Super::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 2.f)
			{
				m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_BACK);
				m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
				m_pCamera->Set_Camera_radius();
				pRasengun_Super->Set_Next_State();
			}
		}

		else if (pRasengun_Super->Get_State() == CRasengun_Super::STATE_FINISH)
		{
			m_bSkillOn[SKILL_RASENGUN_SUPER] = false;
			m_bInvincible = false;
			m_fSkillDurTime = 0.f;
		}

	}

	if (m_bSkillOn[SKILL_WOOD_SWAP])
	{
		CWood_Swap* pWood_Swap= dynamic_cast<CWood_Swap*>(m_PlayerSkills.find(L"Skill_Wood_Swap")->second);

		if (pWood_Swap->Get_State() == CWood_Swap::STATE_FINISH)
		{
			m_bSkillOn[SKILL_WOOD_SWAP] = false;
		}
	}

}

void CPlayer_Naruto::Skill_Cancle()
{
	if (m_bSkillOn[SKILL_RASENGUN])
	{
		CRasengun* pRasenShuriken = dynamic_cast<CRasengun*>(m_PlayerSkills.find(L"Skill_Rasengun")->second);

		if(pRasenShuriken->Get_State() == CRasengun::STATE_MAKING)
			m_bSkillOn[SKILL_RASENGUN] = false;

	}
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
	{
		CRasenShuriken* pRasenShuriken = dynamic_cast<CRasenShuriken*>(m_PlayerSkills.find(L"Skill_RasenShuriken")->second);

		if (pRasenShuriken->Get_State() == CRasengun::STATE_MAKING)
		{
			m_bSkillOn[SKILL_RASENSHURIKEN] = false;
			m_fSkillDurTime = 0.f;
		}
	}
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
	{
		CRasengun_Super* pRasenShuriken = dynamic_cast<CRasengun_Super*>(m_PlayerSkills.find(L"Skill_Rasengun_Super")->second);

	}

	m_pCamera->Set_Camera_radius();
	m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
}

void CPlayer_Naruto::Particles_Priority_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Priority_Tick(fTimeDelta);
}

void CPlayer_Naruto::Particles_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Tick(fTimeDelta);
}

void CPlayer_Naruto::Particles_Late_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Naruto::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};
	
	NaviDesc.iStartCellIndex = 30;
	
	if (m_Current_Level == LEVEL_GAMEPLAY)
	{
		if (FAILED(__super::Add_Component(m_Current_Level, TEXT("Prototype_Component_Navi_Map_Stadium"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
			return E_FAIL;
	}
	else if (m_Current_Level == LEVEL_BOSS)
	{
		if (FAILED(__super::Add_Component(m_Current_Level, TEXT("Prototype_Component_Navi_Map_Konoha_Village"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
			return E_FAIL;
	}

	/////////* Com_Collider *//////////////

	// 플레이어 본체 콜라이더 //
	CBounding_Sphere::SPHERE_DESC		MainBoundingDesc{};	
	MainBoundingDesc.fRadius = 0.7f;
	MainBoundingDesc.vCenter = _float3(0.f, MainBoundingDesc.fRadius, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &MainBoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Main_Collider", m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);
	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

	// 록온 탐색용 콜라이더 //
	CBounding_OBB::OBB_DESC		DetectingBoundingDesc{};
	DetectingBoundingDesc.vExtents = { 8.f , 10.f, 8.f };
	DetectingBoundingDesc.vRadians = { 0.f ,0.f, 0.f };
	DetectingBoundingDesc.vCenter = _float3(0.f, -2.f, 7.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Detecting"), reinterpret_cast<CComponent**>(&m_pColliderDetecting), &DetectingBoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Detecting_Collider", m_pColliderDetecting);
	m_pColliderDetecting->Set_Collider_GameObject(this);
	m_pColliderDetecting->Tick(m_pTransformCom->Get_WorldMatrix());

	// 콤보공격 콜라이더 //
	CBounding_Sphere::SPHERE_DESC		AttackBoundingDesc{};
	AttackBoundingDesc.fRadius = 0.f;
	AttackBoundingDesc.vCenter = _float3(0.f, AttackBoundingDesc.fRadius, AttackBoundingDesc.fRadius);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pColliderAttack), &AttackBoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Attack_Collider", m_pColliderAttack);
	m_pColliderAttack->Set_Collider_GameObject(this);
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());

	Off_Attack_Collider();

	///////////////////////////////////////////////////

	return S_OK;
}

HRESULT CPlayer_Naruto::Add_PartObjects()
{
	//Body_Player 추가
	CBody_Player_Naruto::BODY_PLAYER_DESC BodyPlayerDesc{};

	BodyPlayerDesc.pParentTransform		= m_pTransformCom;
	BodyPlayerDesc.pPlayerState			= &m_iState;
	BodyPlayerDesc.pPlayerGravity		= &m_fGravity;
	BodyPlayerDesc.pPlayerGForcePersec	= &m_fGForcePersec;

	CPartObject* pBody_Player = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player_Naruto"), &BodyPlayerDesc));
	if (nullptr == pBody_Player)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body"), pBody_Player);

	m_pBodyModelCom = dynamic_cast<CModel*>(pBody_Player->Get_Component(TEXT("Com_Model")));
	if (nullptr == m_pBodyModelCom)
		return E_FAIL;

	Safe_AddRef(m_pBodyModelCom);
	return S_OK;                 
}

HRESULT CPlayer_Naruto::Add_Skills()
{
	// 나선환
	CRasengun::SKILL_RASENGUN_DESC Rasengun_desc{};
	Rasengun_desc.pParentTransform = m_pTransformCom;
	Rasengun_desc.User_Type = CSkill::USER_PLAYER;
	Rasengun_desc.pCamera = m_pCamera;
	Rasengun_desc.Current_Level = m_Current_Level;
	Rasengun_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasengun = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengun"), &Rasengun_desc));
	if (nullptr == pRasengun)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Rasengun"), pRasengun);
	m_fSkillCoolTime[SKILL_RASENGUN] = 7.f;
	m_fSkillCurrentCoolTime[SKILL_RASENGUN] = 0.f;

	// 나선 수리검
	CRasenShuriken::SKILL_RASENSHURIKEN_DESC RasenShuriken_desc{};
	RasenShuriken_desc.pParentTransform = m_pTransformCom;
	RasenShuriken_desc.User_Type = CSkill::USER_PLAYER;
	RasenShuriken_desc.pCamera = m_pCamera;
	RasenShuriken_desc.Current_Level = m_Current_Level;
	RasenShuriken_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasenShuriken = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_RasenShuriken"), &RasenShuriken_desc));
	if (nullptr == pRasenShuriken)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_RasenShuriken"), pRasenShuriken);
	m_fSkillCoolTime[SKILL_RASENSHURIKEN] = 7.f;
	m_fSkillCurrentCoolTime[SKILL_RASENSHURIKEN] = 0.f;
	
	// 초대옥 나선환
	CRasengun_Super::SKILL_RASENGUN_SUPER_DESC Rasengun_Super_desc{};
	Rasengun_Super_desc.pParentTransform = m_pTransformCom;
	Rasengun_Super_desc.User_Type = CSkill::USER_PLAYER;
	Rasengun_Super_desc.pCamera = m_pCamera;
	Rasengun_Super_desc.Current_Level = m_Current_Level;
	Rasengun_Super_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasengun_Super = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengun_Super"), &Rasengun_Super_desc));
	if (nullptr == pRasengun_Super)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Rasengun_Super"), pRasengun_Super);
	m_fSkillCoolTime[SKILL_RASENGUN_SUPER] = 15.f;
	m_fSkillCurrentCoolTime[SKILL_RASENGUN_SUPER] = 0.f;

	// 통나무 바꿔치기 술
	CWood_Swap::SKILL_WOOD_SWAP_DESC Wood_Swap_desc{};
	Wood_Swap_desc.pParentTransform = m_pTransformCom;
	Wood_Swap_desc.User_Type = CSkill::USER_PLAYER;
	Wood_Swap_desc.pUser_Navigation = m_pNavigationCom;
	Wood_Swap_desc.pCamera = m_pCamera;
	Wood_Swap_desc.Current_Level = m_Current_Level;
	CSkill* pWood_Swap = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Wood_Swap"), &Wood_Swap_desc));
	if (nullptr == pWood_Swap)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Wood_Swap"), pWood_Swap);
	m_fSkillCoolTime[SKILL_WOOD_SWAP] = 3.f;
	m_fSkillCurrentCoolTime[SKILL_WOOD_SWAP] = 0.f;

	return S_OK;
}

HRESULT CPlayer_Naruto::Add_Trails()
{
	CTrail_Line::Trail_Line_DESC Trail_Line_L_Desc{};
	Trail_Line_L_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_L_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("LeftFoot");
	Trail_Line_L_Desc.eMyCharacter = CTrail_Line::PLAYER_NARUTO;
	CTrail_Line* pTrail_Foot_L = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_L_Desc));
	if (nullptr == pTrail_Foot_L)
		return E_FAIL;
	m_PlayerTrails.emplace(TEXT("Trail_Line_Foot_L"), pTrail_Foot_L);


	CTrail_Line::Trail_Line_DESC Trail_Line_R_Desc{};
	Trail_Line_R_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_R_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("RightFoot");
	Trail_Line_R_Desc.eMyCharacter = CTrail_Line::PLAYER_NARUTO;
	CTrail_Line* pTrail_Foot_R = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_R_Desc));
	if (nullptr == pTrail_Foot_R)
		return E_FAIL;
	m_PlayerTrails.emplace(TEXT("Trail_Line_Foot_R"), pTrail_Foot_R);
	
	return S_OK;
}

HRESULT CPlayer_Naruto::Add_UIs()
{
	CUI_Player_Status::UI_Player_Hp_DESC Hp_Desc{};
	Hp_Desc.pCurrentHp			= &m_CurrentHp;
	Hp_Desc.pMaxHp				= &m_MaxHp;
	Hp_Desc.pCoolTime			= &m_fSkillCoolTime[SKILL_RASENGUN_SUPER];
	Hp_Desc.pCurrentCoolTime	= &m_fSkillCurrentCoolTime[SKILL_RASENGUN_SUPER];
	Hp_Desc.eMyCharacter = CUI_Player_Status::PLAYER_NARUTO;
	CUI_Player_Status* pUIStatus = dynamic_cast<CUI_Player_Status*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Status"), &Hp_Desc));
	if (nullptr == pUIStatus)
		return E_FAIL;
	m_PlayerUIs.emplace(TEXT("UI_Player_Status"), pUIStatus);

	CUI_Player_Skills::UI_Player_CoolTime_DESC Skill_Desc{};
	Skill_Desc.pCoolTime[0] = &m_fSkillCoolTime[SKILL_RASENGUN];
	Skill_Desc.pCurrentCoolTime[0] = &m_fSkillCurrentCoolTime[SKILL_RASENGUN];
	Skill_Desc.pCoolTime[1] = &m_fSkillCoolTime[SKILL_RASENSHURIKEN];
	Skill_Desc.pCurrentCoolTime[1] = &m_fSkillCurrentCoolTime[SKILL_RASENSHURIKEN];
	Skill_Desc.pCoolTime[2] = &m_fSkillCoolTime[SKILL_WOOD_SWAP];
	Skill_Desc.pCurrentCoolTime[2] = &m_fSkillCurrentCoolTime[SKILL_WOOD_SWAP];
	Skill_Desc.eMyCharacter = CUI_Player_Skills::PLAYER_NARUTO;
	
	CUI_Player_Skills* pUISkills = dynamic_cast<CUI_Player_Skills*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Skills"), &Skill_Desc));
	if (nullptr == pUISkills)
		return E_FAIL;
	m_PlayerUIs.emplace(TEXT("UI_Player_Skills"), pUISkills);


	return S_OK;
}

HRESULT CPlayer_Naruto::Add_Particles()
{
	CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
	InstanceDesc.iNumInstance = 30;
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
	InstanceDesc.vSize = _float2(0.01f, 0.04f);
	InstanceDesc.vSpeed = _float2(2.5f, 3.5f);
	InstanceDesc.vLifeTime = _float2(0.7f, 1.0f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	InstanceDesc.fDuration = 1.3f;
	InstanceDesc.MyOption = CVIBuffer_Instancing::OPTION_SPREAD;
	InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";

	CParticle_Point* pParticle_Combo_Attack_1 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
	if (nullptr == pParticle_Combo_Attack_1)
		return E_FAIL;
	m_BasicParticles.push_back(pParticle_Combo_Attack_1);
    
	CParticle_Point* pParticle_Combo_Attack_2 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
	if (nullptr == pParticle_Combo_Attack_2)
		return E_FAIL;
	m_BasicParticles.push_back(pParticle_Combo_Attack_2);
    
	CParticle_Point* pParticle_Combo_Attack_3 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
	if (nullptr == pParticle_Combo_Attack_3)
		return E_FAIL;
	m_BasicParticles.push_back(pParticle_Combo_Attack_3);
    
	CParticle_Point* pParticle_Combo_Attack_4 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
	if (nullptr == pParticle_Combo_Attack_4)
		return E_FAIL;
	m_BasicParticles.push_back(pParticle_Combo_Attack_4);
    
	CParticle_Point* pParticle_Combo_Attack_5 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
	if (nullptr == pParticle_Combo_Attack_5)
		return E_FAIL;
	m_BasicParticles.push_back(pParticle_Combo_Attack_5);
   
	return S_OK;
}

CPlayer_Naruto* CPlayer_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Naruto");
		Safe_Release(pInstance);
	}


	return pInstance;
}


CGameObject* CPlayer_Naruto::Clone(void* pArg)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Naruto::Free()
{
	for (auto& Pair : m_PlayerParts)
		Safe_Release(Pair.second);
	m_PlayerParts.clear();

	for (auto& Pair : m_PlayerSkills)
		Safe_Release(Pair.second);
	m_PlayerSkills.clear();

	 for (auto& Pair : m_PlayerTrails)
		Safe_Release(Pair.second);
	m_PlayerTrails.clear();

	for (auto& Pair : m_PlayerUIs)
		Safe_Release(Pair.second);
	m_PlayerUIs.clear();

	for (auto& pParticle : m_BasicParticles)
		Safe_Release(pParticle);
	m_BasicParticles.clear();
	
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderDetecting);
	Safe_Release(m_pColliderAttack);

	if (m_pLockOnTarget != nullptr)
		Safe_Release(m_pLockOnTarget);

	__super::Free();
}
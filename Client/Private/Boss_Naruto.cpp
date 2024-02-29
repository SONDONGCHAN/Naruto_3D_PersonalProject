#include "Boss_Naruto.h"
#include "Body_Boss_Naruto.h"
#include "Skill.h"
#include "Rasengun.h"
#include "RasenShuriken.h"
#include "Rasengun_Super.h"
#include "Wood_Swap.h"
#include "Trail_Line.h"
#include "Player.h"
#include "UI_Boss_Status.h"
#include "Particle_Point.h"

CBoss_Naruto::CBoss_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLandObject(pDevice, pContext)
{
}

CBoss_Naruto::CBoss_Naruto(const CBoss_Naruto& rhs)
    : CLandObject(rhs)
{
}

HRESULT CBoss_Naruto::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Naruto::Initialize(void* pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;
	
	pGameObjectDesc->fSpeedPerSec = 4.f;
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

	if (FAILED(Add_Particles()))
		return E_FAIL;

	m_MaxHp		= 500.f;
	m_CurrentHp = 500.f;

	if (FAILED(Add_UIs()))
		return E_FAIL;

	m_CurrentState = MONSTER_STATE_IDLE;
	_vector vStart_Pos = { 0.f, 0.f, 13.f, 1.f };
	m_pTransformCom->Set_Pos(vStart_Pos);
	m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	
	return S_OK;
}

void CBoss_Naruto::Priority_Tick(_float fTimeDelta)
{
	Set_Direction();
	RootAnimation();
	Set_Gravity(m_pTransformCom, fTimeDelta);
	State_Control(fTimeDelta);
	
	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
	
	if (m_bSkillOn[SKILL_RASENGUN])
		m_MonsterSkills.find(L"Skill_Rasengun")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_MonsterSkills.find(L"Skill_RasenShuriken")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_MonsterSkills.find(L"Skill_Rasengun_Super")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_MonsterSkills.find(L"Skill_Wood_Swap")->second->Priority_Tick(fTimeDelta);

	Particles_Priority_Tick(fTimeDelta);

}

void CBoss_Naruto::Tick(_float fTimeDelta)
{
	Skill_Tick(fTimeDelta);
	
	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());
	
	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Tick(fTimeDelta);

	if (m_bSkillOn[SKILL_RASENGUN])
		m_MonsterSkills.find(L"Skill_Rasengun")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_MonsterSkills.find(L"Skill_RasenShuriken")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_MonsterSkills.find(L"Skill_Rasengun_Super")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_MonsterSkills.find(L"Skill_Wood_Swap")->second->Tick(fTimeDelta);

	Particles_Tick(fTimeDelta);

}

void CBoss_Naruto::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level,m_pColliderMain, L"Player_Main_Collider");
	m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderAttack, L"Player_Main_Collider", L"Monster_Attack_Collider");

	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	if (m_bSkillOn[SKILL_RASENGUN])
		m_MonsterSkills.find(L"Skill_Rasengun")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_MonsterSkills.find(L"Skill_RasenShuriken")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_MonsterSkills.find(L"Skill_Rasengun_Super")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_MonsterSkills.find(L"Skill_Wood_Swap")->second->Late_Tick(fTimeDelta);

	for (auto& Pair : m_MonsterTrails)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, Pair.second);

	for (auto& Pair : m_MonsterUIs)
		Pair.second->Late_Tick(fTimeDelta);

	Particles_Late_Tick(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderMain);
	m_pGameInstance->Add_DebugComponent(m_pColliderAttack);
#endif
}

HRESULT CBoss_Naruto::Render()
{
	if (m_bSkillOn[SKILL_RASENGUN])
		m_MonsterSkills.find(L"Skill_Rasengun")->second->Render();
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
		m_MonsterSkills.find(L"Skill_RasenShuriken")->second->Render();
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
		m_MonsterSkills.find(L"Skill_Rasengun_Super")->second->Render();
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_MonsterSkills.find(L"Skill_Wood_Swap")->second->Render();

	return S_OK;
}

void CBoss_Naruto::RootAnimation()
{
	// 움직임을 World이동으로치환
	_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	_float3 fInversemovevalue;
	fInversemovevalue.x = -fmovevalue.x;
	fInversemovevalue.y = 0.f;
	fInversemovevalue.z = -fmovevalue.y;
	m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

void CBoss_Naruto::State_Control(_float fTimeDelta)
{
	m_fCoolRasengun			-= fTimeDelta;
	m_fCoolRasenshuriken	-= fTimeDelta;
	m_fCoolRasengun_Super	-= fTimeDelta;
	m_fWaitingTime += fTimeDelta;
	m_fCoolTime_Rush_Current -= fTimeDelta;
	m_fCoolTime_SideDash_Current -= fTimeDelta;
	if (m_CurrentHp <= 0)
	{
		if (!m_bDeadCheck)
		{
			m_bDeadCheck = true;
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level,m_pColliderMain);
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level, m_pColliderAttack);
			m_pColliderMain->Delete_All_IsCollider();
			m_pColliderAttack->Delete_All_IsCollider();
			m_iState = PLAYER_DEAD;
			return;
		}
	}

	if (m_iState & PLAYER_STATE_COMBO_STRONG_3)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.4f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.2f)
			On_Attack_Collider(0.7f, HIT_THROW);

		Dash_Move(DIR_FRONT, 0.95f, fTimeDelta);
	}
	else if (m_iState & PLAYER_STATE_COMBO_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_iState & PLAYER_STATE_COMBO_3)
		{
			if (m_ColliderDelay > 0.55f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.40f)
				On_Attack_Collider(0.7f);
			else if (m_ColliderDelay > 0.25f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.10f)
				On_Attack_Collider(0.7f);

			Dash_Move(DIR_FRONT, 0.9f, fTimeDelta);

		}
		else if (m_iState & PLAYER_STATE_COMBO_4)
		{
			if (m_ColliderDelay > 0.4f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f, HIT_THROW);

			Dash_Move(DIR_FRONT, 0.9f, fTimeDelta);
		}
		else
		{
			if (m_ColliderDelay > 0.3f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.15f)
				On_Attack_Collider(0.7f);

			Dash_Move(DIR_FRONT, 0.9f, fTimeDelta);
		}
	}
	else if (m_iState & PLAYER_STATE_DASH)
		Dash_Move(m_Dash_Dir, 0.96f, fTimeDelta);

	else
	{
		Off_Attack_Collider();
		m_ColliderDelay = 0.f;
	}

	if (m_iState & PLAYER_STATE_STRUCK)
		Dash_Move(DIR_FRONT,0.75f, fTimeDelta);

	if (m_iState & PLAYER_STRUCK_STRONG)
		Dash_Move(DIR_FRONT,0.85f, fTimeDelta);

	if (m_iState & PLAYER_THROW)
		Dash_Move(DIR_FRONT, 0.96f, fTimeDelta);

	if (m_iState & PLAYER_BEATEN)
		Dash_Move(DIR_FRONT, 0.96f, fTimeDelta);


	if (m_iState & PLAYER_STATE_RUN)
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

	if (!(m_iState & PLAYER_STATE_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop()))
		return;

	if (m_iState & PLAYER_DEAD)
	{
		m_bDead = true;
		return;
	}

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
	m_pTransformCom->Set_Look(m_vDirection);

	if (m_fWaitingTime < 1.5f)
	{
		m_iState = 0x00000000;
		m_CurrentState = MONSTER_STATE_IDLE;
		m_iState |= PLAYER_STATE_IDLE;
		return;
	}

	if (!Using_Skill())
	{
		if (m_fCoolRasenshuriken < 0.f)
		{
			if ((!InComboAttackRange()) && CheckPlayer()) {
				Use_Skill(L"Skill_RasenShuriken");
				m_CurrentState = MONSTER_STATE_NINJUTSU_2;
				m_fCoolRasenshuriken = 15.f;
				m_iCountRasenshuriken = 0;
				return;
			}
		}
		if (m_fCoolRasengun < 0.f)
		{
			if ((!InComboAttackRange()) && CheckPlayer()) {
				Use_Skill(L"Skill_Rasengun");
				m_CurrentState = MONSTER_STATE_NINJUTSU;
				m_fCoolRasengun = 20.f;
				m_iCountRasengun = 0;
				return;
			}
		}
		if (m_fCoolRasengun_Super < 0.f)
		{
			if ((InComboAttackRange()) && CheckPlayer()) {
				Use_Skill(L"Skill_Rasengun_Super");
				m_CurrentState = MONSTER_STATE_NINJUTSU_3;
				m_fCoolRasengun_Super = 30.f;
				m_iCountRasengun_Super = 0;
				return;
			}
		}
	}

	if (Skill_State(fTimeDelta))
		return;
	
	m_iState = 0x00000000;

	switch (m_CurrentState)
	{
	case MONSTER_STATE_IDLE:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_iComboCount = 0;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_STRONG_3;
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;

			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_MOVE:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_iComboCount = 0;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_RUSH_ATTACK:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_COMBO_ATTACK:

		if (InComboAttackRange() && m_iComboCount == 0) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_2;
			m_iComboCount++;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		if (InComboAttackRange() && m_iComboCount == 1) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_3;
			m_iComboCount++;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		if (InComboAttackRange() && m_iComboCount == 2) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_4;
			m_iComboCount++;
			m_fDashSpeed = 10.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			break;
		}
		if (InComboAttackRange() && m_iComboCount == 3) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_NINJUTSU:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;

	case MONSTER_STATE_NINJUTSU_2:

		m_fWaitingTime = 0.f;
		m_Skill_Animation_State = SKILL_END;

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_NINJUTSU_3:

		m_fWaitingTime = 0.f;
		m_Skill_Animation_State = SKILL_END;

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_STRUCK:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= PLAYER_STATE_COMBO_1;
			m_iComboCount = 0;
			m_fDashSpeed = 13.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			m_iState |= (PLAYER_STATE_COMBO_STRONG_3);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= PLAYER_STATE_RUN;
				else if (iRandom == 1) {
					m_iState |= PLAYER_STATE_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= PLAYER_STATE_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= PLAYER_STATE_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= PLAYER_STATE_IDLE;
			break;
		}
		break;
	}
}

void CBoss_Naruto::Set_Direction()
{
	_vector Pos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
	_vector Dir = Pos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Dir.m128_f32[3] = 0.f;

	m_vDirection = Dir;
}

_bool CBoss_Naruto::CheckPlayer()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 17.f)
		return true;

	return false;
}

_bool CBoss_Naruto::InRushAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 5.f)
		return true;

	return false;
}

_bool CBoss_Naruto::InComboAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 4.f)
		return true;

	return false;
}

void CBoss_Naruto::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = m_pColliderMain->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = m_pColliderMain->Get_Radius() + pTargetCollider->Get_Radius();

			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));

			m_pTransformCom->Go_Custom_Direction(0.016f, 4, Dir, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		}
	}
	else if (strColliderLayerTag == L"Player_Attack_Collider")
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
			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 2.f, 0.05f);

			if (pTargetCollider->Get_HitType() == HIT_THROW)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (PLAYER_THROW);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 20;
			}
			else if (pTargetCollider->Get_HitType() == HIT_STRONG)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STRUCK_STRONG_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 15;
			}
			else if (pTargetCollider->Get_HitType() == HIT_BEATEN)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (PLAYER_BEATEN_START);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 25;
			}
			else if (pTargetCollider->Get_HitType() == HIT_NORMAL)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STATE_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 10;
			}

		}
	}
	else if (strColliderLayerTag == L"Rasengun_Collider" || strColliderLayerTag == L"FlameBomb_Collider" || strColliderLayerTag == L"Wood_Hand_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);

			m_CurrentState = MONSTER_STATE_STRUCK;
			m_iState = (PLAYER_BEATEN_START);
			m_fDashSpeed = -15.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 30;
		}
	}
	else if (strColliderLayerTag == L"RasenShuriken_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
			m_fGetAttack_FrameCount = 0;
		}
	}
	else if (strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
			m_fGetAttack_FrameCount = 0;
		}
	}
	else if (strColliderLayerTag == L"Kamui_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
			m_fGetAttack_FrameCount = 0;
		}
	}
	else if (strColliderLayerTag == L"Chidori_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			Skill_Cancle();
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);

			m_CurrentState = MONSTER_STATE_STRUCK;
			m_iState = (PLAYER_ELECTRICSHOCK);
			m_fDashSpeed = 0.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 40;
		}
	}
}

void CBoss_Naruto::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = m_pColliderMain->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = m_pColliderMain->Get_Radius() + pTargetCollider->Get_Radius();

			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));

			m_pTransformCom->Go_Custom_Direction(0.016f, 4, Dir, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		}
	}
	else if (strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			if (m_fGetAttack_FrameCount == 0)
			{
				m_fGetAttack_FrameCount = 10;
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STATE_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -3.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 10;
			}
			m_fGetAttack_FrameCount--;
		}
	}

	else if (strColliderLayerTag == L"Kamui_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			if (m_fGetAttack_FrameCount == 0)
			{
				m_fGetAttack_FrameCount = 7;
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (PLAYER_STATE_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -3.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 10;
			}
			m_fGetAttack_FrameCount--;
		}
	}
}

void CBoss_Naruto::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
		}
	}
	else if (strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			m_fGetAttack_FrameCount = 0;
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);

			m_CurrentState = MONSTER_STATE_STRUCK;
			m_iState = PLAYER_BEATEN_START;
			m_fDashSpeed = -15.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 20;
		}
	}
	else if (strColliderLayerTag == L"Kamui_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			m_fGetAttack_FrameCount = 0;
			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);

			m_CurrentState = MONSTER_STATE_STRUCK;
			m_iState = PLAYER_BEATEN_START;
			m_fDashSpeed = -15.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 20;
		}
	}
}

void CBoss_Naruto::On_Attack_Collider(_float radius, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, 0.7f, radius });
	m_pColliderAttack->Set_HitType(HitType);
}	

void CBoss_Naruto::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f });
}

void CBoss_Naruto::Dash_Move(DASH_DIR dir, _float ratio, _float fTimeDelta)
{
	m_fDashSpeed = Lerp(0, m_fDashSpeed, ratio);

	if (dir == DIR_FRONT)
		m_pTransformCom->Go_Straight_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_BACK)
		m_pTransformCom->Go_Backward_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_LEFT)
		m_pTransformCom->Go_Left_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_RIGHT)
		m_pTransformCom->Go_Right_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
}

void CBoss_Naruto::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_Rasengun")
	{
		m_Skill_Animation_State = SKILL_RASENGUN;
		
		dynamic_cast<CRasengun*>(m_MonsterSkills.find(L"Skill_Rasengun")->second)->Set_State();
		
		m_iState = PLAYER_STATE_RASENGUN_CHARGE;
		
		m_bSkillOn[SKILL_RASENGUN] = true;
	}
	else if (strSkillName == L"Skill_RasenShuriken")
	{
		m_Skill_Animation_State = SKILL_RASENSHURIKEN;
		
		dynamic_cast<CRasenShuriken*>(m_MonsterSkills.find(L"Skill_RasenShuriken")->second)->Set_State();
		
		m_iState = PLAYER_STATE_RASENSHURIKEN;
		
		m_bSkillOn[SKILL_RASENSHURIKEN] = true;
	}
	else if (strSkillName == L"Skill_Rasengun_Super")
	{
		m_Skill_Animation_State = SKILL_RASENGUN_SUPER;
		
		dynamic_cast<CRasengun_Super*>(m_MonsterSkills.find(L"Skill_Rasengun_Super")->second)->Set_State();
		
		m_iState = PLAYER_STATE_RASENSGUN_SUPER;
		
		m_bSkillOn[SKILL_RASENGUN_SUPER] = true;

		m_bInvincible = true;
	}
	else if (strSkillName == L"Skill_Wood_Swap")
	{
		m_Skill_Animation_State = SKILL_WOOD_SWAP;
		
		dynamic_cast<CWood_Swap*>(m_MonsterSkills.find(L"Skill_Wood_Swap")->second)->Set_State();
		
		m_iState = 0x0000000000000000;
		m_iState = PLAYER_STATE_FALL_LOOP;
		m_bOnAir = true;
		m_iJumpState = 1;
		m_fGravity = -0.25;
		
		m_bSkillOn[SKILL_WOOD_SWAP] = true;
	}
}

_bool CBoss_Naruto::Skill_State(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_RASENGUN])
	{
		if (m_iState & PLAYER_STATE_RASENGUN_CHARGE)
		{
			m_iState = PLAYER_STATE_RASENGUN_RUN_LOOP;
			dynamic_cast<CRasengun*>(m_MonsterSkills.find(L"Skill_Rasengun")->second)->Set_Next_State();
			return true;
		}
		else if (m_iState & PLAYER_STATE_RASENGUN_RUN_LOOP)
		{
			m_pTransformCom->Go_Straight_Custom(fTimeDelta, 15.f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		
			if (dynamic_cast<CRasengun*>(m_MonsterSkills.find(L"Skill_Rasengun")->second)->Get_IsHit())
			{
				m_iState = PLAYER_STATE_RASENGUN_ATTACK;
			}
			return true;
		}
		else if (m_iState & PLAYER_STATE_RASENGUN_ATTACK)
		{
			m_bSkillOn[SKILL_RASENGUN] = false;
			m_Skill_Animation_State = SKILL_END;
			m_fWaitingTime = 0.f;
			return false;
		}
	}
	return false;
}

void CBoss_Naruto::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
	{
		CRasenShuriken* pRasenShuriken = dynamic_cast<CRasenShuriken*>(m_MonsterSkills.find(L"Skill_RasenShuriken")->second);

		if (pRasenShuriken->Get_State() == CRasenShuriken::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 1.5f)
			{
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 1.f;
				pRasenShuriken->Get_TranformCom()->Set_Pos(Pos);
				pRasenShuriken->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pRasenShuriken->Set_Targeting(TargetPos);
				pRasenShuriken->Set_Next_State();
			}
		}
		else if (pRasenShuriken->Get_State() == CRasenShuriken::STATE_FINISH)
		{
			m_bSkillOn[SKILL_RASENSHURIKEN] = false;
			m_fSkillDurTime = 0.f;
		}

	}

	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
	{
		CRasengun_Super* pRasengun_Super = dynamic_cast<CRasengun_Super*>(m_MonsterSkills.find(L"Skill_Rasengun_Super")->second);

		if (pRasengun_Super->Get_State() == CRasengun_Super::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 2.f)
			{
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
}

void CBoss_Naruto::Skill_Cancle()
{	
	if (m_bSkillOn[SKILL_RASENGUN])
	{
		CRasengun* pRasenShuriken = dynamic_cast<CRasengun*>(m_MonsterSkills.find(L"Skill_Rasengun")->second);
	
		if (pRasenShuriken->Get_State() == CRasengun::STATE_MAKING)
			m_bSkillOn[SKILL_RASENGUN] = false;
	
	
	}
	if (m_bSkillOn[SKILL_RASENSHURIKEN])
	{
		CRasenShuriken* pRasenShuriken = dynamic_cast<CRasenShuriken*>(m_MonsterSkills.find(L"Skill_RasenShuriken")->second);
	
		if (pRasenShuriken->Get_State() == CRasengun::STATE_MAKING)
		{
			m_bSkillOn[SKILL_RASENSHURIKEN] = false;
			m_fSkillDurTime = 0.f;
		}
	}
	if (m_bSkillOn[SKILL_RASENGUN_SUPER])
	{
	}

}

_bool CBoss_Naruto::Using_Skill()
{
	for (_uint i = 0 ; i < SKILL_END; i++ )
	{
		if(m_bSkillOn[i])
			return true;
	}
	return false;
}

void CBoss_Naruto::Particles_Priority_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Priority_Tick(fTimeDelta);
}

void CBoss_Naruto::Particles_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Tick(fTimeDelta);
}

void CBoss_Naruto::Particles_Late_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Late_Tick(fTimeDelta);
}

HRESULT CBoss_Naruto::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};
	NaviDesc.iStartCellIndex = 32;

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
	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
	BoundingDesc.fRadius = 0.7f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Monster_Main_Collider"), m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);
	
	
	CBounding_Sphere::SPHERE_DESC		AttackBoundingDesc{};
	AttackBoundingDesc.fRadius = 0.f;
	AttackBoundingDesc.vCenter = _float3(0.f, AttackBoundingDesc.fRadius, AttackBoundingDesc.fRadius);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pColliderAttack), &AttackBoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(m_Current_Level, L"Monster_Attack_Collider", m_pColliderAttack);
	m_pColliderAttack->Set_Collider_GameObject(this);

	return S_OK;
}

HRESULT CBoss_Naruto::Add_PartObjects()
{
	CBody_Boss_Naruto::BODY_MONSTER_DESC BodyBossDesc{};
	BodyBossDesc.pParentTransform = m_pTransformCom;
	BodyBossDesc.pState = &m_iState;

	CPartObject* pBody_Boss= dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Boss_Naruto"), &BodyBossDesc));
	if (nullptr == pBody_Boss)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Body"), pBody_Boss);

	m_pBodyModelCom = dynamic_cast<CModel*>(pBody_Boss->Get_Component(TEXT("Com_Model")));

	return S_OK;
}

HRESULT CBoss_Naruto::Add_Skills()
{
	// 나선환
	CRasengun::SKILL_RASENGUN_DESC Rasengun_desc{};
	Rasengun_desc.pParentTransform = m_pTransformCom;
	Rasengun_desc.User_Type = CSkill::USER_MONSTER;
	Rasengun_desc.pCamera = m_pCamera;
	Rasengun_desc.Current_Level = m_Current_Level;
	Rasengun_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasengun = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengun"), &Rasengun_desc));
	if (nullptr == pRasengun)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_Rasengun"), pRasengun);


	// 나선 수리검
	CRasenShuriken::SKILL_RASENSHURIKEN_DESC RasenShuriken_desc{};
	RasenShuriken_desc.pParentTransform = m_pTransformCom;
	RasenShuriken_desc.User_Type = CSkill::USER_MONSTER;
	RasenShuriken_desc.pCamera = m_pCamera;
	RasenShuriken_desc.Current_Level = m_Current_Level;
	RasenShuriken_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasenShuriken = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_RasenShuriken"), &RasenShuriken_desc));
	if (nullptr == pRasenShuriken)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_RasenShuriken"), pRasenShuriken);


	// 초대옥 나선환
	CRasengun_Super::SKILL_RASENGUN_SUPER_DESC Rasengun_Super_desc{};
	Rasengun_Super_desc.pParentTransform = m_pTransformCom;
	Rasengun_Super_desc.User_Type = CSkill::USER_MONSTER;
	Rasengun_Super_desc.pCamera = m_pCamera;
	Rasengun_Super_desc.Current_Level = m_Current_Level;
	Rasengun_Super_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	CSkill* pRasengun_Super = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Rasengun_Super"), &Rasengun_Super_desc));
	if (nullptr == pRasengun_Super)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_Rasengun_Super"), pRasengun_Super);


	// 통나무 바꿔치기 술
	CWood_Swap::SKILL_WOOD_SWAP_DESC Wood_Swap_desc{};
	Wood_Swap_desc.pParentTransform = m_pTransformCom;
	Wood_Swap_desc.User_Type = CSkill::USER_MONSTER;
	Wood_Swap_desc.pCamera = m_pCamera;
	Wood_Swap_desc.pUser_Navigation = m_pNavigationCom;
	Wood_Swap_desc.Current_Level = m_Current_Level;
	CSkill* pWood_Swap = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Wood_Swap"), &Wood_Swap_desc));
	if (nullptr == pWood_Swap)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_Wood_Swap"), pWood_Swap);

	return S_OK;
}

HRESULT CBoss_Naruto::Add_Trails()
{
	CTrail_Line::Trail_Line_DESC Trail_Line_L_Desc{};
	Trail_Line_L_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_L_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("LeftFoot");
	Trail_Line_L_Desc.eMyCharacter = CTrail_Line::MONSTER_NARUTO;
	CTrail_Line* pTrail_Foot_L = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_L_Desc));
	if (nullptr == pTrail_Foot_L)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Foot_L"), pTrail_Foot_L);


	CTrail_Line::Trail_Line_DESC Trail_Line_R_Desc{};
	Trail_Line_R_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_R_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("RightFoot");
	Trail_Line_R_Desc.eMyCharacter = CTrail_Line::MONSTER_NARUTO;
	CTrail_Line* pTrail_Foot_R = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_R_Desc));
	if (nullptr == pTrail_Foot_R)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Foot_R"), pTrail_Foot_R);

	return S_OK;
}

HRESULT CBoss_Naruto::Add_UIs()
{
	CUI_Boss_Status::UI_Boss_Hp_DESC Hp_Desc{};
	Hp_Desc.pCurrentHp = &m_CurrentHp;
	Hp_Desc.pMaxHp = &m_MaxHp;
	Hp_Desc.eMyCharacter = CUI_Boss_Status::BOSS_NARUTO;

	CUI_Boss_Status* pUIStatus = dynamic_cast<CUI_Boss_Status*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_CUI_Boss_Status"), &Hp_Desc));
	if (nullptr == pUIStatus)
		return E_FAIL;
	m_MonsterUIs.emplace(TEXT("UI_Boss_Status"), pUIStatus);

	return S_OK;
}

HRESULT CBoss_Naruto::Add_Particles()
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
	InstanceDesc.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
	InstanceDesc.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
	InstanceDesc.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
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

CBoss_Naruto* CBoss_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Naruto* pInstance = new CBoss_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Naruto::Clone(void* pArg)
{
	CBoss_Naruto* pInstance = new CBoss_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Naruto::Free()
{
	for (auto& Pair : m_MonsterParts)
		Safe_Release(Pair.second);
	m_MonsterParts.clear();
	
	for (auto& Pair : m_MonsterSkills)
		Safe_Release(Pair.second);
	m_MonsterSkills.clear();
	
	for (auto& Pair : m_MonsterTrails)
		Safe_Release(Pair.second);
	m_MonsterTrails.clear();

	for (auto& Pair : m_MonsterUIs)
		Safe_Release(Pair.second);
	m_MonsterUIs.clear();

	for (auto& pParticle : m_BasicParticles)
		Safe_Release(pParticle);
	m_BasicParticles.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderAttack);

	__super::Free();
}

#include "stdafx.h"
#include "Monster_Samurai.h"
#include "Body_Monster_Samurai.h"
#include "Skill.h"
#include "Wood_Dragon.h"
#include "Weapon.h"
#include "Player.h"
#include "UI_Monster_Status.h"
#include "Particle_Point.h"

CMonster_Samurai::CMonster_Samurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CMonster_Samurai::CMonster_Samurai(const CMonster_Samurai& rhs)
	: CLandObject(rhs)
{
}

HRESULT CMonster_Samurai::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Samurai::Initialize(void* pArg)
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

	if (FAILED(Add_Particles()))
		return E_FAIL;

	m_MaxHp		= 300.f;
	m_CurrentHp = 300.f;

	if (FAILED(Add_UIs()))
		return E_FAIL;

	m_CurrentState = MONSTER_STATE_IDLE;
	_vector vStart_Pos = { -7.f, 0.f, 10.f, 1.f };

	if (m_Current_Level == LEVEL_GAMEPLAY)
		vStart_Pos = { -7.f, 0.f, 10.f, 1.f };
	else if (m_Current_Level == LEVEL_BOSS)
		vStart_Pos = { -94.43f, 6.91f, 51.8f, 1.f };

	m_pTransformCom->Set_Pos(vStart_Pos);
	m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

	if (pGameObjectDesc->Current_Level == LEVEL_BOSS)
		m_bVitalize = false;

	return S_OK;
}

void CMonster_Samurai::Priority_Tick(_float fTimeDelta)
{
	if (m_bVitalize)
	{
		Set_Direction();
		RootAnimation();
		Set_Gravity(m_pTransformCom, fTimeDelta);
		//Control_State(fTimeDelta);
		State_Control(fTimeDelta);
		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Priority_Tick(fTimeDelta);

		if (m_bSkillOn[SKILL_WOOD_DRAGON])
			m_MonsterSkills.find(L"Skill_Wood_Dragon")->second->Priority_Tick(fTimeDelta);

		Particles_Priority_Tick(fTimeDelta);
		m_SmokeParticle->Priority_Tick(fTimeDelta);
	}
}

void CMonster_Samurai::Tick(_float fTimeDelta)
{
	if (m_bVitalize)
	{
		Skill_Tick(fTimeDelta);

		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
		m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());

		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Tick(fTimeDelta);

		if (m_bSkillOn[SKILL_WOOD_DRAGON])
			m_MonsterSkills.find(L"Skill_Wood_Dragon")->second->Tick(fTimeDelta);

		Particles_Tick(fTimeDelta);
		m_SmokeParticle->Tick(fTimeDelta);

	}
}

void CMonster_Samurai::Late_Tick(_float fTimeDelta)
{
	if (m_bVitalize)
	{
		__super::Late_Tick(fTimeDelta);

		//Collision_ToPlayer();
		m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider");
		m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderAttack, L"Player_Main_Collider", L"Monster_Attack_Collider");


		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Late_Tick(fTimeDelta);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (m_bSkillOn[SKILL_WOOD_DRAGON])
			m_MonsterSkills.find(L"Skill_Wood_Dragon")->second->Late_Tick(fTimeDelta);

		for (auto& Pair : m_MonsterUIs)
			Pair.second->Late_Tick(fTimeDelta);

		Particles_Late_Tick(fTimeDelta);
		m_SmokeParticle->Late_Tick(fTimeDelta);


#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
		m_pGameInstance->Add_DebugComponent(m_pColliderMain);
		m_pGameInstance->Add_DebugComponent(m_pColliderAttack);
#endif
	}
}

HRESULT CMonster_Samurai::Render()
{
	if (m_bVitalize)
	{
		if (m_bSkillOn[SKILL_WOOD_DRAGON])
			m_MonsterSkills.find(L"Skill_Wood_Dragon")->second->Render();
	}

	return S_OK;
}

void CMonster_Samurai::RootAnimation()
{
	// 움직임을 World이동으로치환
	_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	_float3 fInversemovevalue;
	fInversemovevalue.x = -fmovevalue.x;
	fInversemovevalue.y = 0.f;
	fInversemovevalue.z = -fmovevalue.y;
	m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

void CMonster_Samurai::State_Control(_float fTimeDelta)
{
	m_fNinjutsu -= fTimeDelta;
	m_fWaitingTime += fTimeDelta;
	m_fCoolTime_Rush_Current -= fTimeDelta;
	m_fCoolTime_SideDash_Current -= fTimeDelta;

	if (m_CurrentHp <= 0)
	{
		if (!m_bDeadCheck)
		{
			m_bDeadCheck = true;
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level, m_pColliderMain);
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level, m_pColliderAttack);
			m_pColliderMain->Delete_All_IsCollider();
			m_pColliderAttack->Delete_All_IsCollider();
			m_iState = MONSTER_DEAD;
			return;
		}
	}

	if (m_iState & MONSTER_RUSH_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.4f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.2f)
			On_Attack_Collider(0.7f, HIT_THROW);

		Dash_Move(DIR_FRONT, 0.95f, fTimeDelta);
	}
	else if (m_iState & MONSTER_COMBO_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.3f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.15f)
			On_Attack_Collider(0.7f);

		Dash_Move(DIR_FRONT, 0.85f, fTimeDelta);
	}
	else if (m_iState & MONSTER_DASH)
		Dash_Move(m_Dash_Dir, 0.96f, fTimeDelta);

	else
	{
		Off_Attack_Collider();
		m_ColliderDelay = 0.f;
	}

	if (m_iState & MONSTER_STRUCK)
		Dash_Move(DIR_FRONT, 0.75f, fTimeDelta);

	if (m_iState & MONSTER_STRUCK_STRONG)
		Dash_Move(DIR_FRONT, 0.85f, fTimeDelta);

	if (m_iState & MONSTER_THROW)
		Dash_Move(DIR_FRONT, 0.96f, fTimeDelta);

	if (m_iState & MONSTER_BEATEN)
		Dash_Move(DIR_FRONT, 0.96f, fTimeDelta);


	if (m_iState & MONSTER_RUN)
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

	if (!(m_iState & MONSTER_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop()))
		return;

	if (m_iState & MONSTER_DEAD)
	{
		m_bDead = true;
		return;
	}
	if (m_iState & MONSTER_BEATEN)
	{
		if (m_iState != MONSTER_BEATEN_END)
		{
			m_iState *= 2;
			return;
		}
		else
		{
			m_iState = MONSTER_GET_UP;
			return;
		}
	}
	m_pTransformCom->Set_Look(m_vDirection);

	m_iState = 0x00000000;

	if (m_fWaitingTime < 2.f)
	{
		m_CurrentState = MONSTER_STATE_IDLE;
		m_iState |= MONSTER_IDLE;
		return;
	}

	if (m_fNinjutsu < 0.f)
	{
		if ((!InComboAttackRange()) && CheckPlayer()) {
			Use_Skill(L"Skill_Wood_Dragon");
			m_CurrentState = MONSTER_STATE_NINJUTSU;
			//m_iState |= MONSTER_HANDSEAL;
			m_fNinjutsu = 20.f;
			m_iNinjutsuCount = 0;
			return;
		}
	}

	if (Skill_State(fTimeDelta))
		return;

	switch (m_CurrentState)
	{
	case MONSTER_STATE_IDLE:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = MONSTER_STATE_MOVE;
			m_iState |= MONSTER_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_MOVE:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
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
					m_iState |= MONSTER_RUN;
				else if (iRandom == 1) {
					m_iState |= MONSTER_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= MONSTER_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= MONSTER_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_RUSH_ATTACK:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
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
					m_iState |= MONSTER_RUN;
				else if (iRandom == 1) {
					m_iState |= MONSTER_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= MONSTER_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= MONSTER_RUN;
			break;
		}
		else if (CheckPlayer()){
			m_CurrentState = MONSTER_STATE_MOVE;
			m_iState |= MONSTER_RUN;
			break;
		} 
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}
	
		break;
	

	case MONSTER_STATE_COMBO_ATTACK:
	
		if (InComboAttackRange() && m_iComboCount ==0) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_2;
			m_iComboCount++;
			m_fDashSpeed = 10.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			break;
		}
		if (InComboAttackRange() && m_iComboCount == 1) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
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
					m_iState |= MONSTER_RUN;
				else if (iRandom == 1) {
					m_iState |= MONSTER_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= MONSTER_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= MONSTER_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}

		break;


	case MONSTER_STATE_NINJUTSU:

		if (m_iNinjutsuCount == 0) {
			m_CurrentState = MONSTER_STATE_NINJUTSU;
			m_iState |= MONSTER_NINJUTSU_ATTACK;
			m_iNinjutsuCount++;
			m_fWaitingTime = 0.f;
			return;
		}

		if (InComboAttackRange() ) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
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
					m_iState |= MONSTER_RUN;
				else if (iRandom == 1) {
					m_iState |= MONSTER_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= MONSTER_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= MONSTER_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}

		break;

	case MONSTER_STATE_STRUCK:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
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
					m_iState |= MONSTER_RUN;
				else if (iRandom == 1) {
					m_iState |= MONSTER_DASH_LEFT;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 15.f;
				}
				else if (iRandom == 2) {
					m_iState |= MONSTER_DASH_RIGHT;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 15.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= MONSTER_RUN;
			break;
		}
		else {
			m_CurrentState = MONSTER_STATE_IDLE;
			m_iState |= MONSTER_IDLE;
			break;
		}

		break;

	}
}

void CMonster_Samurai::Set_Direction()
{
	_vector Pos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
	_vector Dir = Pos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Dir.m128_f32[3] = 0.f;

	m_vDirection = Dir;
}

_bool CMonster_Samurai::CheckPlayer()
{
	if(XMVectorGetX(XMVector3Length(m_vDirection)) < 17.f)
		return true;

	return false;
}

_bool CMonster_Samurai::InRushAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 5.f)
		return true;

	return false;
}

_bool CMonster_Samurai::InComboAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 3.f)
		return true;

	return false;
}


void CMonster_Samurai::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
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
		if (pMyCollider == m_pColliderMain )
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
				m_pGameInstance->PlaySoundW("Hit_Strong", SOUND_PLAYER_HIT, 0.7f, true);
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (MONSTER_THROW);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 20;
			}
			else if(pTargetCollider->Get_HitType() == HIT_STRONG)
			{
				m_pGameInstance->PlaySoundW("Hit_Basic", SOUND_PLAYER_HIT, 0.7f, true);
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (MONSTER_STRUCK_STRONG_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 15;
			}
			else if (pTargetCollider->Get_HitType() == HIT_BEATEN)
			{
				m_pGameInstance->PlaySoundW("Hit_Strong", SOUND_PLAYER_HIT, 0.7f, true);
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (MONSTER_BEATEN_START);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 25;
			}
			else if(pTargetCollider->Get_HitType() == HIT_NORMAL)
			{
				m_pGameInstance->PlaySoundW("Hit_Basic", SOUND_PLAYER_HIT, 0.7f, true);
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (MONSTER_STRUCK_LEFT * m_iStruckState);
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
			m_iState = (MONSTER_BEATEN_START);
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
			m_iState = (MONSTER_ELECTRICSHOCK);
			m_fDashSpeed = 0.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 40;
		}
	}
	
}

void CMonster_Samurai::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
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
				m_iState = (MONSTER_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -4.f;
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
				m_iState = (MONSTER_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -3.f;
				m_fWaitingTime = 0.f;
				m_CurrentHp -= 10;

				_vector vParPos = m_MyPos;
				vParPos.m128_f32[1] += 0.7f;
				for (auto iter : m_KamuiParticles)
				{
					if (iter->Trigger(vParPos))
						break;
				}
			}
			m_fGetAttack_FrameCount--;
		}
	}
}

void CMonster_Samurai::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
		}
	}
	else if (strColliderLayerTag == L"Player_Attack_Collider")
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
			m_iState = MONSTER_BEATEN_START;
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
			m_iState = MONSTER_BEATEN_START;
			m_fDashSpeed = -15.f;
			m_fWaitingTime = 0.f;
			m_CurrentHp -= 20;
		}
	}
}

void CMonster_Samurai::On_Attack_Collider(_float Radius, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(Radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, 0.7f, Radius });
	m_pColliderAttack->Set_HitType(HitType);
}

void CMonster_Samurai::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f });
	//m_pColliderAttack->Set_HitType(HIT_NORMAL);

}


void CMonster_Samurai::Dash_Move(DASH_DIR dir, _float ratio, _float fTimeDelta)
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

void CMonster_Samurai::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_Wood_Dragon")
	{
		m_Skill_Animation_State = SKILL_WOOD_DRAGON;

		dynamic_cast<CWood_Dragon*>(m_MonsterSkills.find(L"Skill_Wood_Dragon")->second)->Set_State();

		m_iState |= MONSTER_HANDSEAL;

		m_bSkillOn[SKILL_WOOD_DRAGON] = true;
	}
}

_bool CMonster_Samurai::Skill_State(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_DRAGON])
	{
		if (m_iState & MONSTER_HANDSEAL)
		{
			m_Skill_Animation_State = SKILL_END;
			return false;

		}
	}
	return false;
}

void CMonster_Samurai::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_DRAGON])
	{
		CWood_Dragon* pWood_Dragon = dynamic_cast<CWood_Dragon*>(m_MonsterSkills.find(L"Skill_Wood_Dragon")->second);

		//위치 잡아주기

		if (pWood_Dragon->Get_State() == CWood_Dragon::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 0.8f)
			{
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 1.5f;
				pWood_Dragon->Get_TranformCom()->Set_Pos(Pos);
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 1.5f;
				_vector Dir = TargetPos - Pos;
				pWood_Dragon->Get_TranformCom()->Set_Look_IncludeY(Dir);
				pWood_Dragon->Set_Next_State();
			}
		}
		else if (pWood_Dragon->Get_State() == CWood_Dragon::STATE_FINISH)
		{
			m_bSkillOn[SKILL_WOOD_DRAGON] = false;
			m_fSkillDurTime = 0.f;
		}

	}
}

void CMonster_Samurai::Skill_Cancle()
{
	if (m_bSkillOn[SKILL_WOOD_DRAGON])
	{
		CWood_Dragon* pFlameBomb= dynamic_cast<CWood_Dragon*>(m_MonsterSkills.find(L"Skill_Wood_Dragon")->second);

		if (pFlameBomb->Get_State() == CWood_Dragon::STATE_MAKING)
			m_bSkillOn[SKILL_WOOD_DRAGON] = false;

	}
}

void CMonster_Samurai::Particles_Priority_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Priority_Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Priority_Tick(fTimeDelta);
	
}

void CMonster_Samurai::Particles_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Tick(fTimeDelta);
}

void CMonster_Samurai::Particles_Late_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Late_Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Late_Tick(fTimeDelta);
}

void CMonster_Samurai::Set_Vitalize()
{
	m_bVitalize = true; 
	m_SmokeParticle->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

HRESULT CMonster_Samurai::Add_Components()
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
		NaviDesc.iStartCellIndex = 69;
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

HRESULT CMonster_Samurai::Add_PartObjects()
{
	//Body_Monster 추가
	CBody_Monster_Samurai::BODY_MONSTER_DESC BodyMonsterDesc{};

	BodyMonsterDesc.pParentTransform = m_pTransformCom;
	BodyMonsterDesc.pState			= &m_iState;

	CPartObject* pBody_Monster = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Monster_Samurai"), &BodyMonsterDesc));
	if (nullptr == pBody_Monster)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Body"), pBody_Monster);

	CModel* pBodyModel = dynamic_cast<CModel*>(pBody_Monster->Get_Component(TEXT("Com_Model")));
	if (nullptr == pBodyModel)
		return E_FAIL;

	CWeapon::WEAPON_DESC			WeaponObjDesc{};
	WeaponObjDesc.pParentTransform = m_pTransformCom;
	WeaponObjDesc.pSocketMatrix = pBodyModel->Get_CombinedBoneMatrixPtr("L_Hand_Weapon_cnt_tr");
	WeaponObjDesc.vRotation = _float4(-0.541f, -0.541f, 0.644f, 246.f);
	WeaponObjDesc.vLocation = _float3(0.f, 0.f, 0.f);
	const char* charString = "Prototype_Component_Weapon_SamuraiSword";
	wstring strPrototypeTag(charString, charString + strlen(charString));
	WeaponObjDesc.strPrototypeTag = strPrototypeTag;

	//{0.081187, 0.024812, 0.003183}
	CPartObject* pWeaponL = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc));
	if (nullptr == pWeaponL)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Weapon1"), pWeaponL);

	CWeapon::WEAPON_DESC			WeaponObjDesc2{};
	WeaponObjDesc2.pParentTransform = m_pTransformCom;
	WeaponObjDesc2.pSocketMatrix = pBodyModel->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	WeaponObjDesc2.vRotation = _float4(-0.541f, -0.541f, 0.644f, 246.f);
	WeaponObjDesc2.vLocation = _float3(0.f, 0.f, 0.f);
	WeaponObjDesc2.strPrototypeTag = strPrototypeTag;
	//{0.07, 0.025, 0.0}

	
	CPartObject* pWeaponR = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc2));
	if (nullptr == pWeaponR)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Weapon2"), pWeaponR);

	m_pBodyModelCom = dynamic_cast<CModel*>(pBody_Monster->Get_Component(TEXT("Com_Model")));

	return S_OK;
}

HRESULT CMonster_Samurai::Add_Skills()
{
	CSkill::SKILL_DESC Skill_desc{};
	Skill_desc.pParentTransform = m_pTransformCom;
	Skill_desc.User_Type		= CSkill::USER_MONSTER;
	Skill_desc.Current_Level = m_Current_Level;

	 CSkill* pWood_Dragon = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Wood_Dragon"), &Skill_desc));
	 if (nullptr == pWood_Dragon)
	 	return E_FAIL;
	 m_MonsterSkills.emplace(TEXT("Skill_Wood_Dragon"), pWood_Dragon);
   
	return S_OK;
}

HRESULT CMonster_Samurai::Add_UIs()
{
	CUI_Monster_Status::UI_Monster_Hp_DESC Hp_Desc{};
	Hp_Desc.pCurrentHp = &m_CurrentHp;
	Hp_Desc.pMaxHp = &m_MaxHp;
	Hp_Desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Pointer();
	Hp_Desc.eMyType = CUI_Monster_Status::MONSTER_NORMAL;

	CUI_Monster_Status* pUIStatus = dynamic_cast<CUI_Monster_Status*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Monster_Hp"), &Hp_Desc));
	if (nullptr == pUIStatus)
		return E_FAIL;
	m_MonsterUIs.emplace(TEXT("UI_Player_Status"), pUIStatus);

	return S_OK;
}

HRESULT CMonster_Samurai::Add_Particles()
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

	CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc_Kamui{};
	InstanceDesc_Kamui.iNumInstance = 100;
	InstanceDesc_Kamui.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc_Kamui.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc_Kamui.vRange = _float3(0.1f, 0.1f, 0.1f);
	InstanceDesc_Kamui.vSize = _float2(0.02f, 0.03f);
	InstanceDesc_Kamui.vSpeed = _float2(0.5f, 0.7f);
	InstanceDesc_Kamui.vLifeTime = _float2(0.4f, 0.6f);
	InstanceDesc_Kamui.isLoop = false;
	InstanceDesc_Kamui.vColor = _float4(1.f, 120.f / 255.f, 0.f, 1.f);
	InstanceDesc_Kamui.fDuration = 0.7f;
	InstanceDesc_Kamui.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
	InstanceDesc_Kamui.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
	InstanceDesc_Kamui.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
	InstanceDesc_Kamui.strTextureTag = L"Prototype_Component_Texture_Circle";
	
	CParticle_Point* pParticle_Kamui_1 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_1)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_1);
	
	CParticle_Point* pParticle_Kamui_2 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_2)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_2);
	
	CParticle_Point* pParticle_Kamui_3 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_3)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_3);
	
	CParticle_Point* pParticle_Kamui_4 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_4)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_4);
	
	CParticle_Point* pParticle_Kamui_5 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_5)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_5);

	CParticle_Point* pParticle_Kamui_6 = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc_Kamui));
	if (nullptr == pParticle_Kamui_6)
		return E_FAIL;
	m_KamuiParticles.push_back(pParticle_Kamui_6);


	CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc1{};
	InstanceDesc1.iNumInstance = 20;
	InstanceDesc1.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc1.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc1.pCenter = &m_MyPos;
	InstanceDesc1.vRange = _float3(4.f, 4.f, 4.f);
	InstanceDesc1.vSize = _float2(2.f, 2.f);
	InstanceDesc1.vSpeed = _float2(0.1f, 0.2f);
	InstanceDesc1.vLifeTime = _float2(1.5f, 2.f);
	InstanceDesc1.isLoop = false;
	InstanceDesc1.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	InstanceDesc1.fDuration = 2.1f;
	InstanceDesc1.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
	InstanceDesc1.MyOption_Shape = CVIBuffer_Instancing::SHAPE_SQUARE;
	InstanceDesc1.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_SPRITE;
	InstanceDesc1.strTextureTag = L"Prototype_Component_Texture_Smoke_Sprite";
	InstanceDesc1.vSpriteRatio = _float2(4.f, 4.f);
	
	m_SmokeParticle = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc1));
	if (nullptr == m_SmokeParticle)
		return E_FAIL;


	return S_OK;
}

CMonster_Samurai* CMonster_Samurai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Samurai* pInstance = new CMonster_Samurai(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Samurai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Samurai::Clone(void* pArg)
{
	CMonster_Samurai* pInstance = new CMonster_Samurai(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Samurai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Samurai::Free()
{
	for (auto& Pair : m_MonsterParts)
		Safe_Release(Pair.second);
	m_MonsterParts.clear();

	for (auto& Pair : m_MonsterSkills)
		Safe_Release(Pair.second);
	m_MonsterSkills.clear();

	for (auto& Pair : m_MonsterUIs)
		Safe_Release(Pair.second);
	m_MonsterUIs.clear();

	for (auto& pParticle : m_BasicParticles)
		Safe_Release(pParticle);
	m_BasicParticles.clear();

	for (auto& pParticle : m_KamuiParticles)
		Safe_Release(pParticle);
	m_KamuiParticles.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderAttack);
	Safe_Release(m_SmokeParticle);

	__super::Free();
}

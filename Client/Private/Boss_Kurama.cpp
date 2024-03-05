#include "Boss_Kurama.h"
#include "Body_Boss_Kurama.h"
#include "Skill.h"
#include "Trail_Line.h"
#include "Player.h"
#include "UI_Boss_Status.h"
#include "Kurama_Scratch.h"
#include "FlameBomb.h"
#include "Particle_Point.h"

CBoss_Kurama::CBoss_Kurama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CBoss_Kurama::CBoss_Kurama(const CBoss_Kurama& rhs)
	: CLandObject(rhs)
{
}

HRESULT CBoss_Kurama::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Kurama::Initialize(void* pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;
	
	pGameObjectDesc->fSpeedPerSec = 6.f;
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
	
	if (FAILED(Add_Effects()))
		return E_FAIL;

	if (FAILED(Add_Particles()))
		return E_FAIL;

	m_MaxHp = 700.f;
	m_CurrentHp = 700.f;
	
	if (FAILED(Add_UIs()))
		return E_FAIL;
	
	m_CurrentState = BOSS_STATE_APPEAR;
	_vector vStart_Pos = { 77.f, 28.f, -60.f, 1.f };
	m_pTransformCom->Set_Pos(vStart_Pos);
	m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	
	return S_OK;
}

void CBoss_Kurama::Priority_Tick(_float fTimeDelta)
{
	if (m_bAppear)
	{
		if (m_pGameInstance->Key_Down(DIK_L))
		{
			_vector vParPos = m_MyPos;
			vParPos.m128_f32[1] += 0.7f;
			for (auto iter : m_BasicParticles)
			{
				if (iter->Trigger(vParPos))
					break;
			}
		}

		Cal_Direction();
		RootAnimation();
		Set_Gravity(m_pTransformCom, fTimeDelta);
		State_Control(fTimeDelta);

		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Priority_Tick(fTimeDelta);

		Skills_Priority_Tick(fTimeDelta);
		Particles_Priority_Tick(fTimeDelta);

	}
}

void CBoss_Kurama::Tick(_float fTimeDelta)
{
	if (m_bAppear)
	{
		Skill_Tick(fTimeDelta);

		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
		m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());

		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Tick(fTimeDelta);

		Skills_Tick(fTimeDelta);
		Particles_Tick(fTimeDelta);
		if (m_iState == BOSS_ATTACK_SCRATCH || m_iState == BOSS_RUSH_ATTACK)
		{
			Attack_Effect_Tick(fTimeDelta, m_iState);
		}

	}
}

void CBoss_Kurama::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bAppear)
	{
		m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider");
		m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderAttack, L"Player_Main_Collider", L"Monster_Attack_Collider");

		for (auto& Pair : m_MonsterParts)
			(Pair.second)->Late_Tick(fTimeDelta);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		for (auto& Pair : m_MonsterTrails)
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, Pair.second);

		if (m_bAppear_End)
		{
			for (auto& Pair : m_MonsterUIs)
				Pair.second->Late_Tick(fTimeDelta);
		}

		Skills_Late_Tick(fTimeDelta);
		Particles_Late_Tick(fTimeDelta);

		if (m_iState == BOSS_ATTACK_SCRATCH || m_iState == BOSS_RUSH_ATTACK)
		{
			Attack_Effect_Late_Tick(fTimeDelta, m_iState);
		}
		
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
		m_pGameInstance->Add_DebugComponent(m_pColliderMain);
		m_pGameInstance->Add_DebugComponent(m_pColliderAttack);
#endif
	}
}

HRESULT CBoss_Kurama::Render()
{
	if (m_bAppear)
	{
		Skills_Render();
	}

	return S_OK;
}

void CBoss_Kurama::RootAnimation()
{
	// 움직임을 World이동으로치환
	_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	_float3 fInversemovevalue;
	fInversemovevalue.x = -fmovevalue.x;
	fInversemovevalue.y = 0.f;
	fInversemovevalue.z = -fmovevalue.y;
	m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

void CBoss_Kurama::State_Control(_float fTimeDelta)
{	
	if (m_iState == BOSS_APPEAR)
	{
		m_fAppearDurTime += fTimeDelta;

		if (m_fAppearDurTime > 0.4f && m_fAppearDurTime < 0.5f)
		{
			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.2f);
			m_fAppearDurTime += 0.1f;
		}		
	}
	else if (m_iState == BOSS_APPEAR2)
	{
		m_fAppearDurTime2 += fTimeDelta;

		if (m_fAppearDurTime2 > 0.7f && m_fAppearDurTime2 < 0.8f)
		{
			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
			m_fAppearDurTime2 += 0.1f;
		}	
		else if (m_fAppearDurTime2 > 1.4f && m_fAppearDurTime2 < 1.5f)
		{
			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
			m_fAppearDurTime2 += 0.1f;
		}
		else if (m_fAppearDurTime2 > 2.4f && m_fAppearDurTime2 < 2.5f)
		{
			Set_Appear_Camera();
			m_fAppearDurTime2 += 0.1f;
		}
		else if (m_fAppearDurTime2 > 2.8f && m_fAppearDurTime2 < 2.9f)
		{
			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 4.f, 2.3f);
			m_fAppearDurTime2 += 0.1f;
		}
	}
	else 
		CoolTimeTick(fTimeDelta);


	if (m_CurrentHp <= 0)
	{
		if (!m_bDeadCheck)
		{
			m_bDeadCheck = true;
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level, m_pColliderMain);
			m_pGameInstance->Kill_Dead_Collider(m_Current_Level, m_pColliderAttack);
			m_pColliderMain->Delete_All_IsCollider();
			m_pColliderAttack->Delete_All_IsCollider();
			m_iState = BOSS_DEAD;
			return;
		}
	}

	if (m_iState & BOSS_RUSH_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 1.2f) {
			Off_Attack_Collider();
		}
		else if (m_ColliderDelay > 1.0)
		{
			if (!m_bCrash_Start) {
				m_Effect_Rush_Main->Start_Trigger();
				m_bCrash_Start = true;
				m_CrashMat = m_pTransformCom->Get_WorldMatrix();
				_vector Dir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				m_CrashMat.r[3] += XMVector3Normalize(Dir) * 2;
				m_CrashMat.r[3] += _vector{ 0.f, 0.001f ,0.f };
				
			}
			On_Attack_Collider(4.f, 3.f, HIT_BEATEN);
		}
		Dash_Move(DIR_FRONT, 0.96f, fTimeDelta);
	}
	else if (m_iState & BOSS_ATTACK_SCRATCH)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.35f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.2f)
			On_Attack_Collider(2.5f, 2.5f);
	}

	else if (m_iState & BOSS_ATTACK_KICK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.75f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.6f)
			On_Attack_Collider(2.5f, 2.5f);
		else if (m_ColliderDelay > 0.25f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.15f)
			On_Attack_Collider(2.5f, 2.5f);
	}

	else if (m_iState & BOSS_DASH) {
			Dash_Move(m_Dash_Dir, 0.96f, fTimeDelta);
	}
	else
	{
		Off_Attack_Collider();
		m_ColliderDelay = 0.f;
	}

	if (m_iState & BOSS_RUN)
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

	if (!(m_iState & BOSS_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop()))
		return;

	if (m_iState & BOSS_APPEAR)
	{
		m_CurrentState = BOSS_STATE_APPEAR;
		m_iState = BOSS_APPEAR2;
		return;
	}

	if ((m_iState & BOSS_DEAD) || m_iState & BOSS_DEAD_LOOP)
	{
		m_iState = BOSS_DEAD_LOOP;
		return;
	}

	Set_Direction_Lerf(0.01f);

	m_iState = 0x00000000;

	if (m_fWaitingTime < 1.5f)
	{
		m_CurrentState = BOSS_STATE_IDLE;
		m_iState |= BOSS_IDLE;
		return;
	}

	if ((!Using_Skill()) && (!InNormalAttackRange()) && CheckPlayer() )
	{
		if (m_fCoolTime_Scratch_Current < 0.f) {
			m_pTransformCom->Set_Look(m_vDirection);
			Use_Skill(L"Skill_Scratch");
			m_CurrentState = BOSS_STATE_ATTACK;
			m_fCoolTime_Scratch_Current = m_fCoolTime_Scratch;
			return;
		}		
		else if (m_fCoolTime_FireBall_Current < 0.f) {
			m_pTransformCom->Set_Look(m_vDirection);
			Use_Skill(L"Skill_FlameBomb_1");
			m_CurrentState = BOSS_STATE_ATTACK;
			m_fCoolTime_FireBall_Current = m_fCoolTime_FireBall;
			return;
		}
	}


	//if (Skill_State(fTimeDelta))
	//	return;

	switch (m_CurrentState)
	{
	case BOSS_STATE_APPEAR:

		m_bAppear_End = true;
		m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
		m_pCamera->Set_Camera_radius();

		if (InNormalAttackRange()) {
			m_CurrentState = BOSS_STATE_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			//m_iState |= (BOSS_ATTACK_SCRATCH * iRandom);
			m_iState |= (BOSS_ATTACK_SCRATCH);

			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			if(m_iState == BOSS_ATTACK_SCRATCH)
				m_Effect_Claw_Main->Start_Trigger();

			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = BOSS_STATE_ATTACK;
			m_iState |= BOSS_RUSH_ATTACK;
			m_fDashSpeed = 20.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = BOSS_STATE_MOVE;

			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= BOSS_RUN;
				else if (iRandom == 1) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 20.f;
				}
				else if (iRandom == 2) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 20.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= BOSS_RUN;

			break;
		}
		else {
			m_CurrentState = BOSS_STATE_IDLE;
			m_iState |= BOSS_IDLE;
			break;
		}

		break;


	case BOSS_STATE_IDLE:

		if (InNormalAttackRange()) {
			m_CurrentState = BOSS_STATE_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (BOSS_ATTACK_SCRATCH * iRandom);
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			if (m_iState == BOSS_ATTACK_SCRATCH)
				m_Effect_Claw_Main->Start_Trigger();
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <=0.f)) {
			m_CurrentState = BOSS_STATE_ATTACK;
			m_iState |= BOSS_RUSH_ATTACK;
			m_fDashSpeed = 20.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = BOSS_STATE_MOVE;

			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= BOSS_RUN;
				else if (iRandom == 1) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 20.f;
				}
				else if (iRandom == 2) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 20.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= BOSS_RUN;

			break;
		}
		else {
			m_CurrentState = BOSS_STATE_IDLE;
			m_iState |= BOSS_IDLE;
			break;
		}
		
		break;


	case BOSS_STATE_MOVE:

		if (InNormalAttackRange()) {
			m_CurrentState = BOSS_STATE_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (BOSS_ATTACK_SCRATCH * iRandom);
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			if (m_iState == BOSS_ATTACK_SCRATCH)
				m_Effect_Claw_Main->Start_Trigger();
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = BOSS_STATE_ATTACK;
			m_iState |= BOSS_RUSH_ATTACK;
			m_fDashSpeed = 20.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = BOSS_STATE_MOVE;

			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= BOSS_RUN;
				else if (iRandom == 1) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 20.f;
				}
				else if (iRandom == 2) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 20.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= BOSS_RUN;

			break;	
		}
		else {
			m_CurrentState = BOSS_STATE_IDLE;
			m_iState |= BOSS_IDLE;
			break;
		}

		break;


	case BOSS_STATE_ATTACK:

		if (InNormalAttackRange()) {
			m_CurrentState = BOSS_STATE_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (BOSS_ATTACK_SCRATCH * iRandom);
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			if (m_iState == BOSS_ATTACK_SCRATCH)
				m_Effect_Claw_Main->Start_Trigger();
			break;
		}
		else if (InRushAttackRange() && (m_fCoolTime_Rush_Current <= 0.f)) {
			m_CurrentState = BOSS_STATE_ATTACK;
			m_iState |= BOSS_RUSH_ATTACK;
			m_fDashSpeed = 20.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
			m_fCoolTime_Rush_Current = m_fCoolTime_Rush;
			break;
		}
		else if (CheckPlayer()) {
			m_CurrentState = BOSS_STATE_MOVE;

			if (m_fCoolTime_SideDash_Current <= 0.f) {
				_uint iRandom = (rand() % 3);
				if (iRandom == 0)
					m_iState |= BOSS_RUN;
				else if (iRandom == 1) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_LEFT;
					m_fDashSpeed = 20.f;
				}
				else if (iRandom == 2) {
					m_iState |= BOSS_DASH;
					m_Dash_Dir = DIR_RIGHT;
					m_fDashSpeed = 20.f;
				}
				m_fCoolTime_SideDash_Current = m_fCoolTime_SideDash;
			}
			else
				m_iState |= BOSS_RUN;

			break;	
		}
		else {
			m_CurrentState = BOSS_STATE_IDLE;
			m_iState |= BOSS_IDLE;
			break;
		}
		
		break;
	}
}

void CBoss_Kurama::Set_Direction_Lerf(_float ratio)
{
	_vector MyDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector Lerf_Dir;

	if (0 > XMVectorGetX(XMVector4Dot(MyDir, m_vDirection)))
		Lerf_Dir = XMVectorLerp(MyDir, m_vDirection, ratio * 2);

	else
		Lerf_Dir = XMVectorLerp(MyDir, m_vDirection, ratio);

	m_pTransformCom->Set_Look(Lerf_Dir);
}

void CBoss_Kurama::Cal_Direction()
{
	_vector Pos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
	_vector Dir = Pos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Dir.m128_f32[3] = 0.f;	
	m_vDirection = Dir;
}

_bool CBoss_Kurama::CheckPlayer()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 30.f)
		return true;

	return false;
}

_bool CBoss_Kurama::InRushAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 15.f)
		return true;

	return false;
}

_bool CBoss_Kurama::InNormalAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 5.f)
		return true;

	return false;
}

void CBoss_Kurama::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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
			_vector vParPos = m_MyPos;
			_vector	TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector	vDir = TargetPos - vParPos;
			vDir = XMVector3Normalize(vDir);
			vParPos += (vDir * 2);
			vParPos.m128_f32[1] += 1.0f;

			for (auto iter : m_BasicParticles)
			{
				if (iter->Trigger(vParPos))
					break;
			}

			m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 2.f, 0.05f);
		
			if (pTargetCollider->Get_HitType() == HIT_THROW)
				m_CurrentHp -= 20;
			
			else if (pTargetCollider->Get_HitType() == HIT_STRONG)
				m_CurrentHp -= 15;
			
			else if (pTargetCollider->Get_HitType() == HIT_BEATEN)
				m_CurrentHp -= 25;
			
			else if (pTargetCollider->Get_HitType() == HIT_NORMAL)
				m_CurrentHp -= 10;	
		}
	}
	else if (strColliderLayerTag == L"Rasengun_Collider" || strColliderLayerTag == L"FlameBomb_Collider" || strColliderLayerTag == L"Wood_Hand_Collider")
	{
		if (pMyCollider == m_pColliderMain)
			m_CurrentHp -= 30;		
	}
	else if (strColliderLayerTag == L"RasenShuriken_Collider")
	{
		if (pMyCollider == m_pColliderMain)
			m_fGetAttack_FrameCount = 0;
	}
	else if (strColliderLayerTag == L"Rasengun_Super_Collider")
	{
		if (pMyCollider == m_pColliderMain)
			m_fGetAttack_FrameCount = 0;		
	}
	else if (strColliderLayerTag == L"Kamui_Collider")
	{
		if (pMyCollider == m_pColliderMain)
			m_fGetAttack_FrameCount = 0;		
	}
	else if (strColliderLayerTag == L"Chidori_Collider")
	{
		if (pMyCollider == m_pColliderMain)
			m_CurrentHp -= 40;		
	}
}

void CBoss_Kurama::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

void CBoss_Kurama::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (m_bInvincible)
		return;

	 if (strColliderLayerTag == L"Kamui_Collider" || strColliderLayerTag == L"RasenShuriken_Collider" || strColliderLayerTag == L"Rasengun_Super_Collider")
	 {
	 	if (pMyCollider == m_pColliderMain)
	 	{
	 		m_fGetAttack_FrameCount = 0;
	 		m_CurrentHp -= 20;
	 	}
	 }
}

void CBoss_Kurama::On_Attack_Collider(_float radius, _float distance, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, radius -1.f, distance });
	m_pColliderAttack->Set_HitType(HitType);
}

void CBoss_Kurama::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f });
}

void CBoss_Kurama::Dash_Move(DASH_DIR dir, _float ratio, _float fTimeDelta)
{
	m_fDashSpeed = Lerp(0, m_fDashSpeed, ratio);

	if(dir == DIR_FRONT)
		m_pTransformCom->Go_Straight_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_BACK)
		m_pTransformCom->Go_Backward_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_LEFT)
		m_pTransformCom->Go_Left_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (dir == DIR_RIGHT)
		m_pTransformCom->Go_Right_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
}

void CBoss_Kurama::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_Scratch")
	{
		dynamic_cast<CKurama_Scratch*>(m_MonsterSkills.find(L"Skill_Scratch")->second)->Set_State();
	
		m_iState |= BOSS_ATTACK_SCRATCH_FAR;
	
		m_bSkillOn[SKILL_SCRATCH] = true;
	}

	else if (strSkillName == L"Skill_FlameBomb_1")
	{
		dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_1")->second)->Set_State();

		m_iState |= BOSS_ATTACK_FIREBALL;

		m_bSkillOn[SKILL_FLAMEBOMB_1] = true;
	}
	else if (strSkillName == L"Skill_FlameBomb_2")
	{
		dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_2")->second)->Set_State();

		m_bSkillOn[SKILL_FLAMEBOMB_2] = true;
	}
	else if (strSkillName == L"Skill_FlameBomb_3")
	{
		dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_3")->second)->Set_State();

		m_bSkillOn[SKILL_FLAMEBOMB_3] = true;
	}
}

_bool CBoss_Kurama::Skill_State(_float fTimeDelta)
{
	return _bool();
}

void CBoss_Kurama::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_SCRATCH])
	{
		CKurama_Scratch* pScratch = dynamic_cast<CKurama_Scratch*>(m_MonsterSkills.find(L"Skill_Scratch")->second);

		if (pScratch->Get_State() == CKurama_Scratch::STATE_MAKING)
		{
			m_fScratchDurTime += fTimeDelta;

			if (m_fScratchDurTime > 0.8f)
			{
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 2.5f;
				pScratch->Get_TranformCom()->Set_Pos(Pos);
				pScratch->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pScratch->Set_Targeting(TargetPos);
				pScratch->Set_Next_State();

			}
		}
		else if (pScratch->Get_State() == CKurama_Scratch::STATE_FINISH)
		{
			m_bSkillOn[SKILL_SCRATCH] = false;
			m_fScratchDurTime = 0.f;
		}

	}

	if (m_bSkillOn[SKILL_FLAMEBOMB_1])
	{
		m_pTransformCom->Set_Look(m_vDirection);

		CFlameBomb* pFlameBomb_1 = dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_1")->second);

		if (pFlameBomb_1->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_fFireBallDurTime_1 += fTimeDelta;

			if (m_fFireBallDurTime_1 > 0.7f)
			{
				if (!m_bSkillOn[SKILL_FLAMEBOMB_2])
					Use_Skill(L"Skill_FlameBomb_2");

				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 2.5f;
				pFlameBomb_1->Get_TranformCom()->Set_Pos(Pos);
				pFlameBomb_1->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pFlameBomb_1->Set_Targeting(TargetPos);
				pFlameBomb_1->Set_Next_State();

			}
		}
		else if (pFlameBomb_1->Get_State() == CFlameBomb::STATE_FINISH)
		{
			m_bSkillOn[SKILL_FLAMEBOMB_1] = false;
			m_fFireBallDurTime_1 = 0.f;
		}
	}

	if (m_bSkillOn[SKILL_FLAMEBOMB_2])
	{
		m_pTransformCom->Set_Look(m_vDirection);

		CFlameBomb* pFlameBomb_2 = dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_2")->second);

		if (pFlameBomb_2->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_fFireBallDurTime_2 += fTimeDelta;

			if (m_fFireBallDurTime_2 > 0.5f)
			{
				if (!m_bSkillOn[SKILL_FLAMEBOMB_3])
					Use_Skill(L"Skill_FlameBomb_3");

				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 2.5f;
				pFlameBomb_2->Get_TranformCom()->Set_Pos(Pos);
				pFlameBomb_2->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pFlameBomb_2->Set_Targeting(TargetPos);
				pFlameBomb_2->Set_Next_State();

			}
		}
		else if (pFlameBomb_2->Get_State() == CFlameBomb::STATE_FINISH)
		{
			m_bSkillOn[SKILL_FLAMEBOMB_2] = false;
			m_fFireBallDurTime_2 = 0.f;
		}
	}

	if (m_bSkillOn[SKILL_FLAMEBOMB_3])
	{
		m_pTransformCom->Set_Look(m_vDirection);

		CFlameBomb* pFlameBomb_3 = dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb_3")->second);

		if (pFlameBomb_3->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_fFireBallDurTime_3 += fTimeDelta;

			if (m_fFireBallDurTime_3 > 0.5f)
			{
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 2.5f;
				pFlameBomb_3->Get_TranformCom()->Set_Pos(Pos);
				pFlameBomb_3->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_Current_Level, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pFlameBomb_3->Set_Targeting(TargetPos);
				pFlameBomb_3->Set_Next_State();
			}
		}
		else if (pFlameBomb_3->Get_State() == CFlameBomb::STATE_FINISH)
		{
			m_bSkillOn[SKILL_FLAMEBOMB_3] = false;
			m_fFireBallDurTime_3 = 0.f;
		}
	}
}

_bool CBoss_Kurama::Using_Skill()
{
	for (_uint i = 0; i < SKILL_END; i++)
	{
		if (m_bSkillOn[i])
			return true;
	}
	return false;
}

void CBoss_Kurama::Attack_Effect_Tick(_float fTimeDelta, _ulonglong iState)
{
	_matrix ClawMat = m_pTransformCom->Get_WorldMatrix();
	_vector Dir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	ClawMat.r[3] += XMVector3Normalize(Dir)*2;
	ClawMat.r[3] += _vector{ 0.f, 0.7f ,0.f };

	if (iState == BOSS_ATTACK_SCRATCH)
	{
		m_Effect_Claw_Main->State_Tick(ClawMat);
		m_Effect_Claw_Main->Tick(fTimeDelta);
		m_Effect_Claw_Main->Scale_Change(fTimeDelta);
	}

	else if (iState == BOSS_RUSH_ATTACK)
	{
		if (m_bCrash_Start)
		{
			m_fEffect_DurTime += fTimeDelta;

			if (m_fEffect_DurTime > 1.f) {
				m_fEffect_DurTime = 0.f;
				m_bCrash_Start = false;
			}
			//m_CrashMat
			m_Effect_Rush_Main->State_Tick(ClawMat);
			m_Effect_Rush_Main->Tick(fTimeDelta);
			m_Effect_Rush_Main->Scale_Change(fTimeDelta);
		}
	}
}

void CBoss_Kurama::Attack_Effect_Late_Tick(_float fTimeDelta, _ulonglong iState)
{
	if (iState == BOSS_ATTACK_SCRATCH)
	{
		m_Effect_Claw_Main->Late_Tick(fTimeDelta);
	}
	else if (iState == BOSS_RUSH_ATTACK)
	{
		if (m_bCrash_Start)
		{
			m_Effect_Rush_Main->Late_Tick(fTimeDelta);
		}
	}
}

void CBoss_Kurama::CoolTimeTick(_float fTimeDelta)
{
	m_fWaitingTime += fTimeDelta;

	m_fCoolTime_Rush_Current		-= fTimeDelta;
	m_fCoolTime_SideDash_Current	-= fTimeDelta;
	m_fCoolTime_Scratch_Current		-= fTimeDelta;
	m_fCoolTime_FireBall_Current	-= fTimeDelta;
}

void CBoss_Kurama::Skills_Priority_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_SCRATCH])
		m_MonsterSkills.find(L"Skill_Scratch")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_1])
		m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_2])
		m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_3])
		m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Priority_Tick(fTimeDelta);
}

void CBoss_Kurama::Skills_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_SCRATCH])
		m_MonsterSkills.find(L"Skill_Scratch")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_1])
		m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_2])
		m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_3])
		m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Tick(fTimeDelta);
}

void CBoss_Kurama::Skills_Late_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_SCRATCH])
		m_MonsterSkills.find(L"Skill_Scratch")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_1])
		m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_2])
		m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB_3])
		m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Late_Tick(fTimeDelta);
}

void CBoss_Kurama::Skills_Render()
{
	if (m_bSkillOn[SKILL_SCRATCH])
		m_MonsterSkills.find(L"Skill_Scratch")->second->Render();
	if (m_bSkillOn[SKILL_FLAMEBOMB_1])
		m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Render();
	if (m_bSkillOn[SKILL_FLAMEBOMB_2])
		m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Render();
	if (m_bSkillOn[SKILL_FLAMEBOMB_3])
		m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Render();
}

void CBoss_Kurama::Particles_Priority_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Priority_Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Priority_Tick(fTimeDelta);

		m_MonsterSkills.find(L"Skill_Scratch")->second->Particles_Priority_Tick(fTimeDelta);
		m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Particles_Priority_Tick(fTimeDelta);
		m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Particles_Priority_Tick(fTimeDelta);
		m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Particles_Priority_Tick(fTimeDelta);
}

void CBoss_Kurama::Particles_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Tick(fTimeDelta);

	m_MonsterSkills.find(L"Skill_Scratch")->second->Particles_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Particles_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Particles_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Particles_Tick(fTimeDelta);
}

void CBoss_Kurama::Particles_Late_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Late_Tick(fTimeDelta);

	for (auto pParticle : m_KamuiParticles)
		pParticle->Late_Tick(fTimeDelta);

	m_MonsterSkills.find(L"Skill_Scratch")->second->Particles_Late_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_1")->second->Particles_Late_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_2")->second->Particles_Late_Tick(fTimeDelta);
	m_MonsterSkills.find(L"Skill_FlameBomb_3")->second->Particles_Late_Tick(fTimeDelta);
}

void CBoss_Kurama::Set_Appear_Camera()
{
	if (m_iState == BOSS_APPEAR)
	{
		m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_FREE);
		m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_FRONT);
		m_pCamera->Set_Camera_radius(7.f, 0.07f);
	}

	else if (m_iState == BOSS_APPEAR2)
	{
		m_pCamera->Set_Camera_radius(10.f, 0.15f);
	}
}

void CBoss_Kurama::Set_Appear()
{
	Set_Appear_Camera();
	m_bAppear = true;

}

HRESULT CBoss_Kurama::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};
	NaviDesc.iStartCellIndex = 29;
	if (FAILED(__super::Add_Component(m_Current_Level, TEXT("Prototype_Component_Navi_Map_Konoha_Village"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;
	

	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
	BoundingDesc.fRadius = 2.5f;
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

HRESULT CBoss_Kurama::Add_PartObjects()
{
	CBody_Boss_Kurama::BODY_MONSTER_DESC BodyBossDesc{};
	BodyBossDesc.pParentTransform = m_pTransformCom;
	BodyBossDesc.pState = &m_iState;

	CPartObject* pBody_Boss = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Boss_Kurama"), &BodyBossDesc));
	if (nullptr == pBody_Boss)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Body"), pBody_Boss);

	m_pBodyModelCom = dynamic_cast<CModel*>(pBody_Boss->Get_Component(TEXT("Com_Model")));

	return S_OK;
}

HRESULT CBoss_Kurama::Add_Skills()
{
	CSkill::SKILL_DESC Skill_desc{};
	Skill_desc.pParentTransform = m_pTransformCom;
	Skill_desc.User_Type		= CSkill::USER_MONSTER;
	Skill_desc.pCamera			= m_pCamera;
	Skill_desc.Current_Level	= m_Current_Level;	

	CSkill* pScratch = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Scratch"), &Skill_desc));
	if (nullptr == pScratch)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_Scratch"), pScratch);


	CSkill* pFlameBomb_1 = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_FlameBomb"), &Skill_desc));
	if (nullptr == pFlameBomb_1)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_FlameBomb_1"), pFlameBomb_1);

	CSkill* pFlameBomb_2 = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_FlameBomb"), &Skill_desc));
	if (nullptr == pFlameBomb_2)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_FlameBomb_2"), pFlameBomb_2);

	CSkill* pFlameBomb_3 = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_FlameBomb"), &Skill_desc));
	if (nullptr == pFlameBomb_3)
		return E_FAIL;
	m_MonsterSkills.emplace(TEXT("Skill_FlameBomb_3"), pFlameBomb_3);
	

	return S_OK;
}

HRESULT CBoss_Kurama::Add_Trails()
{
	CTrail_Line::Trail_Line_DESC Trail_Line_LH_Desc{};
	Trail_Line_LH_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_LH_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("LeftHandMiddle1");
	Trail_Line_LH_Desc.eMyCharacter = CTrail_Line::BOSS_KURAMA;
	CTrail_Line* pTrail_Hand_L = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_LH_Desc));
	if (nullptr == pTrail_Hand_L)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Hand_L"), pTrail_Hand_L);
	
	CTrail_Line::Trail_Line_DESC Trail_Line_RH_Desc{};
	Trail_Line_RH_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_RH_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("RightHandMiddle1");
	Trail_Line_RH_Desc.eMyCharacter = CTrail_Line::BOSS_KURAMA;
	CTrail_Line* pTrail_Hand_R = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_RH_Desc));
	if (nullptr == pTrail_Hand_R)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Hand_R"), pTrail_Hand_R);

	CTrail_Line::Trail_Line_DESC Trail_Line_LF_Desc{};
	Trail_Line_LF_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_LF_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("LeftFootMiddle1");
	Trail_Line_LF_Desc.eMyCharacter = CTrail_Line::BOSS_KURAMA;
	CTrail_Line* pTrail_Foot_L = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_LF_Desc));
	if (nullptr == pTrail_Foot_L)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Foot_L"), pTrail_Foot_L);

	CTrail_Line::Trail_Line_DESC Trail_Line_RF_Desc{};
	Trail_Line_RF_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_RF_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("RightFootMiddle1");
	Trail_Line_RF_Desc.eMyCharacter = CTrail_Line::BOSS_KURAMA;
	CTrail_Line* pTrail_Foot_R = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_RF_Desc));
	if (nullptr == pTrail_Foot_R)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Foot_R"), pTrail_Foot_R);

	/*CTrail_Line::Trail_Line_DESC Trail_Line_Tail_Desc{};
	Trail_Line_Tail_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_Tail_Desc.eMyCharacter = CTrail_Line::BOSS_KURAMA;

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailRight04Bone06");
	CTrail_Line* pTrail_Tail_1 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_1)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_1"), pTrail_Tail_1);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailRight03Bone06");
	CTrail_Line* pTrail_Tail_2 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_2)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_2"), pTrail_Tail_2);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailRight02Bone06");
	CTrail_Line* pTrail_Tail_3 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_3)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_3"), pTrail_Tail_3);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailRight01Bone06");
	CTrail_Line* pTrail_Tail_4 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_4)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_4"), pTrail_Tail_4);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailCenterBone06");
	CTrail_Line* pTrail_Tail_5 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_5)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_5"), pTrail_Tail_5);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailLeft01Bone06");
	CTrail_Line* pTrail_Tail_6 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_6)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_6"), pTrail_Tail_6);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailLeft02Bone06");
	CTrail_Line* pTrail_Tail_7 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_7)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_7"), pTrail_Tail_7);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailLeft03Bone06");
	CTrail_Line* pTrail_Tail_8 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_8)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_8"), pTrail_Tail_8);

	Trail_Line_Tail_Desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("TailLeft04Bone06");
	CTrail_Line* pTrail_Tail_9 = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_Tail_Desc));
	if (nullptr == pTrail_Tail_9)
		return E_FAIL;
	m_MonsterTrails.emplace(TEXT("Trail_Line_Tail_9"), pTrail_Tail_9);*/
	
	return S_OK;
}

HRESULT CBoss_Kurama::Add_UIs()
{
	CUI_Boss_Status::UI_Boss_Hp_DESC Hp_Desc{};
	Hp_Desc.pCurrentHp = &m_CurrentHp;
	Hp_Desc.pMaxHp = &m_MaxHp;
	Hp_Desc.eMyCharacter = CUI_Boss_Status::BOSS_KURAMA;
	
	CUI_Boss_Status* pUIStatus = dynamic_cast<CUI_Boss_Status*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_CUI_Boss_Status"), &Hp_Desc));
	if (nullptr == pUIStatus)
		return E_FAIL;
	m_MonsterUIs.emplace(TEXT("UI_Boss_Status"), pUIStatus);
	
	return S_OK;
}

HRESULT CBoss_Kurama::Add_Effects()
{
	CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
	Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_KURAMA_CLAW;
	Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_Y;
	Effect_Desc_1.MySpinOption = CEffect_Mesh::SPIN_NONE;
	Effect_Desc_1.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
	m_Effect_Claw_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
	if (nullptr == m_Effect_Claw_Main)
		return E_FAIL;

	CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
	Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_KURAMA_RUSH;
	Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_END;
	Effect_Desc_2.MySpinOption = CEffect_Mesh::SPIN_NONE;
	Effect_Desc_2.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
	m_Effect_Rush_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
	if (nullptr == m_Effect_Rush_Main)
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Kurama::Add_Particles()
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
	return S_OK;
}

CBoss_Kurama* CBoss_Kurama::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Kurama* pInstance = new CBoss_Kurama(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Kurama");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CBoss_Kurama::Clone(void* pArg)
{
	CBoss_Kurama* pInstance = new CBoss_Kurama(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Kurama");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Kurama::Free()
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

	for (auto& pParticle : m_KamuiParticles)
		Safe_Release(pParticle);
	m_KamuiParticles.clear();
	
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderAttack);

	Safe_Release(m_Effect_Claw_Main);
	Safe_Release(m_Effect_Rush_Main);

	__super::Free();
}

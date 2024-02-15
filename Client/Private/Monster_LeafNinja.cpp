#include "stdafx.h"
#include "Monster_LeafNinja.h"
#include "Body_Monster_LeafNinja.h"
#include "Skill.h"
#include "FlameBomb.h"
#include "Player.h"


CMonster_LeafNinja::CMonster_LeafNinja(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CMonster_LeafNinja::CMonster_LeafNinja(const CMonster_LeafNinja& rhs)
	: CLandObject(rhs)
{
}

HRESULT CMonster_LeafNinja::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_LeafNinja::Initialize(void* pArg)
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

	m_CurrentState = MONSTER_STATE_IDLE;
	_vector vStart_Pos = { 7.f, 0.f, 5.f, 1.f };
	m_pTransformCom->Set_Pos(vStart_Pos);
	m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom);
	
	return S_OK;
}

void CMonster_LeafNinja::Priority_Tick(_float fTimeDelta)
{
	Set_Direction();
	RootAnimation();
	Set_Gravity(m_pTransformCom, fTimeDelta);
	State_Control(fTimeDelta);

	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Priority_Tick(fTimeDelta);

	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_MonsterSkills.find(L"Skill_FlameBomb")->second->Priority_Tick(fTimeDelta);
}

void CMonster_LeafNinja::Tick(_float fTimeDelta)
{
	Skill_Tick(fTimeDelta);

	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Tick(fTimeDelta);

	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_MonsterSkills.find(L"Skill_FlameBomb")->second->Tick(fTimeDelta);
}

void CMonster_LeafNinja::Late_Tick(_float fTimeDelta)
{
	//Collision_ToPlayer();
	m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Player_Main_Collider");
	m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderAttack, L"Player_Main_Collider", L"Monster_Attack_Collider");


	for (auto& Pair : m_MonsterParts)
		(Pair.second)->Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_MonsterSkills.find(L"Skill_FlameBomb")->second->Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderMain);
	m_pGameInstance->Add_DebugComponent(m_pColliderAttack);
#endif
}

HRESULT CMonster_LeafNinja::Render()
{
	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_MonsterSkills.find(L"Skill_FlameBomb")->second->Render();

	return S_OK;
}

void CMonster_LeafNinja::RootAnimation()
{
	// 움직임을 World이동으로치환
	_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	_float3 fInversemovevalue;
	fInversemovevalue.x = -fmovevalue.x;
	fInversemovevalue.y = 0.f;
	fInversemovevalue.z = -fmovevalue.y;
	m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

void CMonster_LeafNinja::State_Control(_float fTimeDelta)
{
	m_fNinjutsu -= fTimeDelta;
	m_fWaitingTime += fTimeDelta;

	if (m_iState & MONSTER_RUSH_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.4f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.2f)
			On_Attack_Collider(0.7f, HIT_THROW);

		Dash_Move(0.95f, fTimeDelta);
	}
	else if (m_iState & MONSTER_COMBO_ATTACK)
	{
		m_ColliderDelay += fTimeDelta;

		if (m_ColliderDelay > 0.3f)
			Off_Attack_Collider();
		else if (m_ColliderDelay > 0.15f)
			On_Attack_Collider(0.7f);

		Dash_Move(0.85f, fTimeDelta);
	}
	else
	{
		Off_Attack_Collider();
		m_ColliderDelay = 0.f;
	}

	if (m_iState & MONSTER_STRUCK)
		Dash_Move(0.75f, fTimeDelta);

	if (m_iState & MONSTER_STRUCK_STRONG)
		Dash_Move(0.85f, fTimeDelta);

	if (m_iState & MONSTER_THROW)
		Dash_Move(0.96f, fTimeDelta);

	if (m_iState & MONSTER_BEATEN)
		Dash_Move(0.96f, fTimeDelta);


	if (m_iState & MONSTER_RUN)
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

	if (!(m_iState & MONSTER_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop()))
		return;

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

	if (m_fWaitingTime < 1.5f)
	{
		m_CurrentState = MONSTER_STATE_IDLE;
		m_iState |= MONSTER_IDLE;
		return;
	}

	if (m_fNinjutsu < 0.f)
	{
		if ((!InComboAttackRange()) && CheckPlayer()) {
			Use_Skill(L"Skill_FlameBomb");
			m_CurrentState = MONSTER_STATE_NINJUTSU;
			//m_iState |= MONSTER_HANDSEAL;
			m_fNinjutsu = 12.f;
			m_iNinjutsuCount = 0;
			return;
		}
	}

	//if (Skill_State(fTimeDelta))
	//	return;

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
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
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
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
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
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
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
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
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

	case MONSTER_STATE_STRUCK:

		if (InComboAttackRange()) {
			m_CurrentState = MONSTER_STATE_COMBO_ATTACK;
			m_iState |= MONSTER_COMBO_ATTACK_1;
			m_iComboCount = 0;
			m_fDashSpeed = 10.f;
			m_ColliderDelay = 0.f;
			break;
		}
		else if (InRushAttackRange()) {
			m_CurrentState = MONSTER_STATE_RUSH_ATTACK;
			_uint iRandom = (rand() % 2) + 1;
			m_iState |= (MONSTER_RUSH_ATTACK_PUNCH * iRandom);
			m_fDashSpeed = 15.f;
			m_fWaitingTime = 0.f;
			m_ColliderDelay = 0.f;
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

	}
}

void CMonster_LeafNinja::Set_Direction()
{
	_vector Pos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
	_vector Dir = Pos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Dir.m128_f32[3] = 0.f;

	m_vDirection = Dir;
}

_bool CMonster_LeafNinja::CheckPlayer()
{
	if(XMVectorGetX(XMVector3Length(m_vDirection)) < 17.f)
		return true;

	return false;
}

_bool CMonster_LeafNinja::InRushAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 5.f)
		return true;

	return false;
}

_bool CMonster_LeafNinja::InComboAttackRange()
{
	if (XMVectorGetX(XMVector3Length(m_vDirection)) < 3.f)
		return true;

	return false;
}


void CMonster_LeafNinja::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = m_pColliderMain->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = m_pColliderMain->Get_Radius() + pTargetCollider->Get_Radius();

			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));

			m_pTransformCom->Go_Custom_Direction(0.016f, 4, Dir, m_pNavigationCom);
		}
	}
	else if (strColliderLayerTag == L"Player_Attack_Collider")
	{
		if (pMyCollider == m_pColliderMain )
		{
			Skill_Cancle();

			_vector		MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		TargetPos = pTargetCollider->Get_Collider_GameObject()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
			_vector		vDir = TargetPos - MyPos;
			vDir.m128_f32[1] = 0.f;
			m_pTransformCom->Set_Look(vDir);

			if (pTargetCollider->Get_HitType() == HIT_THROW)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (MONSTER_THROW);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
			}
			else if(pTargetCollider->Get_HitType() == HIT_STRONG)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (MONSTER_STRUCK_STRONG_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
			}
			else if (pTargetCollider->Get_HitType() == HIT_BEATEN)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iState = (MONSTER_BEATEN_START);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
			}
			else if(pTargetCollider->Get_HitType() == HIT_NORMAL)
			{
				m_CurrentState = MONSTER_STATE_STRUCK;
				m_iStruckState++;
				if (m_iStruckState > 2)
					m_iStruckState = 1;
				m_iState = (MONSTER_STRUCK_LEFT * m_iStruckState);
				m_fDashSpeed = -15.f;
				m_fWaitingTime = 0.f;
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
		}
	}
	
}

void CMonster_LeafNinja::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
	if (strColliderLayerTag == L"Player_Main_Collider")
	{
		if (pMyCollider == m_pColliderMain)
		{
			_float3		MyCenter = m_pColliderMain->Get_Center();
			_float3		TargetCenter = pTargetCollider->Get_Center();
			_float		Length = m_pColliderMain->Get_Radius() + pTargetCollider->Get_Radius();

			_vector		Dir = XMVector3Normalize((XMLoadFloat3(&MyCenter) - XMLoadFloat3(&TargetCenter)));

			m_pTransformCom->Go_Custom_Direction(0.016f, 4, Dir, m_pNavigationCom);
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
			}
			m_fGetAttack_FrameCount--;
		}
	}

}

void CMonster_LeafNinja::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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
		}
	}
}

void CMonster_LeafNinja::On_Attack_Collider(_float Radius, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(Radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, 0.7f, Radius });
	m_pColliderAttack->Set_HitType(HitType);
}

void CMonster_LeafNinja::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f });
	//m_pColliderAttack->Set_HitType(HIT_NORMAL);

}


void CMonster_LeafNinja::Dash_Move(_float ratio, _float fTimeDelta)
{
	m_fDashSpeed = Lerp(0, m_fDashSpeed, ratio);
	m_pTransformCom->Go_Straight_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom);
}

void CMonster_LeafNinja::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_FlameBomb")
	{
		m_Skill_Animation_State = SKILL_FLAMEBOMB;

		dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb")->second)->Set_State();

		m_iState |= MONSTER_HANDSEAL;

		m_bSkillOn[SKILL_FLAMEBOMB] = true;
	}
}

//_bool CMonster_LeafNinja::Skill_State(_float fTimeDelta)
//{
//	if (m_bSkillOn[SKILL_FLAMEBOMB])
//	{
//		if (m_iState & MONSTER_HANDSEAL)
//		{
//			m_iState = MONSTER_NINJUTSU_ATTACK;
//			//dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb")->second)->Set_Next_State();
//	
//			return true;
//		}
//		else if (m_iState & MONSTER_NINJUTSU_ATTACK)
//		{
//			m_Skill_Animation_State = SKILL_END;
//			return false;
//		}
//	}
//	return false;
//}

void CMonster_LeafNinja::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_FLAMEBOMB])
	{
		CFlameBomb* pFlameBomb = dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb")->second);

		//위치 잡아주기

		if (pFlameBomb->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 0.8f)
			{
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 1.f;
				pFlameBomb->Get_TranformCom()->Set_Pos(Pos);
				pFlameBomb->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_vector TargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				TargetPos.m128_f32[1] += 0.7f;
				pFlameBomb->Set_Targeting(TargetPos);
				pFlameBomb->Set_Next_State();
				
			}
		}
		else if (pFlameBomb->Get_State() == CFlameBomb::STATE_FINISH)
		{
			m_bSkillOn[SKILL_FLAMEBOMB] = false;
			m_fSkillDurTime = 0.f;
		}

	}
}

void CMonster_LeafNinja::Skill_Cancle()
{
	if (m_bSkillOn[SKILL_FLAMEBOMB])
	{
		CFlameBomb* pFlameBomb= dynamic_cast<CFlameBomb*>(m_MonsterSkills.find(L"Skill_FlameBomb")->second);

		if (pFlameBomb->Get_State() == CFlameBomb::STATE_MAKING)
			m_bSkillOn[SKILL_FLAMEBOMB] = false;

	}
}

HRESULT CMonster_LeafNinja::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};
	NaviDesc.iStartCellIndex = 30;
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navi_Map_Stadium"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;
	
	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
	BoundingDesc.fRadius = 0.7f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(TEXT("Monster_Main_Collider"), m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);


	CBounding_Sphere::SPHERE_DESC		AttackBoundingDesc{};
	AttackBoundingDesc.fRadius = 0.f;
	AttackBoundingDesc.vCenter = _float3(0.f, AttackBoundingDesc.fRadius, AttackBoundingDesc.fRadius);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pColliderAttack), &AttackBoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider(L"Monster_Attack_Collider", m_pColliderAttack);
	m_pColliderAttack->Set_Collider_GameObject(this);
	return S_OK;                           
} 

HRESULT CMonster_LeafNinja::Add_PartObjects()
{
	//Body_Monster 추가
	CBody_Monster_LeafNinja::BODY_MONSTER_DESC BodyMonsterDesc{};

	BodyMonsterDesc.pParentTransform = m_pTransformCom;
	BodyMonsterDesc.pState			= &m_iState;

	CPartObject* pBody_Monster = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Monster_LeafNinja"), &BodyMonsterDesc));
	if (nullptr == pBody_Monster)
		return E_FAIL;
	m_MonsterParts.emplace(TEXT("Part_Body"), pBody_Monster);

	m_pBodyModelCom = dynamic_cast<CModel*>(pBody_Monster->Get_Component(TEXT("Com_Model")));

	return S_OK;
}

HRESULT CMonster_LeafNinja::Add_Skills()
{
	CSkill::SKILL_DESC Skill_desc{};
	Skill_desc.pParentTransform = m_pTransformCom;
	Skill_desc.User_Type		= CSkill::USER_MONSTER;

	 CSkill* pFlameBomb = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_FlameBomb"), &Skill_desc));
	 if (nullptr == pFlameBomb)
	 	return E_FAIL;
	 m_MonsterSkills.emplace(TEXT("Skill_FlameBomb"), pFlameBomb);
   
	return S_OK;
}

CMonster_LeafNinja* CMonster_LeafNinja::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_LeafNinja* pInstance = new CMonster_LeafNinja(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_LeafNinja");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_LeafNinja::Clone(void* pArg)
{
	CMonster_LeafNinja* pInstance = new CMonster_LeafNinja(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_LeafNinja");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_LeafNinja::Free()
{
	for (auto& Pair : m_MonsterParts)
		Safe_Release(Pair.second);
	m_MonsterParts.clear();

	for (auto& Pair : m_MonsterSkills)
		Safe_Release(Pair.second);
	m_MonsterSkills.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderAttack);

	__super::Free();
}

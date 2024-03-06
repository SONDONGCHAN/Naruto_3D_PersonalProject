#include "stdafx.h"
#include "Player_Custom.h"
#include "Body_Player_Naruto.h"
#include "Weapon.h"
#include "Camera_Free.h"
#include "FlameBomb.h"
#include "Kamui.h"
#include "Chidori.h"
#include "Wood_Hand.h"
#include "Wood_Swap.h"
#include "Trail_Line.h"
#include "Body_Player_Custom_Face.h"
#include "Body_Player_Custom_HeadGear.h"
#include "Body_Player_Custom_Upper.h"
#include "Body_Player_Custom_Lower.h"
#include "UI_Player_Status.h"
#include "UI_Player_Skills.h"
#include "UI_Player_Custom.h"
#include "Particle_Point.h"

CPlayer_Custom::CPlayer_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLandObject(pDevice, pContext)
{
}

CPlayer_Custom::CPlayer_Custom(const CPlayer_Custom& rhs)
    : CLandObject(rhs)
{
}

HRESULT CPlayer_Custom::Initialize_Prototype()
{
    return S_OK;
}

 HRESULT CPlayer_Custom::Initialize(void* pArg)
{
	 if (pArg != nullptr)
	 {
		 LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

		 pGameObjectDesc->fSpeedPerSec = 7.f;
		 pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

		 if (FAILED(__super::Initialize(pArg)))
			 return E_FAIL;

		 _vector vStart_Pos = { 0.f, 0.f, -10.f, 1.f };
		
		 if (m_Current_Level == LEVEL_GAMEPLAY)
			 vStart_Pos = { 0.f, 0.f, -10.f, 1.f };
		 else if (m_Current_Level == LEVEL_BOSS)
			 vStart_Pos = { 91.f, 26.f, -8.f, 1.f };
		 //else if (m_Current_Level == LEVEL_BOSS)
			 //vStart_Pos = { -114.5f, 23.f, 82.5f, 1.f };
		
		
		 // m_bOnAir = true;a
		 m_pTransformCom->Set_Pos(vStart_Pos);
		
		 if (FAILED(Add_Components()))
			 return E_FAIL;

		 if (FAILED(Add_PartObjects(m_Current_Level)))
			 return E_FAIL;

		 if(FAILED(Add_Weapon()))
			 return E_FAIL;

		 if (FAILED(Add_Skills()))
			 return E_FAIL;

		 if (FAILED(Add_Trails()))
			 return E_FAIL;

		 if (FAILED(Add_UIs()))
			 return E_FAIL;

		 if (FAILED(Add_Particles()))
			 return E_FAIL;


		 m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
		 m_bCustom_Mode = false;
	 }
	
	 else
	 {
		 LANDOBJ_DESC pGameObjectDesc;
		 pGameObjectDesc.fSpeedPerSec = 5.f;
		 pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		 if (FAILED(__super::Initialize(&pGameObjectDesc)))
			 return E_FAIL;

		 if (FAILED(Add_PartObjects(LEVEL_CUSTOMROOM)))
			 return E_FAIL;

		 if (FAILED(Add_Weapon()))
			 return E_FAIL;

		 if (FAILED(Add_CustomUI()))
			 return E_FAIL;
		 
		 _vector vStart_Pos = { 0.8f, -0.2f, 0.f, 1.f };
		 m_pTransformCom->Set_Pos(vStart_Pos);

		 m_bCustom_Mode = true;
	 }
	return S_OK;
}

void CPlayer_Custom::Priority_Tick(_float fTimeDelta)
{
	if (m_bCustom_Mode)
	{
		Model_Custom();

		_long	MouseMove = { 0 };

		if (MouseMove = m_pGameInstance->Mouse_Move(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(fTimeDelta * MouseMove * 0.05f));
		}

		for (auto& Pair : m_PlayerParts)
			(Pair.second)->Priority_Tick(fTimeDelta);
		for (auto& Pair : m_PlayerWeapon)
			(Pair.second)->Priority_Tick(fTimeDelta);
		
		return;
	}

	for (_uint i = 0; i < SKILL_END; i++)
	{
		m_fSkillCurrentCoolTime[i] -= fTimeDelta;
	
		if (m_fSkillCurrentCoolTime[i] <= 0)
			m_fSkillCurrentCoolTime[i] = 0.f;
	}

	m_LockOnTargetLength = 99999.f;

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
	for (auto& Pair : m_PlayerWeapon)
		(Pair.second)->Priority_Tick(fTimeDelta);

	m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider");
	m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderDetecting, L"Monster_Main_Collider");
	m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderAttack, L"Monster_Main_Collider", L"Player_Attack_Collider");
	
	bool bCurrentLand = m_bCellisLand;

	Player_Dash(fTimeDelta);
	Key_Input(fTimeDelta);

	if (bCurrentLand != m_bCellisLand)
	{
		int a = 0;
	}

	Skills_Priority_Tick(fTimeDelta);
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

void CPlayer_Custom::Tick(_float fTimeDelta)
{
	if (m_bCustom_Mode)
	{
		for (auto& Pair : m_PlayerParts)
			(Pair.second)->Tick(fTimeDelta);
		for (auto& Pair : m_PlayerWeapon)
			(Pair.second)->Tick(fTimeDelta);

		return;
	}

	m_fComboTime += fTimeDelta;

	Skill_Tick(fTimeDelta);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);
	for (auto& Pair : m_PlayerWeapon)
		(Pair.second)->Tick(fTimeDelta);
	
	Skills_Tick(fTimeDelta);
	Particles_Tick(fTimeDelta);

	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderDetecting->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Custom::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bCustom_Mode)
	{
		for (auto& Pair : m_PlayerParts)
			(Pair.second)->Late_Tick(fTimeDelta);
		for (auto& Pair : m_PlayerWeapon)
			(Pair.second)->Late_Tick(fTimeDelta);

		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_Player_Custom_UI->Late_Tick(fTimeDelta);

		return;
	}

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Late_Tick(fTimeDelta);
	for (auto& Pair : m_PlayerWeapon)
		(Pair.second)->Late_Tick(fTimeDelta);

	Skills_Late_Tick(fTimeDelta);
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

HRESULT CPlayer_Custom::Render()
{
	if (m_bCustom_Mode)
		return S_OK;

	Skills_Render();

	return S_OK;
}

void CPlayer_Custom::Add_MainCollider()
{
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Main_Collider", m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);
	m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
	
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Detecting_Collider", m_pColliderDetecting);
	m_pColliderDetecting->Set_Collider_GameObject(this);
	m_pColliderDetecting->Tick(m_pTransformCom->Get_WorldMatrix());
	
	m_pGameInstance->Add_Collider(m_Current_Level, L"Player_Attack_Collider", m_pColliderAttack);
	m_pColliderAttack->Set_Collider_GameObject(this);
	m_pColliderAttack->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Custom::Delete_MainCollider()
{
	m_pGameInstance->Delete_Collider(m_Current_Level, L"Player_Main_Collider", m_pColliderMain);
	m_pGameInstance->Delete_Collider(m_Current_Level, L"Player_Detecting_Collider", m_pColliderDetecting);
	m_pGameInstance->Delete_Collider(m_Current_Level, L"Player_Attack_Collider", m_pColliderAttack);
}

void CPlayer_Custom::Player_Dash(_float fTimeDelta)
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

	if (m_iState & PLAYER_STATE_CHIDORI_ATTACK)
		Dash_Move(0.f, 0.9f, fTimeDelta, DIR_FRONT);
}

void CPlayer_Custom::Player_Moving(_float fTimeDelta)
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

void CPlayer_Custom::Model_Custom()
{
	if (m_pGameInstance->Key_Down(DIK_RIGHT))
	{
		if (m_iCustom_Curser >= 3)
			return;

		m_iCustom_Curser++;
	}
	if (m_pGameInstance->Key_Down(DIK_LEFT))
	{
		if (m_iCustom_Curser <= 0)
			return;

		m_iCustom_Curser--;
	}

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iCustom_Curser == 0) 
			dynamic_cast<CBody_Player_Custom_HeadGear*>(m_PlayerParts.find(L"Part_Body_HeadGear")->second)->Set_Model(true);
		else if (m_iCustom_Curser == 1)
			dynamic_cast<CBody_Player_Custom_Face*>(m_PlayerParts.find(L"Part_Body_Face")->second)->Set_Model(true);
		else if (m_iCustom_Curser == 2)
		{
			CBody_Player_Custom_Upper* pParts = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second);
			pParts->Set_Model(true);
			_float4x4* pSocketMatrix = pParts->Get_CurrentModel()->Get_CombinedBoneMatrixPtr("Attach_Sword");

			for (auto& Pair : m_PlayerWeapon)
				dynamic_cast<CWeapon*>(Pair.second)->Set_SocketMatrixPtr(pSocketMatrix);
		}
		else if (m_iCustom_Curser == 3)
			dynamic_cast<CBody_Player_Custom_Lower*>(m_PlayerParts.find(L"Part_Body_Lower")->second)->Set_Model(true);

		for (auto& Pair : m_PlayerParts)
			Pair.second->Get_CurrentModel()->Reset_TrackPosition();
	}

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iCustom_Curser == 0)
			dynamic_cast<CBody_Player_Custom_HeadGear*>(m_PlayerParts.find(L"Part_Body_HeadGear")->second)->Set_Model(false);
		else if (m_iCustom_Curser == 1)
			dynamic_cast<CBody_Player_Custom_Face*>(m_PlayerParts.find(L"Part_Body_Face")->second)->Set_Model(false);
		else if (m_iCustom_Curser == 2)
		{
			CBody_Player_Custom_Upper* pParts = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second);
			pParts->Set_Model(false);
			_float4x4* pSocketMatrix = pParts->Get_CurrentModel()->Get_CombinedBoneMatrixPtr("Attach_Sword");

			for (auto& Pair : m_PlayerWeapon)
				dynamic_cast<CWeapon*>(Pair.second)->Set_SocketMatrixPtr(pSocketMatrix);
		}
		else if (m_iCustom_Curser == 3)
			dynamic_cast<CBody_Player_Custom_Lower*>(m_PlayerParts.find(L"Part_Body_Lower")->second)->Set_Model(false);

		for (auto& Pair : m_PlayerParts)
			Pair.second->Get_CurrentModel()->Reset_TrackPosition();
	}
	
}

void CPlayer_Custom::Key_Input(_float fTimeDelta)
{
	if (m_iState & PLAYER_HIT)
	{
		if (m_pGameInstance->Key_Down(DIK_Z))
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
			if (m_ColliderDelay > 0.35f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_COMBO_3)
		{
			if (m_ColliderDelay > 0.3f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.2f)
				On_Attack_Collider(0.7f);
		}
		else if (m_iState & PLAYER_STATE_COMBO_4)
		{
			if (m_ColliderDelay > 0.4f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.25f)
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

		_float4x4* pSocketMatrix = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second)->Get_CurrentModel()->Get_CombinedBoneMatrixPtr("R_Hand_Weapon_cnt_tr");
		dynamic_cast<CWeapon*>(m_PlayerWeapon.find(L"Part_Weapon")->second)->Set_SocketMatrixPtr(pSocketMatrix);
		
		if (m_iState & PLAYER_STATE_COMBO_STRONG_1)
		{
			if (m_ColliderDelay > 0.3f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.15f)
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
			if (m_ColliderDelay > 0.55f)
				Off_Attack_Collider();
			else if (m_ColliderDelay > 0.35f)
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
	if (!(m_iState & PLAYER_STATE_COMBO_STRONG_ATTACK))
	{
		_float4x4* pSocketMatrix = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second)->Get_CurrentModel()->Get_CombinedBoneMatrixPtr("Attach_Sword");
		dynamic_cast<CWeapon*>(m_PlayerWeapon.find(L"Part_Weapon")->second)->Set_SocketMatrixPtr(pSocketMatrix);
	}

	if (!(m_iState & PLAYER_STATE_MOVE) && !(m_pBodyModelCom->Get_Current_Animation()->Get_CanStop())) // 재생중에 멈추면 안되는 동작이 끝나지 않았음
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
			;
			m_iState = PLAYER_GET_UP;
			return;
		}
	}

	if (Skill_State(fTimeDelta))
		return;


	if (m_pBodyModelCom->Get_Consecutive())
		return;

	m_iState = 0x00000000;


	if ((m_pGameInstance->Mouse_Down(DIM_LB)))
	{
		if (!m_bOnAir)
		{
			m_pGameInstance->PlaySoundW("aaaa", SOUND_TEST, 0.8f, true);

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

			m_fGravity = Lerp(-0.4f, -1.f, m_fChargingtime / 3.f);
			m_fJumpSpeed = Lerp(7.f, 30.f, m_fChargingtime / 3.f);

			m_vJumpDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_fChargingtime = 0.f;
			return;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
	}

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
	else if (m_pGameInstance->Key_Down(DIK_1))
	{
		if (m_fSkillCurrentCoolTime[SKILL_FLAMEBOMB] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_FLAMEBOMB] = m_fSkillCoolTime[SKILL_FLAMEBOMB];
			Use_Skill(L"Skill_FlameBomb");
			return;
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		if (m_fSkillCurrentCoolTime[SKILL_CHIDORI] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_CHIDORI] = m_fSkillCoolTime[SKILL_CHIDORI];
			Use_Skill(L"Skill_Chidori");
			return;
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_3))
	{
		if (m_fSkillCurrentCoolTime[SKILL_WOODHAND] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_WOODHAND] = m_fSkillCoolTime[SKILL_WOODHAND];
			Use_Skill(L"Skill_Wood_Hand");
			return;
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_4))
	{
		if (m_fSkillCurrentCoolTime[SKILL_KAMUI] <= 0.f)
		{
			m_fSkillCurrentCoolTime[SKILL_KAMUI] = m_fSkillCoolTime[SKILL_KAMUI];
			Use_Skill(L"Skill_Kamui");
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
				if (Front_Dash())
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
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
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



	if ((!m_iState && (m_pBodyModelCom->Get_Current_Animation_Index() == 139) || m_pBodyModelCom->Get_Current_Animation_Index() == 177 || m_pBodyModelCom->Get_Current_Animation_Index() == 128))
	{
		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_IDLE;
		else
			m_iState |= PLAYER_STATE_FALL_LOOP;

	}

	if (!m_iState && m_pBodyModelCom->Get_Current_Animation()->Get_Finished())
	{
		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_IDLE;
		else
			m_iState |= PLAYER_STATE_FALL_LOOP;

	}
}

_float4 CPlayer_Custom::Cal_Direc_To_CameraBase(MOVEDIRECTION eMoveDirec)
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
		XMStoreFloat4(&fDirection, XMLoadFloat4(&front_dir) + XMLoadFloat4(&fDirection));
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

	XMStoreFloat4(&fDirection, XMVector3Normalize(XMLoadFloat4(&fDirection)));
	return fDirection;
}

void CPlayer_Custom::Set_Direc_Lerf(MOVEDIRECTION eMoveDirec, _float ratio)
{
	_float4 Target_Dir = Cal_Direc_To_CameraBase(eMoveDirec);
	_vector	vTarget_Dir = XMLoadFloat4(&Target_Dir);
	_vector vPlayer_Dir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector Lerf_Dir;

	if (0 > XMVectorGetX(XMVector4Dot(vPlayer_Dir, vTarget_Dir)))
		Lerf_Dir = XMVectorLerp(vPlayer_Dir, vTarget_Dir, ratio * 3);

	else
		Lerf_Dir = XMVectorLerp(vPlayer_Dir, vTarget_Dir, ratio);

	m_pTransformCom->Set_Look(Lerf_Dir);
}

void CPlayer_Custom::Dash_Move(_float End_Speed, _float ratio, _float fTimeDelta, MOVEDIRECTION DashDir)
{
	m_fDashSpeed = Lerp(End_Speed, m_fDashSpeed, ratio);
	if (DashDir == DIR_FRONT)
		m_pTransformCom->Go_Straight_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_BACK)
		m_pTransformCom->Go_Backward_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_LEFT)
		m_pTransformCom->Go_Left_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
	else if (DashDir == DIR_RIGHT)
		m_pTransformCom->Go_Right_Custom(fTimeDelta, m_fDashSpeed, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
}

void CPlayer_Custom::TranslateRootAnimation()
{
	// 움직임을 World이동으로치환
	_float3 fmovevalue = m_pBodyModelCom->Get_FramePos();
	_float3 fInversemovevalue;
	fInversemovevalue.x = -fmovevalue.x;
	fInversemovevalue.y = 0.f;
	fInversemovevalue.z = -fmovevalue.y;
	m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

_bool CPlayer_Custom::Front_Dash()
{
	if (m_pGameInstance->Key_Down(DIK_LSHIFT))
	{
		m_iState |= PLAYER_STATE_DASH_FRONT;
		m_fDashSpeed = 20.f;
		return true;
	}
	return false;
}

void CPlayer_Custom::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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
				if(iter->Trigger(vParPos))
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
				m_CurrentHp -= 20;
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

void CPlayer_Custom::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

void CPlayer_Custom::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

void CPlayer_Custom::On_Attack_Collider(_float radius, HIT_TYPE HitType)
{
	m_pColliderAttack->Set_Radius(radius);
	m_pColliderAttack->Set_Center(_float3{ 0.f, 0.7f, radius });
	m_pColliderAttack->Set_HitType(HitType);
}

void CPlayer_Custom::Off_Attack_Collider()
{
	m_pColliderAttack->Set_Radius(0.f);
	m_pColliderAttack->Set_Center(_float3{ 0.f, -9999.f, 0.f });
}

void CPlayer_Custom::Use_Skill(const wstring& strSkillName)
{
	if (strSkillName == L"Skill_Wood_Swap")
	{
		m_Skill_Animation_State = SKILL_WOOD_SWAP;

		dynamic_cast<CWood_Swap*>(m_PlayerSkills.find(L"Skill_Wood_Swap")->second)->Set_State();

		m_iState = 0x0000000000000000;
		m_iState |= PLAYER_STATE_FALL_LOOP;
		m_bOnAir = true;
		m_iJumpState = 1;
		m_fGravity = -0.25;

		m_bSkillOn[SKILL_WOOD_SWAP] = true;
	}
	else if (strSkillName == L"Skill_FlameBomb")
	{
		m_pCamera->Set_Camera_radius(2.f);

		m_Skill_Animation_State = SKILL_FLAMEBOMB;

		dynamic_cast<CFlameBomb*>(m_PlayerSkills.find(L"Skill_FlameBomb")->second)->Set_State();

		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_FLAME_BOMB;
		else
			m_iState |= PLAYER_STATE_AERIAL_FLAME_BOMB;

		//m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_TARGET_CHASE);
		//_vector* pPos = dynamic_cast<CLandObject*>(m_pLockOnTarget)->Get_MyPos();
		//m_pCamera->Set_Target_Position(pPos);

		m_bSkillOn[SKILL_FLAMEBOMB] = true;
	}
	else if (strSkillName == L"Skill_Chidori")
	{
		m_pCamera->Set_Camera_radius(2.f, 0.015f);

		m_Skill_Animation_State = SKILL_CHIDORI;
		
		dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Set_State();
		
		if (!m_bOnAir)
			m_iState |= PLAYER_STATE_CHIDORI_CHARGE;
		else
			m_iState |= PLAYER_STATE_AERIAL_CHIDORI_CHARGE;
		
		m_bSkillOn[SKILL_CHIDORI] = true;
	}
	else if (strSkillName == L"Skill_Wood_Hand")
	{
		if (!m_bOnAir)
		{
			m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_FREE);
			m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_FRONT);
			m_pCamera->Set_Camera_radius(2.f, 0.07f);

			m_Skill_Animation_State = SKILL_WOODHAND;

			dynamic_cast<CWood_Hand*>(m_PlayerSkills.find(L"Skill_Wood_Hand")->second)->Set_State();

			m_iState |= PLAYER_WOOD_HAND;

			m_bSkillOn[SKILL_WOODHAND] = true;

			m_bInvincible = true;
		}
	}
	else if (strSkillName == L"Skill_Kamui")
	{
		if (!m_bOnAir)
		{
			m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_FREE);
			m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_FRONT);
			m_pCamera->Set_Camera_radius(1.3f, 0.15f);

			m_Skill_Animation_State = SKILL_KAMUI;

			dynamic_cast<CKamui*>(m_PlayerSkills.find(L"Skill_Kamui")->second)->Set_State();

			m_iState |= PLAYER_STATE_KAMUI;

			m_bSkillOn[SKILL_KAMUI] = true;

			m_bInvincible = true;
		}
	}
}

_bool CPlayer_Custom::Skill_State(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_FLAMEBOMB])
	{
		if (m_iState & PLAYER_STATE_FLAME_BOMB || m_iState & PLAYER_STATE_AERIAL_FLAME_BOMB)
		{
			m_Skill_Animation_State = SKILL_END;
			return false;
		}
	}
	if (m_bSkillOn[SKILL_KAMUI])
	{
		if (m_iState & PLAYER_STATE_KAMUI )
		{
			m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
			m_pCamera->Set_Camera_radius();
			m_Skill_Animation_State = SKILL_END;
			m_bInvincible = false;
			m_bSkillOn[SKILL_KAMUI] = false;

			return false;
		}
	}
	if (m_bSkillOn[SKILL_CHIDORI])
	{
		if ( m_iState & PLAYER_STATE_CHIDORI_CHARGE)
		{
			m_iState = PLAYER_STATE_CHIDORI_RUN_LOOP;
			dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Set_Next_State();
			m_bInvincible = true;
			return true;
		}
		else if (m_iState & PLAYER_STATE_CHIDORI_RUN_LOOP)
		{
			if (m_LockOnTargetLength < 99999.f)
			{
				_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector LockOnDir = TargetPos - MyPos;
				m_pTransformCom->Set_Look(LockOnDir);
				m_pTransformCom->MoveTo(TargetPos, 50.f, fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
			}
			else
			{
				m_pTransformCom->Go_Straight_Custom(fTimeDelta, 50.f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);
			}
			if (dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Get_IsHit())
			{
				m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_FREE);
;				m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_FRONT);
				dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Set_Next_State();
				m_iState = PLAYER_STATE_CHIDORI_ATTACK;
				m_fDashSpeed = 40.f;
			}

			return true;
		}
		else if (m_iState & PLAYER_STATE_CHIDORI_ATTACK)
		{
			m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
			m_pCamera->Set_Camera_radius();
			m_bInvincible = false;
			m_bSkillOn[SKILL_CHIDORI] = false;
			m_Skill_Animation_State = SKILL_END;
			return false;
		}
		else if (m_iState & PLAYER_STATE_AERIAL_CHIDORI_CHARGE)
		{
			m_iState = PLAYER_STATE_AERIAL_CHIDORI_RUN_LOOP;
			dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Set_Next_State();
			m_bInvincible = true;
			return true;
		}
		else if (m_iState & PLAYER_STATE_AERIAL_CHIDORI_RUN_LOOP)
		{
			if (m_LockOnTargetLength < 99999.f)
			{
				_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
				_vector MyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector LockOnDir = TargetPos - MyPos;
				m_pTransformCom->Set_Look(LockOnDir);
				m_pTransformCom->MoveTo(TargetPos, 30.f, fTimeDelta, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

			}
			else
			{
				m_pTransformCom->Go_Straight_Custom(fTimeDelta, 30.f, m_pNavigationCom, m_bOnAir, &m_bCellisLand);

			}
			if (dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Get_IsHit())
			{
				dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second)->Set_Next_State();
				m_iState = PLAYER_STATE_AERIAL_CHIDORI_ATTACK;

			}
			return true;
		}
		else if (m_iState & PLAYER_STATE_AERIAL_CHIDORI_ATTACK)
		{
			m_pCamera->Set_Camera_radius();
			m_bInvincible = false;
			m_bSkillOn[SKILL_CHIDORI] = false;
			m_Skill_Animation_State = SKILL_END;
			return false;
		}
	}

	if (m_bSkillOn[SKILL_WOODHAND])
	{
		if (m_iState & PLAYER_WOOD_HAND)
		{
			m_Skill_Animation_State = SKILL_END;
			m_bInvincible = false;

			return false;
		}
	}
	return false;
}

void CPlayer_Custom::Skill_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_SWAP])
	{
		CWood_Swap* pWood_Swap = dynamic_cast<CWood_Swap*>(m_PlayerSkills.find(L"Skill_Wood_Swap")->second);

		if (pWood_Swap->Get_State() == CWood_Swap::STATE_FINISH)
		{
			m_bSkillOn[SKILL_WOOD_SWAP] = false;
		}
	}
	if (m_bSkillOn[SKILL_FLAMEBOMB])
	{
		CFlameBomb* pFlameBomb = dynamic_cast<CFlameBomb*>(m_PlayerSkills.find(L"Skill_FlameBomb")->second);

		if (pFlameBomb->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 0.5f)
			{
				m_pCamera->Set_Camera_radius();
				_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				Pos.m128_f32[1] += 1.f;
				pFlameBomb->Get_TranformCom()->Set_Pos(Pos);
				pFlameBomb->Get_TranformCom()->Set_Look(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				pFlameBomb->Set_Next_State();

				if (m_LockOnTargetLength < 99999.f)
				{
					_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					_vector SkillPos = pFlameBomb->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					_vector LockOnDir = TargetPos - SkillPos;
					pFlameBomb->Get_TranformCom()->Set_Look(LockOnDir);
					pFlameBomb->Set_Targeting(TargetPos);
				}

			}
		}
		else if (pFlameBomb->Get_State() == CFlameBomb::STATE_FINISH)
		{
			m_bSkillOn[SKILL_FLAMEBOMB] = false;
			m_fSkillDurTime = 0.f;
		}
	}
	if (m_bSkillOn[SKILL_KAMUI])
	{
		CKamui* pKamui= dynamic_cast<CKamui*>(m_PlayerSkills.find(L"Skill_Kamui")->second);

		if (pKamui->Get_State() == CKamui::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;

			if (m_fSkillDurTime > 0.7f)
			{
				m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_BACK);
				m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
				m_pCamera->Set_Camera_radius(2.f);

				
				_vector KamuiDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				_vector vKamuiPos = m_MyPos + (XMVector3Normalize(KamuiDir) * 7.f);
				vKamuiPos.m128_f32[1] += 0.7f;
				pKamui->Set_Targeting(vKamuiPos);
				//if (m_LockOnTargetLength < 99999.f)
				//{
					//_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					//TargetPos.m128_f32[1] += 0.7f;
					//pKamui->Set_Targeting(TargetPos);
				//}
				pKamui->Set_Next_State();
			}
		}
		else if (pKamui->Get_State() == CKamui::STATE_FINISH)
		{
			m_fSkillDurTime = 0.f;
		}
	}
	if (m_bSkillOn[SKILL_WOODHAND])
	{
		CWood_Hand* pWoodHand= dynamic_cast<CWood_Hand*>(m_PlayerSkills.find(L"Skill_Wood_Hand")->second);
	
		if (pWoodHand->Get_State() == CWood_Hand::STATE_MAKING)
		{
			m_fSkillDurTime += fTimeDelta;
	
			if (m_fSkillDurTime > 0.55f)
			{
				m_pCamera->Set_Camera_Point(&m_MyWorldMat, CCamera_Free::PLAYER_BACK);
				m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
				m_pCamera->Set_Camera_radius();
				
				if (m_LockOnTargetLength < 99999.f)
				{
					_vector TargetPos = m_pLockOnTarget->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
					TargetPos.m128_f32[2] -= 0.01f;
					TargetPos.m128_f32[1] -= 5.f;
					pWoodHand->Set_Targeting(TargetPos);
				}
				else
				{
					_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_vector Dir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					Pos += (Dir * 7.f);
					Pos.m128_f32[1] -= 5.f;
					pWoodHand->Set_Targeting(Pos);
				}
				pWoodHand->Set_Next_State();
			}
		}
		else if (pWoodHand->Get_State() == CWood_Hand::STATE_FINISH)
		{
			m_fSkillDurTime = 0.f;
			m_bSkillOn[SKILL_WOODHAND] = false;
		}
	}
}

void CPlayer_Custom::Skill_Cancle()
{
	if (m_bSkillOn[SKILL_FLAMEBOMB])
	{
		CFlameBomb* pFlameBomb = dynamic_cast<CFlameBomb*>(m_PlayerSkills.find(L"Skill_FlameBomb")->second);

		if (pFlameBomb->Get_State() == CFlameBomb::STATE_MAKING)
		{
			m_bSkillOn[SKILL_FLAMEBOMB] = false;
			m_fSkillDurTime = 0.f;
		}
	}
	if (m_bSkillOn[SKILL_CHIDORI])
	{
		CChidori* pChidori= dynamic_cast<CChidori*>(m_PlayerSkills.find(L"Skill_Chidori")->second);

		if (pChidori->Get_State() == CChidori::STATE_MAKING)
			m_bSkillOn[SKILL_CHIDORI] = false;

	}
	m_pCamera->Set_Camera_radius();
	m_pCamera->Set_Camera_State(CCamera_Free::CAMERA_PLAYER_CHASE);
}

void CPlayer_Custom::Skills_Priority_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_PlayerSkills.find(L"Skill_FlameBomb")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_KAMUI])
		m_PlayerSkills.find(L"Skill_Kamui")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_CHIDORI])
		m_PlayerSkills.find(L"Skill_Chidori")->second->Priority_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOODHAND])
		m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Priority_Tick(fTimeDelta);
}

void CPlayer_Custom::Skills_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_PlayerSkills.find(L"Skill_FlameBomb")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_KAMUI])
		m_PlayerSkills.find(L"Skill_Kamui")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_CHIDORI])
		m_PlayerSkills.find(L"Skill_Chidori")->second->Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOODHAND])
		m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Tick(fTimeDelta);
}

void CPlayer_Custom::Skills_Late_Tick(_float fTimeDelta)
{
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_PlayerSkills.find(L"Skill_FlameBomb")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_KAMUI])
		m_PlayerSkills.find(L"Skill_Kamui")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_CHIDORI])
		m_PlayerSkills.find(L"Skill_Chidori")->second->Late_Tick(fTimeDelta);
	if (m_bSkillOn[SKILL_WOODHAND])
		m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Late_Tick(fTimeDelta);
}

void CPlayer_Custom::Skills_Render()
{
	if (m_bSkillOn[SKILL_WOOD_SWAP])
		m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Render();
	if (m_bSkillOn[SKILL_FLAMEBOMB])
		m_PlayerSkills.find(L"Skill_FlameBomb")->second->Render();
	if (m_bSkillOn[SKILL_KAMUI])
		m_PlayerSkills.find(L"Skill_Kamui")->second->Render();
	if (m_bSkillOn[SKILL_CHIDORI])
		m_PlayerSkills.find(L"Skill_Chidori")->second->Render();
	if (m_bSkillOn[SKILL_WOODHAND])
		m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Render();
}

void CPlayer_Custom::Particles_Priority_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Priority_Tick(fTimeDelta);	

	m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Particles_Priority_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_FlameBomb")->second->Particles_Priority_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Kamui")->second->Particles_Priority_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Chidori")->second->Particles_Priority_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Particles_Priority_Tick(fTimeDelta);
}

void CPlayer_Custom::Particles_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Tick(fTimeDelta);

	m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Particles_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_FlameBomb")->second->Particles_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Kamui")->second->Particles_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Chidori")->second->Particles_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Particles_Tick(fTimeDelta);
}

void CPlayer_Custom::Particles_Late_Tick(_float fTimeDelta)
{
	for (auto pParticle : m_BasicParticles)
		pParticle->Late_Tick(fTimeDelta);

	m_PlayerSkills.find(L"Skill_Wood_Swap")->second->Particles_Late_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_FlameBomb")->second->Particles_Late_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Kamui")->second->Particles_Late_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Chidori")->second->Particles_Late_Tick(fTimeDelta);
	m_PlayerSkills.find(L"Skill_Wood_Hand")->second->Particles_Late_Tick(fTimeDelta);
}


HRESULT CPlayer_Custom::Add_Components()
{
	//////////* Com_Navigation */////////
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
		//NaviDesc.iStartCellIndex = 114;
		NaviDesc.iStartCellIndex = 22;

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
	DetectingBoundingDesc.vRadians = { 0.f , 0.f, 0.f };
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

HRESULT CPlayer_Custom::Add_PartObjects(LEVEL CurrentLevel)
{
	//Body_Player_Custom_Face 추가
	CBody_Player_Custom_Face::BODY_PLAYER_DESC BodyFaceDesc{};
	BodyFaceDesc.pParentTransform = m_pTransformCom;
	BodyFaceDesc.pPlayerState = &m_iState;
	BodyFaceDesc.pPlayerGravity = &m_fGravity;
	BodyFaceDesc.pPlayerGForcePersec = &m_fGForcePersec;
	BodyFaceDesc.Current_LEVEL = CurrentLevel;

	CPartObject* pBody_Face = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player_Custom_Face"), &BodyFaceDesc));
	if (nullptr == pBody_Face)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body_Face"), pBody_Face);
   
	//Body_Player_Custom_HeadGear 추가
	CBody_Player_Custom_HeadGear::BODY_PLAYER_DESC BodyHeadGearDesc{};
	BodyHeadGearDesc.pParentTransform = m_pTransformCom;
	BodyHeadGearDesc.pPlayerState = &m_iState;
	BodyHeadGearDesc.pPlayerGravity = &m_fGravity;
	BodyHeadGearDesc.pPlayerGForcePersec = &m_fGForcePersec;
	BodyHeadGearDesc.Current_LEVEL = CurrentLevel;

	CPartObject* pBody_HeadGear = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player_Custom_HeadGear"), &BodyHeadGearDesc));
	if (nullptr == pBody_HeadGear)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body_HeadGear"), pBody_HeadGear);

	//Body_Player_Custom_Lower 추가
	CBody_Player_Custom_Lower::BODY_PLAYER_DESC BodyLowerDesc{};
	BodyLowerDesc.pParentTransform = m_pTransformCom;
	BodyLowerDesc.pPlayerState = &m_iState;
	BodyLowerDesc.pPlayerGravity = &m_fGravity;
	BodyLowerDesc.pPlayerGForcePersec = &m_fGForcePersec;
	BodyLowerDesc.Current_LEVEL = CurrentLevel;
	CPartObject* pBody_Lower = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player_Custom_Lower"), &BodyLowerDesc));
	if (nullptr == pBody_Lower)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body_Lower"), pBody_Lower);

	//Body_Player_Custom_Upper 추가
	CBody_Player_Custom_Upper::BODY_PLAYER_DESC BodyUpperDesc{};
	BodyUpperDesc.pParentTransform = m_pTransformCom;
	BodyUpperDesc.pPlayerState = &m_iState;
	BodyUpperDesc.pPlayerGravity = &m_fGravity;
	BodyUpperDesc.pPlayerGForcePersec = &m_fGForcePersec;
	BodyUpperDesc.Current_LEVEL = CurrentLevel;
	CPartObject* pBody_Upper = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player_Custom_Upper"), &BodyUpperDesc));
	if (nullptr == pBody_Upper)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body_Upper"), pBody_Upper);

	m_pBodyModelCom = pBody_Upper->Get_CurrentModel();
	if (nullptr == m_pBodyModelCom)
		return E_FAIL;
	Safe_AddRef(m_pBodyModelCom);

	m_pBodyLowerCom = pBody_Lower->Get_CurrentModel();
	if (nullptr == m_pBodyLowerCom)
		return E_FAIL;
	Safe_AddRef(m_pBodyLowerCom);

	
	return S_OK;
}

HRESULT CPlayer_Custom::Add_Weapon()
{
	CWeapon::WEAPON_DESC			WeaponObjDesc{};
	WeaponObjDesc.pParentTransform = m_pTransformCom;
	WeaponObjDesc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("Attach_Sword");
	WeaponObjDesc.vRotation = _float4(1.f, 0.f, 0.f, 90.f);
	WeaponObjDesc.vLocation = _float3(0.f, 0.f, 0.f);
	const char* charString = "Prototype_Component_Weapon_NinjaSword";
	std::wstring strPrototypeTag(charString, charString + strlen(charString));
	WeaponObjDesc.strPrototypeTag = strPrototypeTag;

	//{0.081187, 0.024812, 0.003183}
	CPartObject* pWeapon = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PlayerWeapon.emplace(TEXT("Part_Weapon"), pWeapon);

	CWeapon::WEAPON_DESC			WeaponObjDesc2{};
	WeaponObjDesc2.pParentTransform = m_pTransformCom;
	WeaponObjDesc2.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("Attach_Sword");
	WeaponObjDesc2.vRotation = _float4(1.f, 0.f, 0.f, 90.f);
	WeaponObjDesc2.vLocation = _float3(0.f, 0.f, 0.f);
	const char* charString2 = "Prototype_Component_Weapon_NinjaSwordSheath";
	std::wstring strPrototypeTag2(charString2, charString2 + strlen(charString2));
	WeaponObjDesc2.strPrototypeTag = strPrototypeTag2;
	
	//{0.081187, 0.024812, 0.003183}
	CPartObject* pWeaponSheath = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc2));
	if (nullptr == pWeaponSheath)
		return E_FAIL;
	m_PlayerWeapon.emplace(TEXT("Part_WeaponSheath"), pWeaponSheath);

	return S_OK;
}

HRESULT CPlayer_Custom::Add_Skills()
{	
	// 호화구의 술
	CSkill::SKILL_DESC FlameBomb_desc{};
	FlameBomb_desc.pParentTransform = m_pTransformCom;
	FlameBomb_desc.User_Type = CSkill::USER_PLAYER;
	FlameBomb_desc.pCamera = m_pCamera;
	FlameBomb_desc.Current_Level = m_Current_Level;

    CSkill* pFlameBomb = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_FlameBomb"), &FlameBomb_desc));
	if (nullptr == pFlameBomb)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_FlameBomb"), pFlameBomb);
	m_fSkillCoolTime[SKILL_FLAMEBOMB] = 3.f;
	m_fSkillCurrentCoolTime[SKILL_FLAMEBOMB] = 0.f;

	// 치도리
	CChidori::SKILL_CHIDORI_DESC Chidori_desc{};
	Chidori_desc.pParentTransform = m_pTransformCom;
	Chidori_desc.User_Type = CSkill::USER_PLAYER;
	Chidori_desc.pCamera = m_pCamera;
	Chidori_desc.Current_Level = m_Current_Level;
	Chidori_desc.pSocketMatrix = m_pBodyModelCom->Get_CombinedBoneMatrixPtr("L_Hand_Weapon_cnt_tr");
	CSkill* pChidori = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Chidori"), &Chidori_desc));
	if (nullptr == pChidori)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Chidori"), pChidori);
	m_fSkillCoolTime[SKILL_CHIDORI] = 3.f;
	m_fSkillCurrentCoolTime[SKILL_CHIDORI] = 0.f;

	// 손뼉치기
	CSkill::SKILL_DESC WoodHand_desc{};
	WoodHand_desc.pParentTransform = m_pTransformCom;
	WoodHand_desc.User_Type = CSkill::USER_PLAYER;
	WoodHand_desc.pCamera = m_pCamera;
	WoodHand_desc.Current_Level = m_Current_Level;
	CSkill* pWoodHand= dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Wood_Hand"), &WoodHand_desc));
	if (nullptr == pWoodHand)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Wood_Hand"), pWoodHand);
	m_fSkillCoolTime[SKILL_WOODHAND] = 3.f;
	m_fSkillCurrentCoolTime[SKILL_WOODHAND] = 0.f;

	// 카무이
	CSkill::SKILL_DESC Kamui_desc{};
	Kamui_desc.pParentTransform = m_pTransformCom;
	Kamui_desc.User_Type = CSkill::USER_PLAYER;
	Kamui_desc.pCamera = m_pCamera;
	Kamui_desc.Current_Level = m_Current_Level;
	CSkill* pKamui = dynamic_cast<CSkill*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Kamui"), &Kamui_desc));
	if (nullptr == pKamui)
		return E_FAIL;
	m_PlayerSkills.emplace(TEXT("Skill_Kamui"), pKamui);
	m_fSkillCoolTime[SKILL_KAMUI] = 3.f;
	m_fSkillCurrentCoolTime[SKILL_KAMUI] = 0.f;

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

HRESULT CPlayer_Custom::Add_Trails()
{
	CTrail_Line::Trail_Line_DESC Trail_Line_L_Desc{};
	Trail_Line_L_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_L_Desc.pSocketMatrix = m_pBodyLowerCom->Get_CombinedBoneMatrixPtr("LeftFoot");
	Trail_Line_L_Desc.eMyCharacter = CTrail_Line::PLAYER_CUSTOM;

	CTrail_Line* pTrail_Foot_L = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_L_Desc));
	if (nullptr == pTrail_Foot_L)
		return E_FAIL;
	m_PlayerTrails.emplace(TEXT("Trail_Line_Foot_L"), pTrail_Foot_L);


	CTrail_Line::Trail_Line_DESC Trail_Line_R_Desc{};
	Trail_Line_R_Desc.pParentTransform = m_pTransformCom;
	Trail_Line_R_Desc.pSocketMatrix = m_pBodyLowerCom->Get_CombinedBoneMatrixPtr("RightFoot");
	Trail_Line_R_Desc.eMyCharacter = CTrail_Line::PLAYER_CUSTOM;

	CTrail_Line* pTrail_Foot_R = dynamic_cast<CTrail_Line*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Line"), &Trail_Line_R_Desc));
	if (nullptr == pTrail_Foot_R)
		return E_FAIL;
	m_PlayerTrails.emplace(TEXT("Trail_Line_Foot_R"), pTrail_Foot_R);

	return S_OK;
}

HRESULT CPlayer_Custom::Add_UIs()
{
	CUI_Player_Status::UI_Player_Hp_DESC Hp_Desc{};
	Hp_Desc.pCurrentHp			= &m_CurrentHp;
	Hp_Desc.pMaxHp				= &m_MaxHp;
	Hp_Desc.pCoolTime			= &m_fSkillCoolTime[SKILL_KAMUI];
	Hp_Desc.pCurrentCoolTime	= &m_fSkillCurrentCoolTime[SKILL_KAMUI];
	Hp_Desc.eMyCharacter		= CUI_Player_Status::PLAYER_CUSTOM;
	CUI_Player_Status* pUIStatus = dynamic_cast<CUI_Player_Status*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Status"), &Hp_Desc));
	if (nullptr == pUIStatus)
		return E_FAIL;
	m_PlayerUIs.emplace(TEXT("UI_Player_Status"), pUIStatus);
	
	CUI_Player_Skills::UI_Player_CoolTime_DESC Skill_Desc{};
	Skill_Desc.pCoolTime[0] = &m_fSkillCoolTime[SKILL_FLAMEBOMB];
	Skill_Desc.pCurrentCoolTime[0] = &m_fSkillCurrentCoolTime[SKILL_FLAMEBOMB];
	Skill_Desc.pCoolTime[1] = &m_fSkillCoolTime[SKILL_CHIDORI];
	Skill_Desc.pCurrentCoolTime[1] = &m_fSkillCurrentCoolTime[SKILL_CHIDORI];
	Skill_Desc.pCoolTime[2] = &m_fSkillCoolTime[SKILL_WOODHAND];
	Skill_Desc.pCurrentCoolTime[2] = &m_fSkillCurrentCoolTime[SKILL_WOODHAND];
	Skill_Desc.pCoolTime[3] = &m_fSkillCoolTime[SKILL_WOOD_SWAP];
	Skill_Desc.pCurrentCoolTime[3] = &m_fSkillCurrentCoolTime[SKILL_WOOD_SWAP];
	Skill_Desc.eMyCharacter = CUI_Player_Skills::PLAYER_CUSTOM;
	
	CUI_Player_Skills* pUISkills = dynamic_cast<CUI_Player_Skills*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Skills"), &Skill_Desc));
	if (nullptr == pUISkills)
		return E_FAIL;
	m_PlayerUIs.emplace(TEXT("UI_Player_Skills"), pUISkills);
	
	return S_OK;
}

HRESULT CPlayer_Custom::Add_Particles()
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
	InstanceDesc.MyOption_Moving = CVIBuffer_Instancing :: OPTION_SPREAD;
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

HRESULT CPlayer_Custom::Add_CustomUI()
{
	CUI_Player_Custom::UI_Player_Custom_DESC UI_Custom_Desc{};

	UI_Custom_Desc.pParts_Cursor[CUI_Player_Custom::PARTS_HEADGEAR] = dynamic_cast<CBody_Player_Custom_HeadGear*>(m_PlayerParts.find(L"Part_Body_HeadGear")->second)->Get_Current_Index();
	UI_Custom_Desc.iNum_Parts[CUI_Player_Custom::PARTS_HEADGEAR] = dynamic_cast<CBody_Player_Custom_HeadGear*>(m_PlayerParts.find(L"Part_Body_HeadGear")->second)->Get_Num_Models();

	UI_Custom_Desc.pParts_Cursor[CUI_Player_Custom::PARTS_FACE] = dynamic_cast<CBody_Player_Custom_Face*>(m_PlayerParts.find(L"Part_Body_Face")->second)->Get_Current_Index();
	UI_Custom_Desc.iNum_Parts[CUI_Player_Custom::PARTS_FACE] = dynamic_cast<CBody_Player_Custom_Face*>(m_PlayerParts.find(L"Part_Body_Face")->second)->Get_Num_Models();

	UI_Custom_Desc.pParts_Cursor[CUI_Player_Custom::PARTS_UPPER] = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second)->Get_Current_Index();
	UI_Custom_Desc.iNum_Parts[CUI_Player_Custom::PARTS_UPPER] = dynamic_cast<CBody_Player_Custom_Upper*>(m_PlayerParts.find(L"Part_Body_Upper")->second)->Get_Num_Models();

	UI_Custom_Desc.pParts_Cursor[CUI_Player_Custom::PARTS_LOWER] = dynamic_cast<CBody_Player_Custom_Lower*>(m_PlayerParts.find(L"Part_Body_Lower")->second)->Get_Current_Index();
	UI_Custom_Desc.iNum_Parts[CUI_Player_Custom::PARTS_LOWER] = dynamic_cast<CBody_Player_Custom_Lower*>(m_PlayerParts.find(L"Part_Body_Lower")->second)->Get_Num_Models();

	UI_Custom_Desc.pTitle_Cursor = &m_iCustom_Curser;

	m_Player_Custom_UI = dynamic_cast<CUI_Player_Custom*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Custom"), &UI_Custom_Desc));
	if (nullptr == m_Player_Custom_UI)
		return E_FAIL;

	return S_OK;
}

CPlayer_Custom* CPlayer_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Custom* pInstance = new CPlayer_Custom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Custom::Clone(void* pArg)
{
	CPlayer_Custom* pInstance = new CPlayer_Custom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Custom::Free()
{
	for (auto& Pair : m_PlayerParts)
		Safe_Release(Pair.second);
	m_PlayerParts.clear();
	
	for (auto& Pair : m_PlayerSkills)
		Safe_Release(Pair.second);
	m_PlayerSkills.clear();
	
	for (auto& Pair : m_PlayerWeapon)
		Safe_Release(Pair.second);
	m_PlayerWeapon.clear();

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
	Safe_Release(m_pBodyLowerCom);
	Safe_Release(m_pColliderMain);
	Safe_Release(m_pColliderDetecting);
	Safe_Release(m_pColliderAttack);
	
	//if(m_pLockOnTarget != nullptr)
		//Safe_Release(m_pLockOnTarget);

	if(m_bCustom_Mode)
		Safe_Release(m_Player_Custom_UI);

	__super::Free();
}

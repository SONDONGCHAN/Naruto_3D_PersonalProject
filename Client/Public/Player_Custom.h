#pragma once

#include "Client_Defines.h"
#include "LandObject.h"



BEGIN(Client)

class CPlayer_Custom : public CLandObject
{
	enum MOVEDIRECTION { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_FRONT_LEFT, DIR_FRONT_RIGHT, DIR_BACK_LEFT, DIR_BACK_RIGHT, DIR_END };
	enum SKILL_TYPE {SKILL_FLAMEBOMB, SKILL_CHIDORI, SKILL_WOODHAND, SKILL_WOOD_SWAP, SKILL_KAMUI, SKILL_END };

private:
	CPlayer_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Custom(const CPlayer_Custom& rhs);
	virtual ~CPlayer_Custom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Set_State(_ulonglong _state) { m_iState = _state; }
	CModel*		Get_BodyModel() { return m_pBodyModelCom; }
	void		Add_MainCollider();
	void		Delete_MainCollider();

private:
	void		Player_Dash(_float fTimeDelta);
	void		Player_Moving(_float fTimeDelta);

	void		Model_Custom();
	void		Key_Input(_float fTimeDelta);
	_float4		Cal_Direc_To_CameraBase(MOVEDIRECTION eMoveDirec);
	void		Set_Direc_Lerf(MOVEDIRECTION eMoveDirec, _float ratio);

	void		Dash_Move(_float End_Speed, _float ratio, _float fTimeDelta, MOVEDIRECTION DashDir);
	void		TranslateRootAnimation();

	///// 반복구문
	_bool		Front_Dash();

	/////////// 충돌이벤트/////////

	// 충돌이 시작됐을때 한번만 호출
	void		Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

	// 충돌이 지속되면 계속 호출
	void		Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

	// 충돌이 끝날때 한번만 호출
	void		Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

	// 콤보공격 콜라이더 제어
	void		On_Attack_Collider(_float radius, HIT_TYPE HitType = HIT_NORMAL);
	void		Off_Attack_Collider();

	// 스킬 사용
	void		Use_Skill(const wstring& strSkillName);
	_bool		Skill_State(_float fTimeDelta);
	void		Skill_Tick(_float fTimeDelta);
	void		Skill_Cancle();

	void		Skills_Priority_Tick(_float fTimeDelta);
	void		Skills_Tick(_float fTimeDelta);
	void		Skills_Late_Tick(_float fTimeDelta);
	void		Skills_Render();

	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta);
	void		Particles_Tick(_float fTimeDelta);
	void		Particles_Late_Tick(_float fTimeDelta);

private:
	CCollider* m_pColliderMain = { nullptr };
	CCollider* m_pColliderDetecting = { nullptr };
	CCollider* m_pColliderAttack = { nullptr };

	CModel* m_pBodyModelCom = { nullptr };	// 상체 모델
	CModel* m_pBodyLowerCom = { nullptr };	// 하체 모델

private:
	// 상태
	_ulonglong					m_iState = { PLAYER_STATE_IDLE };
	_float						m_fDashSpeed = { 0.f };

	//점프
	_vector						m_vJumpDirection = {};
	_float						m_fJumpSpeed = { 0.f };
	_float						m_fChargingtime = { 0.f };

	// 록온
	CGameObject*				m_pLockOnTarget = { nullptr };
	_float						m_LockOnTargetLength = 99999.f;

	//공격
	_float						m_ColliderDelay = 0.f;
	_float						m_fComboTime = { 1.f };		//콤보 연계 가능한 시간
	_ulonglong					m_iCombo = { 0x0000000000000000 };

	// 피격
	_uint						m_iStruckState = 0;
	_uint						m_iGetAttackFrameCount;

	// 스킬
	SKILL_TYPE					m_Skill_Animation_State = { SKILL_END };
	_bool						m_bSkillOn[SKILL_END] = { false, };
	_float						m_fSkillDurTime = { 0.f };
	_float						m_fSkillCoolTime[SKILL_END];
	_float						m_fSkillCurrentCoolTime[SKILL_END];

	// 무적
	_bool						m_bInvincible = false;

	// 커스텀
	_bool						m_bCustom_Mode = false;
	_uint						m_iCustom_Curser = { 0 };

private:
	map<const wstring, class CPartObject*>		m_PlayerParts;
	map<const wstring, class CPartObject*>		m_PlayerWeapon;
	map<const wstring, class CSkill*>			m_PlayerSkills;
	map<const wstring, class CTrail_Line*>		m_PlayerTrails;
	map<const wstring, class CUI*>				m_PlayerUIs;

	//map<const wstring, vector<class CParticle_Point*> >	m_ParticleClip;
	vector<class CParticle_Point*>	m_BasicParticles;


	CUI* m_Player_Custom_UI = {nullptr};

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects(LEVEL CurrentLevel);
	HRESULT Add_Weapon();
	HRESULT Add_Skills();
	HRESULT Add_Trails();
	HRESULT Add_UIs();
	HRESULT Add_Particles();


	HRESULT Add_CustomUI();


public:
	static CPlayer_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
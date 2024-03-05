#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Effect_Mesh.h"
#include "Particle_Point.h"

BEGIN(Client)

class CBoss_Kurama : public CLandObject
{
	enum SKILL_TYPE { SKILL_SCRATCH, SKILL_FLAMEBOMB_1, SKILL_FLAMEBOMB_2, SKILL_FLAMEBOMB_3, SKILL_END };
	enum DASH_DIR { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_END};


private:
	CBoss_Kurama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Kurama(const CBoss_Kurama& rhs);
	virtual ~CBoss_Kurama() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void		RootAnimation();

	// 상태 제어
	void		State_Control(_float fTimeDelta);
	void		Set_Direction_Lerf(_float ratio);

	// 패턴 계산 함수
	void		Cal_Direction();
	_bool		CheckPlayer();
	_bool		InRushAttackRange();
	_bool		InNormalAttackRange();

	// 충돌 이벤트
	void	Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

	// 공격
	void		On_Attack_Collider(_float radius, _float distance, HIT_TYPE HitType = HIT_NORMAL);
	void		Off_Attack_Collider();

private:
	// 이동 관련 
	void		Dash_Move(DASH_DIR dir, _float ratio, _float fTimeDelta);

	// 스킬 사용
	void		Use_Skill(const wstring& strSkillName);
	_bool		Skill_State(_float fTimeDelta);
	void		Skill_Tick(_float fTimeDelta);
	_bool		Using_Skill();

	// 공격 사용
	void		Attack_Effect_Tick(_float fTimeDelta, _ulonglong iState);
	void		Attack_Effect_Late_Tick(_float fTimeDelta, _ulonglong iState);


	void		CoolTimeTick(_float fTimeDelta);
	void		Skills_Priority_Tick(_float fTimeDelta);
	void		Skills_Tick(_float fTimeDelta);
	void		Skills_Late_Tick(_float fTimeDelta);
	void		Skills_Render();

	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta);
	void		Particles_Tick(_float fTimeDelta);
	void		Particles_Late_Tick(_float fTimeDelta);

public:
	// 카메라 연출
	void		Set_Appear_Camera();

public:
	void		Set_Appear();

private:
	CModel* m_pBodyModelCom = { nullptr };

	CCollider* m_pColliderMain = { nullptr };
	CCollider* m_pColliderAttack = { nullptr };


private:
	// 등장 트리거		
	_bool				m_bAppear = { false };
	_bool				m_bAppear_End = { false };
	_float				m_fAppearDurTime2 = { 0.f };
	_float				m_fAppearDurTime = { 0.f };


	// 상태
	_ulonglong				m_iState = { 0x00000010 };
	BOSS_STATE			m_CurrentState = { BOSS_STATE_APPEAR };

	// 움직임
	_vector				m_vDirection = {};
	_float				m_fDashSpeed = { 0.f };
	_uint				m_iComboCount = { 0 };
	_float				m_fWaitingTime = { 1.5f };
	DASH_DIR			m_Dash_Dir = { DIR_END };
	_float				m_fCoolTime_SideDash_Current = { 10.f };
	_float				m_fCoolTime_SideDash = { 20.f };

	// 피격
	_uint				m_fGetAttack_FrameCount = 0;

	//공격
	_float				m_ColliderDelay		= 0.f;
	_float				m_fCoolTime_Rush_Current = { 10.f };
	_float				m_fCoolTime_Rush = { 10.f };

	// 스킬
	_bool				m_bSkillOn[SKILL_END] = { false, };

	_float				m_fCoolTime_Scratch_Current = { 15.f };
	_float				m_fCoolTime_Scratch = { 15.f };
	_float				m_fScratchDurTime = { 0.f };

	_float				m_fCoolTime_FireBall_Current = { 20.f };
	_float				m_fCoolTime_FireBall = { 20.f };
	_float				m_fFireBallDurTime_1 = { 0.f };
	_float				m_fFireBallDurTime_2 = { 0.f };
	_float				m_fFireBallDurTime_3 = { 0.f };

	// 무적
	_bool				m_bInvincible = false;
	
	// 땜빵 변수
	_bool				m_bCrash_Start = false;
	_float				m_fEffect_DurTime = 0.f;
	_matrix				m_CrashMat = {};


private:
	map<const wstring, class CPartObject*>		m_MonsterParts;
	map<const wstring, class CSkill*>			m_MonsterSkills;
	map<const wstring, class CTrail_Line*>		m_MonsterTrails;
	map<const wstring, class CUI*>				m_MonsterUIs;
	//map<const wstring, vector<class CParticle_Point*> >	m_ParticleClip;
	vector<class CParticle_Point*>	m_BasicParticles;
	vector<class CParticle_Point*>	m_KamuiParticles;

private:
	CEffect_Mesh*		m_Effect_Claw_Main = { nullptr };
	CEffect_Mesh*		m_Effect_Rush_Main = { nullptr };

	//CParticle_Point*	m_BoomParticles = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Skills();
	HRESULT Add_Trails();
	HRESULT Add_UIs();
	HRESULT Add_Effects();
	HRESULT Add_Particles();

public:
	static CBoss_Kurama*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

END
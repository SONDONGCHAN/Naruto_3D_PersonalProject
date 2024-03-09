#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Client)

class CMonster_Samurai : public CLandObject
{
	enum SKILL_TYPE { SKILL_WOOD_DRAGON, SKILL_END };
	enum DASH_DIR { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_END };

private:
	CMonster_Samurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Samurai(const CMonster_Samurai& rhs);
	virtual ~CMonster_Samurai() = default;

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
	// 패턴 계산 함수
	void		Set_Direction();
	_bool		CheckPlayer();
	_bool		InRushAttackRange();
	_bool		InComboAttackRange();
	
	// 충돌 이벤트
	void	Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

	// 공격
	void		On_Attack_Collider(_float radius, HIT_TYPE HitType = HIT_NORMAL);
	void		Off_Attack_Collider();

private:
	// 이동 관련 
	void Dash_Move(DASH_DIR dir, _float ratio, _float fTimeDelta);

	// 스킬 사용
	void		Use_Skill(const wstring& strSkillName);
	_bool		Skill_State(_float fTimeDelta);
	void		Skill_Tick(_float fTimeDelta);
	void		Skill_Cancle();

	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta);
	void		Particles_Tick(_float fTimeDelta);
	void		Particles_Late_Tick(_float fTimeDelta);

private:
	CModel*		m_pBodyModelCom	= { nullptr };

	CCollider*	m_pColliderMain	= { nullptr };
	CCollider*	m_pColliderAttack = { nullptr };


private:
	// 상태
	_uint				m_iState		= { 0x00000001 };
	MONSTER_STATE		m_CurrentState	= { MONSTER_STATE_IDLE };
	
	// 움직임
	_vector				m_vDirection = {};
	_float				m_fDashSpeed = { 0.f };	
	_uint				m_iComboCount = { 0 };	
	_float				m_fWaitingTime= { 0.f };
	_float				m_fCoolTime_SideDash_Current = { 5.f };
	_float				m_fCoolTime_SideDash = { 10.f };
	DASH_DIR			m_Dash_Dir = { DIR_END };

	// 인술
	_uint				m_iNinjutsuCount = { 0 };
	_float				m_fNinjutsu = 15.f;

	// 피격
	_uint				m_iStruckState = 0;
	_uint				m_fGetAttack_FrameCount= 0;

	//공격
	_float				m_ColliderDelay = 0.f;
	_float				m_fCoolTime_Rush_Current = { 10.f };
	_float				m_fCoolTime_Rush = { 10.f };

	// 스킬
	SKILL_TYPE			m_Skill_Animation_State = { SKILL_END };
	_bool				m_bSkillOn[SKILL_END] = { false, };
	_float				m_fSkillDurTime = { 0.f };

	_bool				m_bVitalize = true;

public:
	void	Set_Vitalize();

private:
	map<const wstring, class CPartObject*>		m_MonsterParts;
	map<const wstring, class CSkill*>			m_MonsterSkills;
	map<const wstring, class CUI*>				m_MonsterUIs;
	//map<const wstring, vector<class CParticle_Point*> >	m_ParticleClip;
	vector<class CParticle_Point*>	m_BasicParticles;
	vector<class CParticle_Point*>	m_KamuiParticles;
	CParticle_Point* m_SmokeParticle = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Skills();
	HRESULT Add_UIs();
	HRESULT Add_Particles();

public:
	static CMonster_Samurai*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};

END
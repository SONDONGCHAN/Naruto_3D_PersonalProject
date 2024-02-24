#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Client)

class CBoss_Kurama : public CLandObject
{
	enum SKILL_TYPE { SKILL_SCRATCH, SKILL_END };
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
	void		Skill_Cancle();
	_bool		Using_Skill();

	void		CoolTimeTick(_float fTimeDelta);

private:
	CModel* m_pBodyModelCom = { nullptr };

	CCollider* m_pColliderMain = { nullptr };
	CCollider* m_pColliderAttack = { nullptr };


private:
	// 상태
	_ulonglong				m_iState = { 0x00000001 };
	BOSS_STATE			m_CurrentState = { BOSS_STATE_IDLE };

	// 움직임
	_vector				m_vDirection = {};
	_float				m_fDashSpeed = { 0.f };
	_uint				m_iComboCount = { 0 };
	_float				m_fWaitingTime = { 0.f };
	_float				m_CoolTime_SideDash = { 0.f };
	DASH_DIR			m_Dash_Dir = { DIR_END };

	// 피격
	_uint				m_fGetAttack_FrameCount = 0;

	//공격
	_float				m_ColliderDelay = 0.f;
	_float				m_CoolTime_Rush = 0.f;

	// 스킬
	SKILL_TYPE			m_Skill_Animation_State = { SKILL_END };
	_bool				m_bSkillOn[SKILL_END] = { false, };
	_float				m_fSkillDurTime = { 0.f };

	// 무적
	_bool				m_bInvincible = false;

private:
	map<const wstring, class CPartObject*>		m_MonsterParts;
	map<const wstring, class CSkill*>			m_MonsterSkills;
	map<const wstring, class CTrail_Line*>		m_MonsterTrails;
	map<const wstring, class CUI*>				m_MonsterUIs;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Skills();
	HRESULT Add_Trails();
	HRESULT Add_UIs();

public:
	static CBoss_Kurama* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;

};

END
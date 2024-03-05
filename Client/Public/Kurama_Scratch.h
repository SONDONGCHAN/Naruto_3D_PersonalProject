#pragma once
#include "Client_Defines.h"
#include "Skill.h"
#include "Effect_Mesh.h"
#include "Particle_Point.h"

BEGIN(Client)

class CKurama_Scratch : public CSkill
{
public:
	enum KURAMA_SCRATCH_STATE { STATE_MAKING, STATE_DETECTING, STATE_HIT, STATE_FINISH, STATE_END };

private:
	CKurama_Scratch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKurama_Scratch(const CKurama_Scratch& rhs);
	virtual ~CKurama_Scratch() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;
	void	State_Control(_float fTimeDelta);

public:
	void	Set_Next_State();
	void	Set_State();
	void	Set_Targeting(_vector Target_Pos);

	KURAMA_SCRATCH_STATE	Get_State() { return myState; }

public:
	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta) override;
	void		Particles_Tick(_float fTimeDelta) override;
	void		Particles_Late_Tick(_float fTimeDelta) override;

private:
	CCollider*			m_pColliderMain = { nullptr };
	CEffect_Mesh*		m_Effect_Scratch_Main= { nullptr };
	CParticle_Point*	m_BoomParticles = { nullptr };

private:
	KURAMA_SCRATCH_STATE myState = { STATE_MAKING };
	_bool		m_bIsHit = false;
	_bool		m_bTargeting = false;
	_float		m_fDurTime = 0.f;
	_vector		m_vTarget_Pos = {};

private:
	HRESULT Add_Components();
	HRESULT Add_Effects();
	HRESULT Add_Particles();

public:
	static CKurama_Scratch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
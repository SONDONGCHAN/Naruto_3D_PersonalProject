#pragma once
#include "Client_Defines.h"
#include "Skill.h"

#include "Effect_Mesh.h"
#include "Particle_Point.h"

BEGIN(Client)

class CRasengun_Super : public CSkill
{
public:
	enum RASENGUN_SUPER_STATE { STATE_MAKING, STATE_HIT, STATE_DISSOLVE, STATE_FINISH, STATE_END };

	struct SKILL_RASENGUN_SUPER_DESC : public  CSkill::SKILL_DESC
	{
		_float4x4* pSocketMatrix;
	};

private:
	CRasengun_Super(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasengun_Super(const CRasengun_Super& rhs);
	virtual ~CRasengun_Super() = default;

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

	RASENGUN_SUPER_STATE	Get_State() { return myState; }

public:
	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta) override;
	void		Particles_Tick(_float fTimeDelta) override;
	void		Particles_Late_Tick(_float fTimeDelta) override;

private:
	void		Deco_Control_Tick(_float fTimeDelta);
	void		Deco_Control_Late_Tick(_float fTimeDelta);
	void		Deco_Start_Trigger();


private:
	CCollider* m_pColliderMain = { nullptr };

	CEffect_Mesh* m_Effect_Rasengun_Super_Main = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Noise = { nullptr };
								
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_1 = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_2 = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_3 = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_4 = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_5 = { nullptr };
	CEffect_Mesh* m_Effect_Rasengun_Super_Deco_6 = { nullptr };

	CParticle_Point* m_Making_Particles = { nullptr };
	CParticle_Point* m_Boom_Particles = { nullptr };


private:
	RASENGUN_SUPER_STATE myState = { STATE_MAKING };
	_float		m_fDurTime = 0.f;
	_float4x4* m_pSocketMatrix = { nullptr };
	_matrix  	m_OriginalMat = {};
	_matrix		m_FinalMat = {};

	_bool		m_bIsEnd = false;
	_bool		m_bHitStart = false;
	_float		m_fCheckDelay = 0.f;
	_float		m_fMaking_Delay= 0.f;
	_bool		m_bMakeParticle = false;

private:
	HRESULT Add_Components();
	HRESULT Add_Effects();
	HRESULT Add_Particles();

public:
	static CRasengun_Super* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
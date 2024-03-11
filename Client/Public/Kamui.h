#pragma once
#include "Client_Defines.h"
#include "Skill.h"
#include "Effect_Mesh.h"

BEGIN(Client)

class CKamui : public CSkill
{
public:
	enum KAMUI_STATE { STATE_MAKING, STATE_HIT, STATE_FINISH, STATE_END };

private:
	CKamui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKamui(const CKamui& rhs);
	virtual ~CKamui() = default;

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

	KAMUI_STATE	Get_State() { return myState; }

public:
	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta) override;
	void		Particles_Tick(_float fTimeDelta) override;
	void		Particles_Late_Tick(_float fTimeDelta) override;

private:
	CCollider*		m_pColliderMain = { nullptr };
	CEffect_Mesh*	m_Effect_Kamui_Boom = { nullptr };
	CEffect_Mesh*	m_Effect_Kamui_ShockWave = { nullptr };

private:
	KAMUI_STATE myState = { STATE_MAKING };
	_float		m_fDurTime = 0.f;
	_bool		m_bTargeting = false;
	_vector		m_vTarget_Pos = {};

private:
	HRESULT Add_Components();
	HRESULT Add_Effects();

public:
	static CKamui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
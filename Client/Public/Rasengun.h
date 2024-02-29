#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Client)

class CRasengun : public CSkill
{
public:
	enum RASENGUN_STATE {STATE_MAKING, STATE_DETECTING, STATE_HIT, STATE_FINISH, STATE_END };

	struct SKILL_RASENGUN_DESC : public  CSkill::SKILL_DESC
	{
		_float4x4* pSocketMatrix;
	};

private:
	CRasengun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasengun(const CRasengun& rhs);
	virtual ~CRasengun() = default;

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

	RASENGUN_STATE	Get_State() { return myState; }
	_bool	Get_IsHit() { return m_bIsHit;  }

public:
	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta) override;
	void		Particles_Tick(_float fTimeDelta) override;
	void		Particles_Late_Tick(_float fTimeDelta) override;


private:
	CCollider*	m_pColliderMain = { nullptr };

private:
	RASENGUN_STATE myState = { STATE_MAKING };
	_bool		m_bIsHit = false;
	_float		m_fDurTime = 0.f;
	_float4x4*	m_pSocketMatrix = { nullptr }; // 손바닥 뼈 위치를 받아오자
	_matrix  	m_OriginalMat = {};

private:
	HRESULT Add_Components();


public:
	static CRasengun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
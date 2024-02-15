#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Client)

class CRasenShuriken : public CSkill
{
public:
	enum RASENSHURIKEN_STATE { STATE_MAKING, STATE_DETECTING, STATE_HIT, STATE_FINISH, STATE_END };

	struct SKILL_RASENSHURIKEN_DESC : public  CSkill::SKILL_DESC
	{
		_float4x4* pSocketMatrix;
	};

private:
	CRasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRasenShuriken(const CRasenShuriken& rhs);
	virtual ~CRasenShuriken() = default;

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


	RASENSHURIKEN_STATE	Get_State() { return myState; }

private:
	CCollider* m_pColliderMain = { nullptr };

private:
	RASENSHURIKEN_STATE myState = { STATE_MAKING };
	_bool		m_bIsHit = false;
	_bool		m_bTargeting = false;
	_float		m_fDurTime = 0.f;
	_vector		m_vTarget_Pos = {};
	_float4x4*	m_pSocketMatrix = { nullptr };
	_matrix  	m_OriginalMat = {};

private:
	HRESULT Add_Components();


public:
	static CRasenShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
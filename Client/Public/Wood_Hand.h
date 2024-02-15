#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Client)

class CWood_Hand : public CSkill
{
public:
	enum WOODD_HAND_STATE { STATE_MAKING, STATE_DETECTING, STATE_HIT, STATE_FINISH, STATE_END };

private:
	CWood_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWood_Hand(const CWood_Hand& rhs);
	virtual ~CWood_Hand() = default;

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

	WOODD_HAND_STATE	Get_State() { return myState; }

private:
	CCollider* m_pColliderMain = { nullptr };

private:
	WOODD_HAND_STATE		myState = { STATE_MAKING };
	_float					m_fDurTime = 0.f;
	_bool					m_bTargeting = false;
	_vector					m_vTarget_Pos = {};
	vector<CModel*>			m_vModels;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources(_float4x4 _WorldMatrix);

public:
	static CWood_Hand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Client)

class CWood_Dragon : public CSkill
{
public:
	enum WOOD_DRAGON_STATE { STATE_MAKING, STATE_HIT, STATE_FINISH, STATE_END };


private:
	CWood_Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWood_Dragon(const CWood_Dragon& rhs);
	virtual ~CWood_Dragon() = default;

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
	void	RootAnimation();

	WOOD_DRAGON_STATE	Get_State() { return myState; }

private:
	CCollider* m_pColliderMain = { nullptr };

private:
	WOOD_DRAGON_STATE myState = { STATE_MAKING };
	_bool		m_bIsHit = false;
	_float		m_fDurTime = 0.f;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWood_Dragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
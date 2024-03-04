#pragma once
#include "Client_Defines.h"
#include "Skill.h"
#include "Effect_Mesh.h"
#include "Particle_Point.h"

BEGIN(Client)

class CWood_Swap : public CSkill
{
public:
	enum WOOD_SWAP_STATE { STATE_APPEAR, STATE_RENDER, STATE_FINISH };

	struct SKILL_WOOD_SWAP_DESC : public  CSkill::SKILL_DESC
	{
		CNavigation* pUser_Navigation;
	};
private:
	CWood_Swap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWood_Swap(const CWood_Swap& rhs);
	virtual ~CWood_Swap() = default;

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
	void	Set_User_Pos();

	WOOD_SWAP_STATE	Get_State() { return myState; }

public:
	// 파티클 제어
	void		Particles_Priority_Tick(_float fTimeDelta) override;
	void		Particles_Tick(_float fTimeDelta) override;
	void		Particles_Late_Tick(_float fTimeDelta) override;

private:
	CEffect_Mesh*		m_Effect_WoodSwap_Main = { nullptr };
	CParticle_Point*	m_SmokeParticles	= { nullptr };
	
private:
	WOOD_SWAP_STATE		myState = { STATE_APPEAR };
	_float				m_fDurTime = 0.f;
	random_device		m_RandomDevice;
	mt19937_64			m_RandomNumber;
	CNavigation*		m_pUser_Navigation;

	_matrix				m_MyMat = {};

private:
	HRESULT Add_Components();
	HRESULT Add_Effects();
	HRESULT Add_Particles();

public:
	static CWood_Swap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END
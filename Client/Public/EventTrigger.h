#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Camera_Free.h"
#include "Effect_Mesh.h"
#include "Particle_Point.h"

BEGIN(Client)
class CEventTrigger : public CGameObject
{
public:
	enum TRIGGER_TYPE {
		TRIGGER_BOSS,
		TRIGGER_CHECK_POINT_1,
		TRIGGER_CHECK_POINT_2,
		TRIGGER_CHECK_POINT_3,
		TRIGGER_CHECK_POINT_4,
		TRIGGER_CHECK_POINT_5,
		TRIGGER_CHECK_POINT_6,
		TRIGGER_CHECK_POINT_7,
		TRIGGER_CHECK_POINT_8,
		TRIGGER_END};

	struct EVENTTRIGGER_DESC 
	{
		LEVEL			Current_Level;
		_vector			vPos;
		TRIGGER_TYPE	Trigger_Type;
	};
	 
private:
	CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventTrigger(const CEventTrigger& rhs);
	virtual ~CEventTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();
	HRESULT Add_Effects();
	HRESULT Add_Particles();

	void	Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;

public:
	void	Set_Vitalize() { m_bVitalize = true ;}

private:
	CCollider*			m_pColliderMain = { nullptr };
	EVENTTRIGGER_DESC	m_MyDesc = {};
	_bool				m_bUsed		= false;
	_bool				m_bVitalize = false;

	CEffect_Mesh*		m_Effect_CheckPoint = { nullptr };
	CEffect_Mesh*		m_Effect_Line		= { nullptr };

	CParticle_Point*	m_CheckPoint_Particles = { nullptr };


public:
	static CEventTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
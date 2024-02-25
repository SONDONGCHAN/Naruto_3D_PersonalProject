#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Camera_Free.h"

BEGIN(Client)
class CEventTrigger : public CGameObject
{
public:
	struct EVENTTRIGGER_DESC 
	{
		LEVEL		Current_Level;
		_vector		vPos;
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

	void	Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider) override;


private:
	CCollider*			m_pColliderMain = { nullptr };
	EVENTTRIGGER_DESC	m_MyDesc = {};
	_bool				m_bUsed = false;


public:
	static CEventTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
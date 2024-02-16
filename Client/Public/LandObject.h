#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	struct LANDOBJ_DESC : public GAMEOBJECT_DESC
	{
		CTransform*				pMapTransform = { nullptr };
	};

protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float			Lerp(_float start, _float end, _float ratio);

public:
	CNavigation*	Get_Navigation() { return m_pNavigationCom ;}

protected:
	CTransform*		m_pMapTransform = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };


protected:
	// ม฿ทย
	_float	m_fGravity = 0.f;
	_float	m_fMaxGravity = 0.3f;
	_float	m_fGForcePersec	 = 1.f;
	_bool	m_bOnAir = false;
	_uint	m_iJumpState = 0;

	_float	m_CurrentHp = 1000.f;
	_float	m_MaxHp		= 1000.f;


protected:
	_bool Set_Gravity(class CTransform* pTargetTransform, _float fTimeDelta);
	HRESULT SetUp_OnCell(class CTransform* pTargetTransform);
		
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
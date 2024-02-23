#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Camera_Free.h"

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
		CTransform*		pMapTransform	= { nullptr };
		CCamera_Free*	pCamera			= { nullptr };
		LEVEL			Current_Level;
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
	CCamera_Free*	m_pCamera		= { nullptr };
	//

	CNavigation*	m_pNavigationCom = { nullptr };

protected:
	// 중력
	_float	m_fGravity = 0.f;
	_float	m_fMaxGravity = 0.3f;
	_float	m_fGForcePersec	 = 1.f;
	_bool	m_bOnAir = { false };
	_bool	m_bOnWall = { false };
	_uint	m_iJumpState = 0;
	_bool	m_bCellisLand = { true };

	_float	m_CurrentHp = { 1000.f };
	_float	m_MaxHp		= { 1000.f };

	// 위치
	_vector m_MyPos = {0.f, 0.f, 0.f, 1.f};
	_matrix m_MyWorldMat = { };

	LEVEL	m_Current_Level = { LEVEL_END };


protected:
	_bool Set_Gravity(class CTransform* pTargetTransform, _float fTimeDelta);

	HRESULT SetUp_OnCell(class CTransform* pTargetTransform);

public:
	_vector*		Get_MyPos()			{ return &m_MyPos; }
	_matrix*		Get_MyPWorldMat()	{ return &m_MyWorldMat; }

		
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
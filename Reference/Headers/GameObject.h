#pragma once
#include "Transform.h"


BEGIN(Engine)

class CCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{

public:
	struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_float	fGameObjectScale = 0.f;
	};

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const wstring& strComTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void	Collider_Event_Enter(const wstring& strColliderLayerTag, _Out_ CCollider* pMyCollider, _Out_ CCollider* pTargetCollider) {};
	virtual void	Collider_Event_Stay(const wstring& strColliderLayerTag, _Out_ CCollider* pMyCollider, _Out_ CCollider* pTargetCollider) {};
	virtual void	Collider_Event_Exit(const wstring& strColliderLayerTag, _Out_ CCollider* pMyCollider, _Out_ CCollider* pTargetCollider) {};

	void			Compute_Z();

	CTransform*		Get_TranformCom() { return m_pTransformCom; }
	_bool			Get_isDead()	{ return m_bDead; }
	_bool			Get_DeadCheck() { return m_bDeadCheck; }



protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };
	class CTransform*		m_pTransformCom = { nullptr };

	_bool					m_bDead			= { false };
	_bool					m_bDeadCheck	= { false };


protected:
	map<const wstring, class CComponent*>		m_Components;
	_float	m_fGameObjectScale = 1.f;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
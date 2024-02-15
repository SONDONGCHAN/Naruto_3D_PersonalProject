#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollider_Layer : public CBase
{
private:
	CCollider_Layer();
	virtual ~CCollider_Layer() = default;

public:
	class CCollider* Get_Collider(_uint iIndex);


public:
	HRESULT Initialize();
	HRESULT Add_Collider(class CCollider* pCollider);

private:
	list<class CCollider*>			m_Colliders;
	_uint	m_iColliderNum = { 0 };

public:
	static CCollider_Layer* Create();
	virtual void	Free() override;
};
END
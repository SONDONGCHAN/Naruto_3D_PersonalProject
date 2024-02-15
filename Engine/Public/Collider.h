#pragma once
#include "Component.h"
#include "GameObject.h"

BEGIN(Engine)


class ENGINE_DLL CCollider final : public CComponent
{

public:
	enum TYPE{ TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END};

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void	Tick(_fmatrix WorldMatrix);
#ifdef _DEBUG
	HRESULT Render() override;
#endif  // _DEBUG

public:
	_bool	Intersect(CCollider* pTargetCollider);

	void			Set_Collider_GameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; } // //콜라이더가 속한 게임 오브젝트 지정
	CGameObject*	Get_Collider_GameObject() { return m_pGameObject; }		// 콜라이더가 속한 게임 오브젝트 반환s
	_bool			Find_IsCollider(CCollider* Target_Collider );			// Target_Collider* 를 받아서 보관중인 list 중에 있는지 bool로 반환
	void			Add_IsCollider(CCollider* Target_Collider);				// Target_Collider* 를 받아서 보관중인 list 에다 추가
	void			Delete_IsCollider(CCollider* Target_Collider);			// Target_Collider* 를 받아서 보관중인 list 에서 삭제

	void			Set_Center(_float3 vCenter);
	void			Set_Extents_AABB(_float3 vExtents);
	void			Set_Extents_OBB(_float3 vExtents);
	void			Set_Radius(_float fRadius);
	void			Set_HitType(HIT_TYPE HitType) { m_HitType = HitType;}



	_float3			Get_Center();
	_float3			Get_Extents_AABB();
	_float3			Get_Extents_OBB();
	_float			Get_Radius();
	HIT_TYPE		Get_HitType() { return m_HitType;}


private:
	TYPE				m_eType = { TYPE_END };
	class	CBounding* m_pBounding = { nullptr };
	list<CCollider*>	IsColliders = {};				// 충돌한 상대 콜라이더* 를 보관, 충돌이 되지 않을때 삭제
	HIT_TYPE			m_HitType = { HIT_NORMAL };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };

#endif
	// 해당 콜라이더를 가지고 있는 오브젝트의 충돌 이벤트를 호출 해 주기 위함.
private:
	CGameObject* m_pGameObject = { nullptr };


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CCollider* Clone(void* pArg) override;
	virtual void Free() override;

};

END
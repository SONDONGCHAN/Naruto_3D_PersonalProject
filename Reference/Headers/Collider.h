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

	void			Set_Collider_GameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; } // //�ݶ��̴��� ���� ���� ������Ʈ ����
	CGameObject*	Get_Collider_GameObject() { return m_pGameObject; }		// �ݶ��̴��� ���� ���� ������Ʈ ��ȯs
	_bool			Find_IsCollider(CCollider* Target_Collider );			// Target_Collider* �� �޾Ƽ� �������� list �߿� �ִ��� bool�� ��ȯ
	void			Add_IsCollider(CCollider* Target_Collider);				// Target_Collider* �� �޾Ƽ� �������� list ���� �߰�
	void			Delete_IsCollider(CCollider* Target_Collider);			// Target_Collider* �� �޾Ƽ� �������� list ���� ����

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
	list<CCollider*>	IsColliders = {};				// �浹�� ��� �ݶ��̴�* �� ����, �浹�� ���� ������ ����
	HIT_TYPE			m_HitType = { HIT_NORMAL };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };

#endif
	// �ش� �ݶ��̴��� ������ �ִ� ������Ʈ�� �浹 �̺�Ʈ�� ȣ�� �� �ֱ� ����.
private:
	CGameObject* m_pGameObject = { nullptr };


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CCollider* Clone(void* pArg) override;
	virtual void Free() override;

};

END
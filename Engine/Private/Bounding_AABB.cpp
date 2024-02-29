#include "..\Public\Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

_float3 CBounding_AABB::Get_Extents()
{
	return 	m_pAABB->Extents;
}
_float3 CBounding_AABB::Get_Center()
{
	return 	m_pAABB->Center;
}

void CBounding_AABB::Set_Extents(_float3 vExtents)
{
	m_pOriginalAABB->Extents = vExtents;
}

void CBounding_AABB::Set_Center(_float3 vCenter)
{
	m_pOriginalAABB->Center = vCenter;
}

HRESULT CBounding_AABB::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	AABB_DESC* pDesc = (AABB_DESC*)pArg;

	m_pOriginalAABB = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pAABB			= new BoundingBox(*m_pOriginalAABB);

	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;
	
	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]).m128_f32[0];
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]).m128_f32[0];
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]).m128_f32[0];
	
	m_pOriginalAABB->Transform(*m_pAABB, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pAABB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{
	m_isColl = false;

	void* pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pAABB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pAABB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pAABB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}
		
	return m_isColl;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalAABB);
	Safe_Delete(m_pAABB);
}

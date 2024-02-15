#include "..\Public\Bounding_OBB.h"


CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

_float3 CBounding_OBB::Get_Extents()
{
	return 	m_pOBB->Extents;
}

_float3 CBounding_OBB::Get_Center()
{
	return 	m_pOBB->Center;
}

void CBounding_OBB::Set_Extents(_float3 vExtents)
{
	m_pOriginalOBB->Extents = vExtents;
}

void CBounding_OBB::Set_Center(_float3 vCenter)
{
	m_pOriginalOBB->Center = vCenter;

}


HRESULT CBounding_OBB::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	OBB_DESC* pDesc = (OBB_DESC*)pArg;

	_float4		vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB	= new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB			= new BoundingOrientedBox(*m_pOriginalOBB);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, WorldMatrix);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pOBB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{
	m_isColl = false;

	void* pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pOBB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pOBB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_isColl;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();
	Safe_Delete(m_pOriginalOBB);
	Safe_Delete(m_pOBB);
}

#include "Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pBounding(rhs.m_pBounding)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
	Safe_AddRef(m_pBounding);

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pArg);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pArg);
		break;
	}

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	m_pBounding->Tick(WorldMatrix);
}

HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}

_bool CCollider::Intersect(CCollider* pTargetCollider)
{
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

_bool CCollider::Find_IsCollider(CCollider* Target_Collider)
{
	auto iter = find(IsColliders.begin(), IsColliders.end(), Target_Collider);

	if (iter == IsColliders.end())
		return false;
	else
		return true;
}

void CCollider::Add_IsCollider(CCollider* Target_Collider)
{
	IsColliders.push_back(Target_Collider);
	Safe_AddRef(Target_Collider);
}

void CCollider::Delete_IsCollider(CCollider* Target_Collider)
{
	auto iter = find(IsColliders.begin(), IsColliders.end(), Target_Collider);

	if (iter == IsColliders.end())
		return ;

	else
	{
		Safe_Release(*iter);
		IsColliders.erase(iter);
	}
}

void CCollider::Set_Center(_float3 vCenter)
{
	m_pBounding->Set_Center(vCenter);
}

void CCollider::Set_Extents_AABB(_float3 vExtents)
{
	dynamic_cast<CBounding_AABB*>(m_pBounding)->Set_Extents(vExtents);
}

void CCollider::Set_Extents_OBB(_float3 vExtents)
{
	dynamic_cast<CBounding_OBB*>(m_pBounding)->Set_Extents(vExtents);
}

void CCollider::Set_Radius(_float fRadius)
{
	dynamic_cast<CBounding_Sphere*>(m_pBounding)->Set_Radius(fRadius);
}


_float3 CCollider::Get_Center()
{
	return m_pBounding->Get_Center(); 
}

_float3 CCollider::Get_Extents_AABB()
{
	return dynamic_cast<CBounding_AABB*>(m_pBounding)->Get_Extents();
}

_float3 CCollider::Get_Extents_OBB()
{
	return dynamic_cast<CBounding_OBB*>(m_pBounding)->Get_Extents();
}

_float CCollider::Get_Radius()
{
	return dynamic_cast<CBounding_Sphere*>(m_pBounding)->Get_Radius();
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCollider* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	for (auto& pIsCollider : IsColliders)
		Safe_Release(pIsCollider);

	IsColliders.clear();

	__super::Free();

#ifdef _DEBUG

	if (false == m_bClone)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);
}
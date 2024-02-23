#include "Collider_Layer.h"
#include "Collider.h"

CCollider_Layer::CCollider_Layer()
{
}

CCollider* CCollider_Layer::Get_Collider(_uint iIndex)
{
	if (iIndex >= m_iColliderNum)
		return nullptr;

	auto	iter = m_Colliders.begin();
	
	for (size_t i = 0; i < iIndex; i++)
		++iter;
	
	return *iter;
}

HRESULT CCollider_Layer::Initialize()
{
	return S_OK;
}

HRESULT CCollider_Layer::Add_Collider(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders.push_back(pCollider);

	m_iColliderNum++;

	return S_OK;
}

CCollider_Layer* CCollider_Layer::Create()
{
	CCollider_Layer* pInstance = new CCollider_Layer();
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollider_Layer");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CCollider_Layer::Free()
{
	//for (auto& pCollider : m_Colliders)
	//	Safe_Release(pCollider);

	m_Colliders.clear();
}

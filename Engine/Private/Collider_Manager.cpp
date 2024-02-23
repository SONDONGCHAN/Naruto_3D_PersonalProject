#include "Collider_Manager.h"
#include "Collider_Layer.h"
#include "Collider.h"


CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Initialize(_uint iNumLevels)
{	
	m_iNumLevels = iNumLevels;

	m_pCollider_Layers = new COLLIDER_LAYERS[iNumLevels];

	return S_OK;
}


HRESULT CCollider_Manager::Add_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* _pCollider , void* pArg)
{
	if (nullptr == _pCollider)
		return E_FAIL;

	CCollider_Layer* pCollider_Layer = Find_Collider_Layer(iLevelIndex, strLayerTag);
	
	if (nullptr == pCollider_Layer)
	{
		pCollider_Layer = CCollider_Layer::Create();
	
		if (nullptr == pCollider_Layer)
			return E_FAIL;
	
		pCollider_Layer->Add_Collider(_pCollider);
	
		m_pCollider_Layers[iLevelIndex].emplace(strLayerTag, pCollider_Layer);
	}
	else
	{
		pCollider_Layer->Add_Collider(_pCollider);
	}

	return S_OK;
}

void CCollider_Manager::Check_Collision_For_MyEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag)
{   
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(iLevelIndex, strTargetColliderLayerTag);

	for (_uint i = 0; ; i++)
	{
		CCollider* pTargetCollider = pColliderLayer->Get_Collider(i);

		if (pTargetCollider == nullptr)
			break;

		if (pTargetCollider->Get_Collider_GameObject()->Get_DeadCheck())
			continue;
		
		if (MyColliderCom->Intersect(pTargetCollider))
		{
			if (MyColliderCom->Find_IsCollider(pTargetCollider))
				MyColliderCom->Get_Collider_GameObject()->Collider_Event_Stay(strTargetColliderLayerTag, MyColliderCom,pTargetCollider);
			
			else
			{
				MyColliderCom->Get_Collider_GameObject()->Collider_Event_Enter(strTargetColliderLayerTag, MyColliderCom, pTargetCollider);
				MyColliderCom->Add_IsCollider(pTargetCollider);
			}
		}
		else
		{
			if (MyColliderCom->Find_IsCollider(pTargetCollider))
			{
				MyColliderCom->Get_Collider_GameObject()->Collider_Event_Exit(strTargetColliderLayerTag, MyColliderCom, pTargetCollider);
				MyColliderCom->Delete_IsCollider(pTargetCollider);
			}
		}
	}

	return;
}

void CCollider_Manager::Check_Collision_For_TargetEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag)
{
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(iLevelIndex, strTargetColliderLayerTag);
	
	for (_uint i = 0; ; i++)
	{
		CCollider* pTargetCollider = pColliderLayer->Get_Collider(i);
	
		if (pTargetCollider == nullptr)
			break;

		if (pTargetCollider->Get_Collider_GameObject()->Get_DeadCheck())
			continue;
		
		if (MyColliderCom->Intersect(pTargetCollider))
		{
			if (pTargetCollider->Find_IsCollider(MyColliderCom))
				pTargetCollider->Get_Collider_GameObject()->Collider_Event_Stay(strMyColliderLayerTag, pTargetCollider, MyColliderCom);
				
			else
			{
				pTargetCollider->Get_Collider_GameObject()->Collider_Event_Enter(strMyColliderLayerTag, pTargetCollider, MyColliderCom);
				pTargetCollider->Add_IsCollider(MyColliderCom);
			}
		}
		else
		{
			if (pTargetCollider->Find_IsCollider(MyColliderCom))
			{
				pTargetCollider->Get_Collider_GameObject()->Collider_Event_Exit(strMyColliderLayerTag, pTargetCollider, MyColliderCom);
				pTargetCollider->Delete_IsCollider(MyColliderCom);
			}
		}
	}

	return;
}

_bool CCollider_Manager::Is_Collision(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strColliderLayerTag)
{	
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(iLevelIndex,strColliderLayerTag);
	
	for (_uint i = 0; ; i++)
	{
		CCollider* pTargetCollider = pColliderLayer->Get_Collider(i);
	
		if (pTargetCollider == nullptr)
			return false;
	
		if (MyColliderCom->Intersect(pTargetCollider))
			return true;

		else
			continue;
	}

	return false;
}

void CCollider_Manager::Kill_Dead_Collider(_uint iLevelIndex, CCollider* DeadCollider)
{
	for (auto& Pair : m_pCollider_Layers[iLevelIndex])
	{
		list<class CCollider*>* pColliders = Pair.second->Get_Colliders();

		for (auto pCollier : *pColliders)
		{
			if (pCollier->Find_IsCollider(DeadCollider))
				pCollier->Delete_IsCollider(DeadCollider);
		}

	}
}

void CCollider_Manager::Collider_Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pCollider_Layers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pCollider_Layers[iLevelIndex].clear();
}

CCollider_Layer* CCollider_Manager::Find_Collider_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{	
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pCollider_Layers[iLevelIndex].find(strLayerTag);
	if (iter == m_pCollider_Layers[iLevelIndex].end())
		return nullptr;
	
	return iter->second;
}

CCollider_Manager* CCollider_Manager::Create(_uint iNumLevels)
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CCollider_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pCollider_Layers[i])
			Safe_Release(Pair.second);

		m_pCollider_Layers[i].clear();
	}
	Safe_Delete_Array(m_pCollider_Layers);

}

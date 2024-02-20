#include "Collider_Manager.h"
#include "Collider_Layer.h"
#include "Collider.h"


CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Initialize()
{	
	return S_OK;
}


HRESULT CCollider_Manager::Add_Collider(const wstring& strLayerTag, CCollider* _pCollider , void* pArg)
{
	if (nullptr == _pCollider)
		return E_FAIL;

	CCollider_Layer* pCollider_Layer = Find_Collider_Layer(strLayerTag);
	
	if (nullptr == pCollider_Layer)
	{
		pCollider_Layer = CCollider_Layer::Create();
	
		if (nullptr == pCollider_Layer)
			return E_FAIL;
	
		pCollider_Layer->Add_Collider(_pCollider);
	
		m_pCollider_Layer.emplace(strLayerTag, pCollider_Layer);
	}
	else
	{
		pCollider_Layer->Add_Collider(_pCollider);
	}

	return S_OK;
}

void CCollider_Manager::Check_Collision_For_MyEvent(CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag)
{   
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(strTargetColliderLayerTag);

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

void CCollider_Manager::Check_Collision_For_TargetEvent(CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag)
{
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(strTargetColliderLayerTag);
	
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

_bool CCollider_Manager::Is_Collision(CCollider* MyColliderCom, const wstring& strColliderLayerTag)
{	
	CCollider_Layer* pColliderLayer = Find_Collider_Layer(strColliderLayerTag);
	
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

void CCollider_Manager::Kill_Dead_Collider(CCollider* DeadCollider)
{
	for (auto& Pair : m_pCollider_Layer)
	{
		list<class CCollider*>* pColliders = Pair.second->Get_Colliders();

		for (auto pCollier : *pColliders)
		{
			if (pCollier->Find_IsCollider(DeadCollider))
				pCollier->Delete_IsCollider(DeadCollider);
		}

	}
}

CCollider_Layer* CCollider_Manager::Find_Collider_Layer(const wstring& strLayerTag)
{	
	auto	iter = m_pCollider_Layer.find(strLayerTag);
	if (iter == m_pCollider_Layer.end())
		return nullptr;
	
	return iter->second;
}

CCollider_Manager* CCollider_Manager::Create()
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollider_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	for (auto& Pair : m_pCollider_Layer)
		Safe_Release(Pair.second);

	m_pCollider_Layer.clear();

}

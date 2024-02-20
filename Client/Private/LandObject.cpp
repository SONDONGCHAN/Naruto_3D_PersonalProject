#include "stdafx.h"
#include "LandObject.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject& rhs)
    : CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

		if (pGameObjectDesc->fSpeedPerSec != 5.f)
		{
			m_pMapTransform = pGameObjectDesc->pMapTransform;
			m_pMapModel = pGameObjectDesc->pMapModel;
			m_pCamera = pGameObjectDesc->pCamera;

			Safe_AddRef(m_pMapTransform);
			Safe_AddRef(m_pMapModel);
			Safe_AddRef(m_pCamera);

			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Calculator"),
				TEXT("Com_Calculator"), reinterpret_cast<CComponent**>(&m_pCalculator))))
				return E_FAIL;
		}
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(_float fTimeDelta)
{
	 
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
	m_MyPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_MyWorldMat	= m_pTransformCom->Get_WorldMatrix();
}

HRESULT CLandObject::Render()
{
    return S_OK;
}

_float CLandObject::Lerp(_float start, _float end, _float ratio)
{
	return start + ratio * (end - start);
}

_bool CLandObject::Set_Gravity(CTransform* pTargetTransform, _float fTimeDelta)
{
	if (m_bOnAir)
	{
		if(m_fMaxGravity > m_fGravity)
			m_fGravity += fTimeDelta * m_fGForcePersec;

		_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION); // Ÿ���� ��ġ�� �����´�.

		vTargetPos.m128_f32[1] -= m_fGravity;  // ���� �߷��� �����Ͽ� Ÿ���� Y��ǥ ��ġ�� �����Ѵ�.

		_float fCellHeight = m_pNavigationCom->Compute_Height(vTargetPos); // ���� ���� ���̸� ��� 

		if (fCellHeight >= vTargetPos.m128_f32[1])
		{
			vTargetPos = XMVectorSetY(vTargetPos, fCellHeight);
			//m_bOnAir = false;
			m_fGravity = 0.f;
			m_iJumpState = 0;

			pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.
			return false;
		}
		pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.
		return true;

	}
	else
	{
		SetUp_OnCell(pTargetTransform);
		return false;
	}
}

_bool CLandObject::Set_Gravity_2(CTransform* pTargetTransform, _float fTimeDelta)
{
	 if (m_bOnAir)
	 {
	 	if (m_fMaxGravity > m_fGravity)
	 		m_fGravity += fTimeDelta * m_fGForcePersec;
	 
	 	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION); // Ÿ���� ��ġ�� �����´�.
	 	vTargetPos.m128_f32[1] -= m_fGravity;  // ���� �߷��� �����Ͽ� Ÿ���� Y��ǥ ��ġ�� �����Ѵ�.

		_vector		RayDir = {0.f, -1.f, 0.f, 0.f};
		_matrix Map_PivotMat = XMMatrixIdentity();
		_float3 vCalPos = m_pCalculator->Ray_Picking_OnModel(m_pMapTransform, m_pMapModel, Map_PivotMat, vTargetPos, RayDir);
		_float fCellHeight = vCalPos.y;

	 	if (fCellHeight >= vTargetPos.m128_f32[1])
	 	{
	 		vTargetPos = XMVectorSetY(vTargetPos, fCellHeight);
	 		//m_bOnAir = false;
	 		m_fGravity = 0.f;
	 		m_iJumpState = 0;
	 
	 		pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.
	 		return false;
	 	}
	 	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.
	 	return true;
	 
	 }
	 else
	 {
		 SetUp_OnCell_2(pTargetTransform);
	 	return false;
	 }
}

HRESULT CLandObject::SetUp_OnCell(CTransform* pTargetTransform)
{
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);// Ÿ���� ��ġ�� �����´�.

	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix_Inverse()); // ���� WorldMat�� �����´�

	vTargetPos = XMVectorSetY(vTargetPos, m_pNavigationCom->Compute_Height(vTargetPos)); //Ÿ���� Y��ġ�� ���� ���� ���̿� �����ش�.

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.

	return S_OK;
}

HRESULT CLandObject::SetUp_OnCell_2(CTransform* pTargetTransform)
{
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);// Ÿ���� ��ġ�� �����´�.
	
	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix_Inverse()); // ���� WorldMat�� �����´�

	_vector		RayDir = { 0.f, -1.f, 0.f, 0.f };
	_matrix Map_PivotMat = XMMatrixIdentity();
	_float3 vCalPos = m_pCalculator->Ray_Picking_OnModel(m_pMapTransform, m_pMapModel, Map_PivotMat, vTargetPos, RayDir);
	_float fCellHeight = vCalPos.y;
	
	vTargetPos = XMVectorSetY(vTargetPos, fCellHeight); //Ÿ���� Y��ġ�� ���� ���� ���̿� �����ش�.
	
	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat�� �����Ѵ�.
	
	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pMapTransform);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMapModel);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pCamera);
}

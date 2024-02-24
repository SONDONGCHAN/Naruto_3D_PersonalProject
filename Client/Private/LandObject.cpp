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

		if (pGameObjectDesc->fSpeedPerSec != 1.2345f)
		{
			m_pMapTransform = pGameObjectDesc->pMapTransform;
			m_pCamera = pGameObjectDesc->pCamera;
			m_Current_Level = pGameObjectDesc->Current_Level;

			Safe_AddRef(m_pMapTransform);
			Safe_AddRef(m_pCamera);
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
	
		_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION); // 타겟의 위치를 가져온다.

		vTargetPos.m128_f32[1] -= m_fGravity;  // 현재 중력을 적용하여 타겟의 Y좌표 위치를 조정한다.
	
		
		_float fCellHeight = m_pNavigationCom->Compute_Height(vTargetPos); // 현재 셀의 높이를 계산 

		if (fCellHeight >= vTargetPos.m128_f32[1])
		{
			//m_pTransformCom->Go_Straight_Custom(fTimeDelta,0.000000001f,m_pNavigationCom, true, &m_bCellisLand);

			if (m_bCellisLand)
			{
				m_fGravity = 0.f;
				m_iJumpState = 0;
				vTargetPos = XMVectorSetY(vTargetPos, fCellHeight);
				//m_bOnAir = false;				
			}
			else
			{
				_float fCellDepth = m_pNavigationCom->Compute_Depth(vTargetPos); // 현재 Z를 계산 
				vTargetPos = XMVectorSetZ(vTargetPos, fCellDepth);

				_float fCellWidth = m_pNavigationCom->Compute_Width(vTargetPos); // 현재 X를 계산 
				vTargetPos = XMVectorSetX(vTargetPos, fCellWidth);

				fCellDepth = m_pNavigationCom->Compute_Depth(vTargetPos); // 현재 Z를 계산 
				vTargetPos = XMVectorSetZ(vTargetPos, fCellDepth);
				
				fCellWidth = m_pNavigationCom->Compute_Width(vTargetPos); // 현재 X를 계산 
				vTargetPos = XMVectorSetX(vTargetPos, fCellWidth);

				m_fGravity = 0.f;
				m_iJumpState = 1;
			}

			pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat에 적용한다.
			return false;
			
		}

		pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat에 적용한다.
		return true;
	
	}
	else
	{
		SetUp_OnCell(pTargetTransform);
		return false;
	}
}

HRESULT CLandObject::SetUp_OnCell(CTransform* pTargetTransform)
{
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);// 타겟의 위치를 가져온다.

	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix_Inverse()); // 맵의 WorldMat을 가져온다

	vTargetPos = XMVectorSetY(vTargetPos, m_pNavigationCom->Compute_Height(vTargetPos)); //타겟의 Y위치를 맵의 셀의 높이에 맞춰준다.

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pMapTransform->Get_WorldMatrix())); //WorldMat에 적용한다.

	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pMapTransform);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pCamera);
}

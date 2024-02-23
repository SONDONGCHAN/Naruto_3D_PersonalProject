#include "..\Public\Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

void CTransform::Set_Scaling(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pTransformDesc = (TRANSFORM_DESC*)pArg;

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

void CTransform::Go_Straight(_float fTimeDelta,CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else 
	{
 		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos += ResultDir * m_fSpeedPerSec * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);		
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Straight_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;
	
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else 
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos += ResultDir * fSpeed * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else 
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos -= ResultDir * m_fSpeedPerSec * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);		
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Backward_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vLook) * fSpeed * fTimeDelta;
	
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos -= ResultDir * fSpeed * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos -= ResultDir * m_fSpeedPerSec * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);		
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Left_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos -= ResultDir * fSpeed * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos += ResultDir * m_fSpeedPerSec * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);		
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Right_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Go_Custom_Direction(_float fTimeDelta, _float fSpeed, _vector vDirection, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;
	
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDirection, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vDirection, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_vector		vStateDir = Get_State(STATE(i));

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		vScaled = Get_Scaled();

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_float4		vTmp = _float4(0.f, 0.f, 0.f, 0.f);

		*((_float*)&vTmp + i) = 1.f * *((_float*)&vScaled + i);

		_vector		vStateDir = XMLoadFloat4(&vTmp);

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::MoveTo(_fvector vPoint, _float fSpeed, _float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint - vPosition;
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vDir) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else 
	{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::LookAt(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::LookAt_ForLandObject(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Pos(_fvector vPoint)
{
	Set_State(STATE_POSITION, vPoint);
}

void CTransform::Set_LandObject_Pos(_fvector vPoint, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint - vPosition;
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += vDir;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir;

			if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

void CTransform::Set_Look(_fvector direction)
{
	_float3	vScaled = Get_Scaled();
	_vector	vLook = direction;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Look_IncludeY(_fvector direction)
{
	_vector	vLook = XMVector3Normalize(direction);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize (XMVector3Cross(vLook, vRight));
	
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight));
	Set_State(STATE_UP, XMVector3Normalize(vUp));
	Set_State(STATE_LOOK, XMVector3Normalize(vLook));
}

void CTransform::SetAnimationMove(_fvector vPoint, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint;
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += vDir;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos += ResultDir;

		if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);	
	}
	if (_isLand != nullptr)
		*_isLand = isLand;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();

}


/*

#include "..\Public\Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

void CTransform::Set_Scaling(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pTransformDesc = (TRANSFORM_DESC*)pArg;

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

void CTransform::Go_Straight(_float fTimeDelta,CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
	{
		Set_State(STATE_POSITION, vPosition);
		if ((!isLand))
		{
			*_isLand = false;
		}
	}
	else 
{
	if ((!isLand) && onAir)
	{
		Set_State(STATE_POSITION, vPosition);
		*_isLand = false;
	}
	else
	{
		_vector		vSlidingPos = Get_State(STATE_POSITION);
		vSlidingPos += ResultDir * m_fSpeedPerSec * fTimeDelta;

		if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
			Set_State(STATE_POSITION, vSlidingPos);
	}
}
}

void CTransform::Go_Straight_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos -= ResultDir * m_fSpeedPerSec * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Backward_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vLook, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos -= ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vLook, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos -= ResultDir * m_fSpeedPerSec * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Left_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition -= XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos -= ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * m_fSpeedPerSec * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Right_Custom(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vRight, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vRight, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Go_Custom_Direction(_float fTimeDelta, _float fSpeed, _vector vDirection, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDirection, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vDirection, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_vector		vStateDir = Get_State(STATE(i));

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		vScaled = Get_Scaled();

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_float4		vTmp = _float4(0.f, 0.f, 0.f, 0.f);

		*((_float*)&vTmp + i) = 1.f * *((_float*)&vScaled + i);

		_vector		vStateDir = XMLoadFloat4(&vTmp);

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::MoveTo(_fvector vPoint, _float fSpeed, _float fTimeDelta, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint - vPosition;
	_vector		ResultDir;
	_bool		isLand = true;

	//_float		fDistance = XMVector3Length(vDir).m128_f32[0];

	//if (fDistance >= fLimit)
	vPosition += XMVector3Normalize(vDir) * fSpeed * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else 
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir * fSpeed * fTimeDelta;

			if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::LookAt(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::LookAt_ForLandObject(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Pos(_fvector vPoint)
{
	Set_State(STATE_POSITION, vPoint);
}

void CTransform::Set_LandObject_Pos(_fvector vPoint, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint - vPosition;
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += vDir;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir;

			if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}
}

void CTransform::Set_Look(_fvector direction)
{
	_float3	vScaled = Get_Scaled();
	_vector	vLook = direction;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Look_IncludeY(_fvector direction)
{
	_vector	vLook = XMVector3Normalize(direction);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight));
	Set_State(STATE_UP, XMVector3Normalize(vUp));
	Set_State(STATE_LOOK, XMVector3Normalize(vLook));
}

void CTransform::SetAnimationMove(_fvector vPoint, CNavigation* pNavigation, _bool onAir, _bool* _isLand)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint;
	_vector		ResultDir;
	_bool		isLand = true;

	vPosition += vDir;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, vDir, &ResultDir, &isLand, onAir))
		Set_State(STATE_POSITION, vPosition);

	else
	{
		if ((!isLand) && onAir)
		{
			Set_State(STATE_POSITION, vPosition);
			_isLand = false;
		}
		else
		{
			_vector		vSlidingPos = Get_State(STATE_POSITION);
			vSlidingPos += ResultDir;

			if (true == pNavigation->isMove(vSlidingPos, vDir, &ResultDir, &isLand, onAir))
				Set_State(STATE_POSITION, vSlidingPos);
		}
	}

}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();

}


*/
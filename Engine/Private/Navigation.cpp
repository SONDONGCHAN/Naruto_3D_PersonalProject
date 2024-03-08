#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"

#include "GameInstance.h"

_float4x4	CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
	, m_vecCells(rhs.m_vecCells)
{
	for (auto& pCell : m_vecCells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG

}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFilePath)
{ 
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	

	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strNavigationDataFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END];

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;


		_vector AB = XMLoadFloat3(&vPoints[1]) - XMLoadFloat3(&vPoints[0]);
		_vector AC = XMLoadFloat3(&vPoints[2]) - XMLoadFloat3(&vPoints[0]);

		AB.m128_f32[1] = 0.f;
		AB.m128_f32[3] = 0.f;
		AC.m128_f32[1] = 0.f;
		AC.m128_f32[3] = 0.f;

		if(XMVector3Cross(AB, AC).m128_f32[1] < 0)
		{
			_float3 point = vPoints[1];
			vPoints[1] = vPoints[2];
			vPoints[2] = point;
		}

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_vecCells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_vecCells.push_back(pCell);
	}


	CloseHandle(hFile);

	if (FAILED(Make_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentIndex = ((NAVI_DESC*)pArg)->iStartCellIndex;
	}

	return S_OK;
}

void CNavigation::Tick(_fmatrix MapWorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, MapWorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition, _vector Dir, _Out_ _vector* ResultDir, _Out_ _bool* isLand, _bool onAir)
{
	_int		iNeighborIndex = { -1 };
	_float3		vLineNormal = { 0.f, 0.f, 0.f };

	/* 현재 존재하고 있던 셀 안에서 움직였다. */
	if (true == m_vecCells[m_iCurrentIndex]->isIn(vPosition, &iNeighborIndex, &vLineNormal))
	{
		if(!m_vecCells[m_iCurrentIndex]->Get_isLand())
			*isLand = false;

		*ResultDir = Dir;
		return true;
	}
	/* 현재 셀을 나갔다. */
	else
	{

		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;
		
				if (true == m_vecCells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, &vLineNormal))
				{
					if (m_vecCells[iNeighborIndex]->Get_isLand())
					{
						if (m_vecCells[m_iCurrentIndex]->Get_isLand() == false)
						{
							if (vPosition.m128_f32[1] < m_vecCells[iNeighborIndex]->Compute_Height(vPosition))
							{
								*ResultDir = Dir;
								*isLand = false;
								return false;
							}
						}
						m_iCurrentIndex = iNeighborIndex;
						*ResultDir = Dir;
		
						return true;
		
					}
					else
					{
						if (onAir == true)
						{
							m_iCurrentIndex = iNeighborIndex;
							*ResultDir = Dir;
							*isLand = false;
							return true;
						}
						else
						{


							_vector vCounterNormal = XMVector3Cross(XMVector3Cross(XMLoadFloat3(&vLineNormal), Dir), XMLoadFloat3(&vLineNormal));
							vCounterNormal = XMVector3Normalize(vCounterNormal);
							_float fScale = XMVectorGetX(XMVector3Dot(vCounterNormal, Dir));
							*ResultDir = vCounterNormal * fScale;
							return false;
						}
					}
				}
			}
		}
		/*if (-1 != iNeighborIndex)
{
	while (true)
	{
		if (-1 == iNeighborIndex)
			return false;

		if (true == m_vecCells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, &vLineNormal))
		{
			if (m_vecCells[iNeighborIndex]->Get_isLand())
			{
				m_iCurrentIndex = iNeighborIndex;
				*ResultDir = Dir;

				return true;

			}
			else
			{
				if(onAir == true)
					m_iCurrentIndex = iNeighborIndex;

				_vector vCounterNormal = XMVector3Cross(XMVector3Cross(XMLoadFloat3(&vLineNormal), Dir), XMLoadFloat3(&vLineNormal));
				vCounterNormal = XMVector3Normalize(vCounterNormal);

				_float fScale = XMVectorGetX(XMVector3Dot(vCounterNormal, Dir));
				*ResultDir = vCounterNormal * fScale;
				*isLand = false;
				return false;
			}
		}
	}
}*/

		else
		{
			_vector vCounterNormal = XMVector3Cross(XMVector3Cross(XMLoadFloat3(&vLineNormal), Dir), XMLoadFloat3(&vLineNormal));
			vCounterNormal = XMVector3Normalize(vCounterNormal);

			_float fScale = XMVectorGetX(XMVector3Dot(vCounterNormal, Dir));
			*ResultDir = vCounterNormal * fScale;
			return false;
		}
	}
}

_float CNavigation::Compute_Height(_fvector vLocalPos)
{
	return m_vecCells[m_iCurrentIndex]->Compute_Height(vLocalPos);
}

_float CNavigation::Compute_Width(_fvector vLocalPos)
{
	return m_vecCells[m_iCurrentIndex]->Compute_Width(vLocalPos);
}

_float CNavigation::Compute_Depth(_fvector vLocalPos)
{
	return m_vecCells[m_iCurrentIndex]->Compute_Depth(vLocalPos);
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ViewMatrix_Float())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ProjMatrix_Float())))
		return E_FAIL;

	_float4		vColor = _float4(0.f, 0.f, 0.f, 1.f);
	_float4x4	WorldMatrix = m_WorldMatrix;

	if (-1 == m_iCurrentIndex)
	{
		WorldMatrix._42 += 0.1f;
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
	}
	else
	{
		WorldMatrix._42 += 0.15f;
		vColor = _float4(1.f, 0.f, 0.f, 1.f);
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Begin(0);

	if (-1 == m_iCurrentIndex)
	{
		for (auto& pCell : m_vecCells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
		m_vecCells[m_iCurrentIndex]->Render();

	return S_OK;
}

#endif

HRESULT CNavigation::Make_Neighbors()
{
	for (auto& pSourCell : m_vecCells)
	{
		for (auto& pDestCell : m_vecCells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}
	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_vecCells)
		Safe_Release(pCell);

	m_vecCells.clear();



#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif


}

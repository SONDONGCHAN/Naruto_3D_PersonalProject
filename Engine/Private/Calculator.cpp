#include "Calculator.h"
#include "GameInstance.h"

CCalculator::CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CCalculator::CCalculator(const CCalculator& rhs)
    : CComponent(rhs)
{
}

HRESULT CCalculator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCalculator::Initialize(void* pArg)
{
    return S_OK;
}

_float3 CCalculator::Ray_Picking_OnModel(CTransform* pTargetTransformCom, CModel* m_pModelCom, _fmatrix PivotMatrix, _vector MyPos, _vector dir)
{
	_vector		vecRayPos = MyPos;
	_vector		vecRayDir = dir;

	_matrix	matWorld_Inverse = pTargetTransformCom->Get_WorldMatrix_Inverse();

	vecRayPos = XMVector3TransformCoord(vecRayPos, matWorld_Inverse);
	vecRayDir = XMVector3TransformNormal(vecRayDir, matWorld_Inverse);
	vecRayDir = XMVector3Normalize(vecRayDir);

	vector<class CMesh*> vecMeshes = m_pModelCom->Get_vecMeshes();
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float fFinalDistance = { 0.f };

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_ulong		iNumFace = (vecMeshes[i]->Get_NumIndices()) / 3;
		_ulong		dwVtxIdx[3]{};
		_float		fDistance(0.f);

		_uint NumIndices = vecMeshes[i]->Get_NumIndices();

		//_uint* pIndicesIndex = new _uint[NumIndices];
		vector<_uint>*	m_pVecIndices = vecMeshes[i]->Get_pVecIndices();
		//pIndicesIndex = vecMeshes[i]->Get_IndicesIndex();

		_float3* pVtxPos = vecMeshes[i]->Get_VtxPos();

		//VTXMESH* mesh = vecMeshes[i]->Get_Vertices();s
		//_float3* pVtxPos = &(mesh->vPosition);

		for (_ulong j = 0; j < iNumFace; j++)
		{
			// 오른쪽 위
			dwVtxIdx[0] = (*m_pVecIndices)[j * 3];
			dwVtxIdx[1] = (*m_pVecIndices)[j * 3 + 1];
			dwVtxIdx[2] = (*m_pVecIndices)[j * 3 + 2];

			if (DirectX::TriangleTests::Intersects(vecRayPos, vecRayDir,
				XMVector3TransformCoord(XMLoadFloat3(&pVtxPos[dwVtxIdx[0]]), PivotMatrix),
				XMVector3TransformCoord(XMLoadFloat3(&pVtxPos[dwVtxIdx[1]]), PivotMatrix),
				XMVector3TransformCoord(XMLoadFloat3(&pVtxPos[dwVtxIdx[2]]), PivotMatrix),
				fDistance))
			{
				if (fFinalDistance == 0.f)
					fFinalDistance = fDistance;

				else if ((fFinalDistance != 0.f) && (fFinalDistance > fDistance))
					fFinalDistance = fDistance;
			}
		}

		Safe_Delete(m_pVecIndices);

	}

	if (fFinalDistance == 0.f)
		return m_vLastPos;

	else
	{
		_float3 fPos = {};
		XMStoreFloat3(&fPos, (vecRayPos + (vecRayDir * fFinalDistance)));
		m_vLastPos = fPos;
		return fPos;
	}
}

CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCalculator* pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCalculator::Clone(void* pArg)
{
	CCalculator* pInstance = new CCalculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
	__super::Free();
}

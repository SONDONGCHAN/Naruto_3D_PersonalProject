#include "Mesh.h"
#include "Bone.h"


CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_iNumBones(rhs.m_iNumBones)
	, m_vecBoneIndices(rhs.m_vecBoneIndices)
	, m_vecOffsetMatrices(rhs.m_vecOffsetMatrices)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, CModel* pModel, _fmatrix PivotMatrix)
{
	m_iNumVertexBuffers = 1;

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT		hr;

	if (eType == CModel::TYPE_NONANIM)
		hr = Ready_Vertex_Buffer_NonAnim(PivotMatrix);
	else if (eType == CModel::TYPE_ANIM)
		hr = Ready_Vertex_Buffer_Anim(pModel);


	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_Vertex_Buffer_NonAnim( _fmatrix PivotMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	
	m_InitialData.pSysMem = m_pVertices;
	
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_Vertex_Buffer_Anim( CModel* pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	

	m_InitialData.pSysMem = m_pAnimVertices;
	
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	
	return S_OK;
}


void CMesh::SetUp_BoneMatrices(_Out_ _float4x4* pBoneMatrices, vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pBoneMatrices[i], XMLoadFloat4x4(&m_vecOffsetMatrices[i]) * XMLoadFloat4x4(&Bones[m_vecBoneIndices[i]]->Get_CombindTransformationMatrix()));
	}
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, CModel* pModel, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	//if (FAILED(pInstance->Initialize_Prototype(eType, pModel, PivotMatrix)))
	//{
	//	MSG_BOX("Failed to Created : CMesh");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pAnimVertices);

		Safe_Delete_Array(m_pIndices);
	}
	__super::Free();
}

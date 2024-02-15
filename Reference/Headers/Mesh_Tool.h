#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh_Tool final : public CVIBuffer
{
	CMesh_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Tool(const CMesh_Tool& rhs);
	virtual ~CMesh_Tool() = default;

public:
	HRESULT Initialize_Prototype(CModel::TYPE eType, const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Ready_Vertex_Buffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertex_Buffer_Anim(const aiMesh* pAIMesh);

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }

private:
	/* 메시별로 이용하고 있는 뼈들. */
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	_uint				m_iMaterialIndex = { 0 };

	_float3*			m_pVertices_Pos;


public:
	static CMesh_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const aiMesh* pAIMesh, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
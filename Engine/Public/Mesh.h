#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT Initialize_Prototype(CModel::TYPE eType,CModel* pModel, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Ready_Vertex_Buffer_NonAnim(_fmatrix PivotMatrix);
	HRESULT Ready_Vertex_Buffer_Anim(CModel* pModel);

public:
	void SetUp_BoneMatrices(_Out_ _float4x4* pBoneMatrices, vector<class CBone*>& Bones);

public:
	void	Set_MaterialIndex(_uint index) {  m_iMaterialIndex = index;}
	void	Set_NumVertices(_uint NumVertices) 
	{ 
		m_iNumVertices = NumVertices; 
		m_pVertices = new VTXMESH[m_iNumVertices];
		m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	}
	void	Set_Vertices(VTXMESH Vertices, _int VertexNum) 
	{
		m_pVertices[VertexNum].vPosition	= Vertices.vPosition;
		m_pVertices[VertexNum].vNormal		= Vertices.vNormal;
		m_pVertices[VertexNum].vTexcoord	= Vertices.vTexcoord;
		m_pVertices[VertexNum].vTangent		= Vertices.vTangent;
	}
	void	Set_AnimVertices(VTXANIMMESH Vertices, _int VertexNum)
	{
		m_pAnimVertices[VertexNum].vPosition = Vertices.vPosition;
		m_pAnimVertices[VertexNum].vNormal = Vertices.vNormal;
		m_pAnimVertices[VertexNum].vTexcoord = Vertices.vTexcoord;
		m_pAnimVertices[VertexNum].vTangent = Vertices.vTangent;
		m_pAnimVertices[VertexNum].vBlendIndices = Vertices.vBlendIndices;
		m_pAnimVertices[VertexNum].vBlendWeights = Vertices.vBlendWeights;
	}
	void	Set_NumIndices(_uint NumIndices) 
	{ 
		m_iNumIndices = NumIndices; 
		m_pIndices = new _uint[m_iNumIndices];
	}
	void	Set_Indices(_uint Indices, _int IndexNum) { 
		m_pIndices[IndexNum] = Indices ;
		m_vecIndices.push_back(Indices);
	}
	void	Set_NumBones(_uint NumBones) { m_iNumBones = NumBones; }
	void	Set_BoneIndices(vector<_uint> vecBoneIndices){ m_vecBoneIndices = vecBoneIndices;}
	void	Set_OffsetMatrices(vector<_float4x4> vecOffsetMatrices) { m_vecOffsetMatrices = vecOffsetMatrices; }
	void	Set_MeshName(_char* szName) { strcpy_s(m_szName, szName); }


	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }


	_uint			Get_NumIndices() const { return m_iNumIndices; }
	_uint*			Get_IndicesIndex() { return m_pIndices; }
	vector<_uint>*	Get_pVecIndices() { return &m_vecIndices; }


	_float3*	Get_VtxPos() const { return  m_pVertices_Pos; }
	//VTXMESH*	Get_Vertices() const{ return m_pVertices; }

private:
	void Set_Vertices_Pos();


private:
	/* 메시별로 이용하고 있는 뼈들. */
	_uint				m_iNumBones = { 0 };	//O
	vector<_uint>		m_vecBoneIndices;		//O		
	vector<_float4x4>	m_vecOffsetMatrices;	//O

	_uint				m_iMaterialIndex = { 0 };	//O
	_char				m_szName[MAX_PATH] = "";	//O

	VTXMESH*			m_pVertices;				//O
	VTXANIMMESH*		m_pAnimVertices;			//O

	_uint*				m_pIndices;					//O
	vector<_uint>		m_vecIndices;
	_uint**				m_ppIndices;				//O

	_float3*			m_pVertices_Pos;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType , CModel* pModel, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
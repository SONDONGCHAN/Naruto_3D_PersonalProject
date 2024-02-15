#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain_Tool final : public CVIBuffer
{
private:
	CVIBuffer_Terrain_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain_Tool(const CVIBuffer_Terrain_Tool& rhs);
	virtual ~CVIBuffer_Terrain_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	const	_float3* Get_VtxPos() const { return  m_pVertices_Pos; }
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }
	
	
private:
	_uint			m_iNumVerticesX = { 0 };
	_uint			m_iNumVerticesZ = { 0 };
	_float3*		m_pVertices_Pos;

public:
	static CVIBuffer_Terrain_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	struct AABB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	};
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public : 
	virtual void*	Get_Bounding() override { return m_pAABB; }

	_float3				Get_Extents();
	virtual _float3		Get_Center() override;

	void				Set_Extents(_float3 vExtents);
	void				Set_Center(_float3 vCenter) override;

public:
	HRESULT			Initialize(void* pArg);
	virtual void	Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool	Intersect(CCollider::TYPE eType, CBounding* pBounding) override;

private:
	BoundingBox* m_pOriginalAABB	= { nullptr }; // 소유자의 월드Mat을 적용하기전 상태를 저장해둠
	BoundingBox* m_pAABB			= { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END

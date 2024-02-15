#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	struct SPHERE_DESC : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	};
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual void* Get_Bounding() override { return m_pSphere; }

	_float				Get_Radius();
	virtual _float3		Get_Center() override;

	void				Set_Radius(_float fRadius);
	void				Set_Center(_float3 vCenter) override;

public:
	HRESULT			Initialize(void* pArg);
	virtual void	Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool	Intersect(CCollider::TYPE eType, CBounding* pBounding) override;

private:
	BoundingSphere*		m_pOriginalSphere = { nullptr }; // 소유자의 월드Mat을 적용하기전 상태를 저장해둠
	BoundingSphere*		m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END
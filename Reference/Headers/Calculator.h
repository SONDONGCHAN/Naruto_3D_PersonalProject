#pragma once
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator final : public CComponent
{

private:
	CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCalculator(const CCalculator& rhs);
	virtual ~CCalculator() = default;

public:
	virtual HRESULT Initialize_Prototype(); 
	virtual HRESULT Initialize(void* pArg) override;

	_float3	Ray_Picking_OnModel(CTransform* pTargetTransformCom, CModel* m_pModelCom, _fmatrix PivotMatrix, _vector MyPos, _vector dir);

public:
	static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_float3 m_vLastPos = {0.f, 0.f, 0.f};
};

END
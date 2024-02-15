#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Calculator final : public CComponent
{

private:
	Calculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Calculator(const Calculator& rhs);
	virtual ~Calculator() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	static Calculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
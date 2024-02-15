#include "Calculator.h"

Calculator::Calculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

Calculator::Calculator(const Calculator& rhs)
    : CComponent(rhs)
{
}

HRESULT Calculator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Calculator::Initialize(void* pArg)
{
    return S_OK;
}

Calculator* Calculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Calculator* pInstance = new Calculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* Calculator::Clone(void* pArg)
{
	Calculator* pInstance = new Calculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Calculator::Free()
{
	__super::Free();
}

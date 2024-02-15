#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_ViewMatrix_Inverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)));
	XMStoreFloat4x4(&m_ProjMatrix_Inverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ProjMatrix)));

	memcpy(&m_vCameraPos, &m_ViewMatrix_Inverse.m[3][0], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
    return new CPipeLine;
}

void CPipeLine::Free()
{
}

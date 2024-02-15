#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine : public CBase
{

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void		Set_ViewMatrix(_fmatrix _ViewMatrix){ XMStoreFloat4x4(&m_ViewMatrix, _ViewMatrix); }
	_matrix		Get_ViewMatrix()					{ return XMLoadFloat4x4(&m_ViewMatrix); }
	_float4x4	Get_ViewMatrix_Float()				{ return m_ViewMatrix; }
	_matrix		Get_ViewMatrix_Inverse()			{ return XMLoadFloat4x4(&m_ViewMatrix_Inverse); }
	_float4x4	Get_ViewMatrix_Float_Inverse()		{ return m_ViewMatrix_Inverse; }

	void		Set_ProjMatrix(_fmatrix _ViewMatrix){ XMStoreFloat4x4(&m_ProjMatrix, _ViewMatrix); }
	_matrix		Get_ProjMatrix()					{ return XMLoadFloat4x4(&m_ProjMatrix); }
	_float4x4	Get_ProjMatrix_Float()				{ return m_ProjMatrix; }
	_matrix		Get_ProjMatrix_Inverse()			{ return XMLoadFloat4x4(&m_ProjMatrix_Inverse); }
	_float4x4	Get_ProjMatrix_Float_Inverse()		{ return m_ProjMatrix_Inverse; }

	_vector		Get_CameraPos()			{ return XMLoadFloat4(&m_vCameraPos); }
	_float4		Get_CameraPos_Float()	{ return m_vCameraPos; }

public:
	void Tick();

private:
	_float4x4	m_ViewMatrix = {};
	_float4x4	m_ViewMatrix_Inverse = {};

	_float4x4	m_ProjMatrix = {};
	_float4x4	m_ProjMatrix_Inverse = {};

	_float4		m_vCameraPos = {};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END
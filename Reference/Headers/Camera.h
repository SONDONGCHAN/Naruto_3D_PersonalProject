#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public: 
	struct CAMERA_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4	vEye, vAt;

		_float	fFovy	= { 0.f };
		_float	fNear	= { 0.f };
		_float	fFar	= { 0.f };
		_float	fAspect = { 0.f };
	};
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CGameObject& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float	m_fFovy		= { 0.f };
	_float	m_fNear		= { 0.f };
	_float	m_fFar		= { 0.f };
	_float	m_fAspect	= { 0.f };

protected:
	HRESULT SetUp_TransformMatices();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
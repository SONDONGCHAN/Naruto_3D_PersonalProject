#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJ_DESC
	{
		CTransform* pParentTransform;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CModel* Get_CurrentModel() { return m_pCurrentModel; }


protected:
	CTransform*				m_pParentTransform = { nullptr };
	_float4x4				m_WorldMatrix;
	CModel*					m_pCurrentModel = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END


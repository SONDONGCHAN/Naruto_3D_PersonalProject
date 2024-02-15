#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* �÷��̾��� �� ������ ��Ƽ� ������ �ִ� ��ü. */

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		_float4x4*	pSocketMatrix;
		_float4		vRotation;
		_float3		vLocation;
		wstring		strPrototypeTag;
	};
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_SocketMatrixPtr(_float4x4* _pSocketMatrix) { m_pSocketMatrix = _pSocketMatrix;}

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };

private:
	_float4x4*		m_pSocketMatrix = { nullptr };

private:
	HRESULT Add_Component(const wstring& _strPrototypeTag);
	HRESULT Bind_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
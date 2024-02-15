#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CDeco : public CGameObject
{
public:
	struct DECO_DESC : public GAMEOBJECT_DESC
	{
		_float3 fpos = {};
		_float3 fRot = {};

		_uint	iDeco_Index;
	};

private:
	CDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeco(const CDeco& rhs);
	virtual ~CDeco() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	DECO_DESC* m_pDecoDesc;

private:
	HRESULT Add_Model_Component();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();


public:
	static CDeco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

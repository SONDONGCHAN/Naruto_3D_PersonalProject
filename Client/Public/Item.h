#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)



class CItem : public CGameObject
{
public:
	struct ITEM_DESC : public GAMEOBJECT_DESC
	{
		_float3 fpos = {};
		_uint	iItem_Index;
	};

private:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

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
	ITEM_DESC* m_pItemDesc;

private:
	HRESULT Add_Model_Component();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();


public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"
#include "Deco.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMap : public CGameObject
{
private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	_uint*			m_pMapIndex;

private:
	HRESULT Add_Model_Component();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
END


BEGIN(Client)

class CUI abstract : public CGameObject
{

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Point*	m_pVIBufferCom = { nullptr };

protected:
	_float4x4		m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Component();

public:
	virtual void Free() override;
};

END
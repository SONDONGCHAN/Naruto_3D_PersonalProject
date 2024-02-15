#pragma once
#include "Client_Defines.h"
#include "C:\Users\dhqks\Desktop\3DPersonal_Client\Reference\Headers\GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Line;
END

BEGIN(Client)

class CTrail_Line : public CGameObject
{

public:
	struct Trail_Line_DESC 
	{
		_float4x4*	pSocketMatrix;
		CTransform* pParentTransform;;
	};
private:
	CTrail_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Line(const CTrail_Line& rhs);
	virtual ~CTrail_Line() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Line*	m_pVIBufferCom = { nullptr };
	CTransform*		m_pParentTransform = { nullptr };

	_float4x4*		m_pSocketMatrix = { nullptr };
	_float4x4		m_WorldMatrix_Current;
	_float4x4		m_WorldMatrix_Old[30];
	_matrix  		m_OriginalMat = {};



private:
	HRESULT Add_Component(); 


public:
	static CTrail_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
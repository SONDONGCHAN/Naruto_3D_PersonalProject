#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CEffect_Mesh : public CGameObject
{
public:
	enum EFFECT_TYPE {EFFECT_FIREBALL_MAIN, EFFECT_FIREBALL_RING, EFFECT_END};

	struct EFFECT_DESC
	{
		EFFECT_TYPE	MyType;
		_vector vMyScale;
	};

private:
	CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual ~CEffect_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	State_Tick(_fmatrix WorldMatrix);
	void	Move_UV(_float fTimeDelta);
	void	Get_StartMat(_fmatrix WorldMatrix);

private:
	CShader*				m_pShaderCom	= { nullptr };
	vector<CModel*>			m_vModels;
	vector<CTexture*>		m_vTextures;

private:
	EFFECT_DESC m_MyDesc		= {};
	_float4x4	m_StartMat		= {};
	_float2		m_vUVMovement	= { 0.f, 0.f};
	_float		m_vUVSpeed		= { 1.f };
	_float		m_fAlpha		= { 1.f };
	_float		m_fBrightness	= { 1.f };


private:
	HRESULT		Add_Component();
	HRESULT		Bind_ShaderResources();

public:
	static CEffect_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

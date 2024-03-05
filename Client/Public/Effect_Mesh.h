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
	enum EFFECT_TYPE 
	{
		EFFECT_FIREBALL_MAIN, EFFECT_FIREBALL_RING, 
		EFFECT_CHIDORI,
		EFFECT_SHOCKWAVE, 
		EFFECT_KAMUI, EFFECT_KAMUI_SHOCK, 
		EFFECT_RASENGUN_CHARGE, EFFECT_RASENGUN_MAIN, EFFECT_RASENGUN_RUSH, EFFECT_RASENGUN_BOOM,
		EFFECT_RASENSHURIKEN_MAIN, EFFECT_RASENSHURIKEN_WING, EFFECT_RASENSHURIKEN_RING, EFFECT_RASENSHURIKEN_BOOM, EFFECT_RASENSHURIKEN_DECO,
		EFFECT_RASENGUNSUPER_MAIN, EFFECT_RASENGUNSUPER_NOISE, EFFECT_RASENGUNSUPER_DECO,
		EFFECT_WOOD_SWAP,
		EFFECT_KURAMA_SCRATCH, EFFECT_KURAMA_CLAW, EFFECT_KURAMA_RUSH,
		EFFECT_END};
	enum EFFECT_OPTION_UV	{ MOVE_X, MOVE_Y, MOVE_X_INVERSE, MOVE_Y_INVERSE, MOVE_All, MOVE_END};
	enum EFFECT_OPTION_SPIN { SPIN_X, SPIN_Y, SPIN_Z, SPIN_X_INVERSE, SPIN_Y_INVERSE, SPIN_Z_INVERSE, SPIN_RANDOM, SPIN_NONE, SPIN_END};

	
	struct EFFECT_DESC
	{
		EFFECT_TYPE			MyType;
		EFFECT_OPTION_UV	MyUVOption;
		EFFECT_OPTION_SPIN	MySpinOption;
		_vector				vMyScale;
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
	void	Start_Trigger();
	void	End_Trigger(_float fDiscardSpeed);
	void	Scale_Change(_float fTimeDelta);

private:
	_float	Lerp(_float start, _float end, _float ratio);
	void	Move_UV(EFFECT_OPTION_UV UVOption, _float fTimeDelta);

private:
	CShader*				m_pShaderCom	= { nullptr };
	vector<CModel*>			m_vModels;
	vector<CTexture*>		m_vTextures;

private:
	EFFECT_DESC m_MyDesc		= {};
	_vector		vCurrentScale	= {};
	_float		m_ScalingSpeed	= { 5.f };
	_float		m_AlphaSpeed = { 5.f };
	_float		m_ScalingRatio	= { 1.f };
	_uint		m_ModelCount		= { 0 };
	_float		m_ModelCountCheck = { 0.f };


	_float2		m_vUVMovement	= { 0.f, 0.f};
	_float		m_vUVSpeed		= { 1.f };
	_float		m_fAlpha		= { 1.f };
	_float		m_fBrightness	= { 1.f };
	_float3		m_vRatation		= {0.f, 0.f, 0.f};
	_float		m_fSpinSpeed	= {1.f};
	_bool		m_bEndTrigger	= false;
	_float		m_fDiscardColor = { 0.f };
	_float		m_fDiscardSpeed = { 1.f };


private:
	HRESULT		Add_Component();
	HRESULT		Bind_ShaderResources();

public:
	static CEffect_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Point : public CGameObject
{
private:
	CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Point(const CParticle_Point& rhs);
	virtual ~CParticle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool	Trigger(_vector vCenterPos);
	void	Set_Loop_ON(_bool isOn);

private:
	void	Sprite_Tick(_float fTimeDelta);

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Point*		m_pVIBufferCom = { nullptr };

private:
	_float		m_vSpriteTimeCal		= { 0.f };
	_float2		m_vSpriteCurrentRatio	= { 0.f, 0.f };
	CVIBuffer_Instancing::INSTANCE_DESC m_Instance_Desc = {};
	_float4		m_vCenterPos			= {0.f, 0.f, 0.f, 1.f};

private:
	HRESULT Add_Component(void* pArg);


public:
	static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
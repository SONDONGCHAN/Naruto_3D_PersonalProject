#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	enum  PARTICLE_MOVING { OPTION_DROP, OPTION_SPREAD, OPTION_END};
	enum  PARTICLE_SHAPE { SHAPE_SQUARE, SHAPE_RECTANGLE, SHAPE_NIDDLE, SHAPE_END };
	enum  PARTICLE_TEXTURE { TEXTURE_SPRITE, TEXTURE_NONE_SPRITE, TEXTURE_NONE, TEXTURE_END};

	struct INSTANCE_DESC
	{
		_uint				iNumInstance;
		_float3				vPivot;
		_float3				vCenter;
		_vector*			pCenter;
		_float3				vRange;
		_float2				vSize;
		_float2				vSpeed;
		_float2				vLifeTime;
		_bool				isLoop;
		_float4				vColor;
		_float				fDuration;
		PARTICLE_MOVING		MyOption_Moving;
		PARTICLE_SHAPE		MyOption_Shape;
		PARTICLE_TEXTURE	MyOption_Texture;
		wstring				strTextureTag;
		_float2				vSpriteRatio = {1.f ,1.f};
	};

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();
	virtual HRESULT Bind_Buffers();

private: _float	Lerp(_float start, _float end, _float ratio);

public:
	_bool	Trigger(_vector vCenterPos);
	void	Tick_Particle(_float fTimeDelta);
	void	Set_Loop_ON(_bool isOn) { m_LoopOn = isOn; }


private:
	void Tick_Drop(_float fTimeDelta);
	void Tick_Spread_Loop(_float fTimeDelta);
	void Tick_Spread(_float fTimeDelta);


protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	INSTANCE_DESC				m_InstanceData;

	_float*						m_pSpeed = { nullptr };
	_float*						m_pLifeTime = { nullptr };
	_float*						m_pMaxLifeTime = { nullptr };
	_float*						m_pDelayTime = { nullptr };
	_float*						m_pMaxSizeY = { nullptr };

	_float						m_fTimeAcc = { 0.f };
	_bool						m_isFinished = { true };
	_bool						m_LoopOn	= false;

private:
	D3D11_MAPPED_SUBRESOURCE			m_SubResource{};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
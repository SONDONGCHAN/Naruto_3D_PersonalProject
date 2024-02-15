#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	struct BACKGROUND_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL MyLevel;
		_uint Loading_Texture_Index;
	};

private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pLogo_TextureCom = { nullptr };
	CTexture*		m_pLoading_TextureCom = { nullptr };
	CTexture*		m_pCustom_TextureCom = { nullptr };

	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ViewMatrix, m_ProjMatrix;

	LEVEL			m_MyLevel = { LEVEL_END };
	_uint			m_iLoading_Texture_Index = 0;

private:
	HRESULT Add_Component ();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
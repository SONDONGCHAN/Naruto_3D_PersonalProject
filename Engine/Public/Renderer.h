#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
public: 
	enum RENDERGROUP{ RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_GLOW, RENDER_DISTORTION, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Render();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif // _DEBUG
	
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	class CGameInstance*				m_pGameInstance = { nullptr };

private:
	class CShader*						m_pShader_Deferred			= { nullptr };
	class CShader*						m_pShader_PostProcessing	= { nullptr };

	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;


#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugCom;
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_NonLights();
	HRESULT Render_Final();
	HRESULT Render_Glow();
	HRESULT Render_Bloom();
	HRESULT Render_Distortion();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_PostProcessing();


#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END
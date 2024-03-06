#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLIne.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppDeviceContext);
	void	Tick_Engine(_float fTimeDelta);
	HRESULT Draw(const _float4& vClearColor);
	HRESULT Present();

	void	Clear(_uint iNumLevels);

public: /* For.Key_Manager */
	_bool	Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime = nullptr);
	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Up(_ubyte byKeyID);
	_bool	Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouse);
	_long	Mouse_Move(MOUSEMOVESTATE eMouseState);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag,_uint iIndex = 0);


public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif // _DEBUG

public: /* For.Timer_Manager */
	_float Compute_TimeDelta(const wstring& pTimerTag);
	HRESULT Add_Timer(const wstring& pTimerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);

public: /* For.PipeLine */
	void		Set_ViewMatrix(_fmatrix _ViewMatrix);
	_matrix		Get_ViewMatrix();
	_float4x4	Get_ViewMatrix_Float();
	_matrix		Get_ViewMatrix_Inverse();
	_float4x4	Get_ViewMatrix_Float_Inverse();
	void		Set_ProjMatrix(_fmatrix _ViewMatrix);
	_matrix		Get_ProjMatrix();
	_float4x4	Get_ProjMatrix_Float();
	_matrix		Get_ProjMatrix_Inverse();
	_float4x4	Get_ProjMatrix_Float_Inverse();
	_vector		Get_CameraPos();
	_float4		Get_CameraPos_Float();

public: /* For.Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Collider_Manager */
	HRESULT Add_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* pCollider, void* pArg = nullptr);
	void	Delete_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* pCollider);
	void	Check_Collision_For_MyEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strColliderLayerTag);
	void	Check_Collision_For_TargetEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag);
	_bool	Is_Collision(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strColliderLayerTag);
	void	Kill_Dead_Collider(_uint iLevelIndex, CCollider* DeadColliderCom);


public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, FONT_ORIGIN_OPTION _option = ORIGIN_CENTER, _float fRotation = 0.f, _float fScale = 1.f);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_SRV(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

public: /* For.Sound_Manager */
	void PlaySoundW(const string pSoundKey, CHANNELID eID, float fVolume, _bool _overlap = false);
	void PlayBGM(string pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);


#ifdef _DEBUG
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CKey_Manager*			m_pKey_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager= { nullptr };
	class CCollider_Manager*	m_pCollider_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };



public:
	static void Release_Engine();
	void Release_Manager();
	virtual void Free() override;
};

END
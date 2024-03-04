#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Key_Manager.h"
#include "Light_Manager.h"
#include "Collider_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppDeviceContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppDeviceContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create(hInst, GraphicDesc.hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create(iNumLevels);
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;

}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;


	m_pKey_Manager->Tick();
	m_pObject_Manager->Priority_Tick(fTimeDelta);
	m_pPipeLine->Tick();
	m_pObject_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Late_Tick(fTimeDelta);
}

HRESULT CGameInstance::Draw(const _float4& vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	
	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();
	
	m_pRenderer->Render();	
	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

void CGameInstance::Clear(_uint iNumLevels)
{
	if (nullptr == m_pObject_Manager || nullptr == m_pComponent_Manager)
		return;

	m_pObject_Manager		->Clear(iNumLevels);
	m_pComponent_Manager	->Clear(iNumLevels);
	m_pCollider_Manager		->Collider_Clear(iNumLevels);
}
/* For.Key_Manager */

bool CGameInstance::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime)
{
	return m_pKey_Manager->Key_Pressing(byKeyID, fTimeDelta, pPressingTime);
}
bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pKey_Manager->Key_Down(byKeyID);
}
bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pKey_Manager->Key_Up(byKeyID);
}
bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_pKey_Manager->Mouse_Pressing(eMouse);
}
bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return m_pKey_Manager->Mouse_Down(eMouse);
}
bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_pKey_Manager->Mouse_Up(eMouse);
}
long CGameInstance::Mouse_Move(MOUSEMOVESTATE eMouseState)
{
	return m_pKey_Manager->Mouse_Move(eMouseState);
}

/* For.Level_Manager */

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

/* For.Object_Manager */

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

/* For.Renderer*/

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pGameObject);
}

HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}

/* For.Timer_Manager */

_float CGameInstance::Compute_TimeDelta(const wstring& pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring& pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

/* For.Component_Manager */

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

/* For.PipeLine */

void CGameInstance::Set_ViewMatrix(_fmatrix _ViewMatrix)
{
	m_pPipeLine->Set_ViewMatrix(_ViewMatrix);
}

_matrix CGameInstance::Get_ViewMatrix()
{
	return m_pPipeLine->Get_ViewMatrix();
}

_float4x4 CGameInstance::Get_ViewMatrix_Float()
{
	return m_pPipeLine->Get_ViewMatrix_Float();
}

_matrix CGameInstance::Get_ViewMatrix_Inverse()
{
	return m_pPipeLine->Get_ViewMatrix_Inverse();
}

_float4x4 CGameInstance::Get_ViewMatrix_Float_Inverse()
{
	return m_pPipeLine->Get_ViewMatrix_Float_Inverse();
}

void CGameInstance::Set_ProjMatrix(_fmatrix _ViewMatrix)
{
	m_pPipeLine->Set_ProjMatrix(_ViewMatrix);
}

_matrix CGameInstance::Get_ProjMatrix()
{
	return m_pPipeLine->Get_ProjMatrix();
}

_float4x4 CGameInstance::Get_ProjMatrix_Float()
{
	return m_pPipeLine->Get_ProjMatrix_Float();
}

_matrix CGameInstance::Get_ProjMatrix_Inverse()
{
	return m_pPipeLine->Get_ProjMatrix_Inverse();
}

_float4x4 CGameInstance::Get_ProjMatrix_Float_Inverse()
{
	return m_pPipeLine->Get_ProjMatrix_Float_Inverse();
}

_vector CGameInstance::Get_CameraPos()
{
	return m_pPipeLine->Get_CameraPos();
}

_float4 CGameInstance::Get_CameraPos_Float()
{
	return m_pPipeLine->Get_CameraPos_Float();
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Light(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Add_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* pCollider, void* pArg)
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return m_pCollider_Manager->Add_Collider(iLevelIndex, strLayerTag, pCollider, pArg);
}

void CGameInstance::Delete_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* pCollider)
{
	m_pCollider_Manager->Delete_Collider(iLevelIndex, strLayerTag, pCollider);
}

void CGameInstance::Check_Collision_For_MyEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag)
{
	if (nullptr == m_pCollider_Manager)
		return;

	return m_pCollider_Manager->Check_Collision_For_MyEvent(iLevelIndex, MyColliderCom, strTargetColliderLayerTag);
}

void CGameInstance::Check_Collision_For_TargetEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag)
{
	if (nullptr == m_pCollider_Manager)
		return;

	return m_pCollider_Manager->Check_Collision_For_TargetEvent(iLevelIndex, MyColliderCom, strTargetColliderLayerTag, strMyColliderLayerTag);
}

_bool CGameInstance::Is_Collision(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strColliderLayerTag)
{
	return m_pCollider_Manager->Is_Collision(iLevelIndex, MyColliderCom, strColliderLayerTag);
}

void CGameInstance::Kill_Dead_Collider(_uint iLevelIndex, CCollider* DeadColliderCom)
{
	return m_pCollider_Manager->Kill_Dead_Collider(iLevelIndex, DeadColliderCom);
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, FONT_ORIGIN_OPTION _option, _float fRotation, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, _option, fRotation, fScale);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}
HRESULT CGameInstance::Bind_SRV(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_SRV(strTargetTag, pShader, pConstantName);
}
#ifdef _DEBUG
HRESULT CGameInstance::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_MRT(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_MRT(strMRTTag, pShader, pVIBuffer);
}
#endif // _DEBUG
void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();

	DestroyInstance();
}

void CGameInstance::Release_Manager()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pTarget_Manager);

}

void CGameInstance::Free()
{
}

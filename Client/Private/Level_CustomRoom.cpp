#include "stdafx.h"
#include "Level_CustomRoom.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Camera_Free.h"

CLevel_CustomRoom::CLevel_CustomRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_CustomRoom::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;


    return S_OK;
}

void CLevel_CustomRoom::Tick(_float fTimeDelta)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
            return;

        return;
    }
}

HRESULT CLevel_CustomRoom::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("커스터마이징 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_CustomRoom::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    CBackGround::BACKGROUND_DESC		BackGroundDesc = {};
    BackGroundDesc.MyLevel = LEVEL_CUSTOMROOM;
    BackGroundDesc.Loading_Texture_Index = 0;

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_CUSTOMROOM, strLayerTag, TEXT("Prototype_GameObject_BackGround"), &BackGroundDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CustomRoom::Ready_Layer_Player(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_CUSTOMROOM, strLayerTag, TEXT("Prototype_GameObject_Player_Custom"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_CustomRoom::Ready_Layer_UI(const wstring& strLayerTag)
{

    return S_OK;
}

HRESULT CLevel_CustomRoom::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC		CameraDesc = {};
   
    CameraDesc.fMouseSensor = 0.05f;
    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.vEye = _float4(0.f, 0.7f, 2.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.7f, 0.f, 1.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.2f;
    CameraDesc.fFar = 1000.f;
    CameraDesc.fSpeedPerSec = 20.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.Current_LEVEL = LEVEL_CUSTOMROOM;

   
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_CUSTOMROOM, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
        return E_FAIL;
   
    return S_OK;
}

HRESULT CLevel_CustomRoom::Ready_Lights()
{
    LIGHT_DESC		LightDesc = {};
    
    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, -1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
    
    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;
    
    return S_OK;
}

CLevel_CustomRoom* CLevel_CustomRoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_CustomRoom* pInstance = new CLevel_CustomRoom(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_CustomRoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_CustomRoom::Free()
{
    __super::Free();
}

#include "stdafx.h"
#include "Level_GamePlay.h"
#include "Level_Loading.h"
#include "Camera_Free.h"
#include "Item.h"
#include "LandObject.h"
#include "Player_Custom.h"
#include "UI_System.h"



CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;


	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI( TEXT("Layer_System_UI") )))
		return E_FAIL;
	

	//if (FAILED(Ready_Layer_Item(TEXT("Layer_Item"))))
	//		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	m_fRun_Time+= fTimeDelta;

	Add_Run_Time_Object();

	if (Check_Can_NextLevel())
	{
		m_fEnd_Time += fTimeDelta;

		if (m_fEnd_Time > 10.f)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS))))
				return;
		}
		return;
	}

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS))))
			return;

		return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc = {};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.fFovy	= XMConvertToRadians(60.0f);
	CameraDesc.vEye		= _float4(0.0f, 30.f, -25.f, 1.f);
	CameraDesc.vAt		= _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fAspect	= (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear	= 0.2f;
	CameraDesc.fFar		= 1000.f;
	CameraDesc.fSpeedPerSec		= 20.f;
	CameraDesc.fRotationPerSec	= XMConvertToRadians(90.f);
	CameraDesc.Current_LEVEL = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const wstring& strLayerTag)
{
	_uint Map_Index = 0;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Map"),&Map_Index)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC			LandObjDesc{};
	
	LandObjDesc.pMapTransform	= dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Map"), g_strTransformTag)); 
	LandObjDesc.pCamera			= dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));
	LandObjDesc.Current_Level	= LEVEL_GAMEPLAY;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), LandObjDesc)))
		return E_FAIL;

	 if(FAILED(Ready_Layer_Player(TEXT("Layer_Player"), LandObjDesc)))
		return E_FAIL;
	
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC& GameObjDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &GameObjDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC& GameObjDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_LeafNinja"), &GameObjDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_Samurai"), &GameObjDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Boss_Naruto"), &GameObjDesc)))
		return E_FAIL;

	CGameObject* pMonster_LeafNinja = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 0);
	pMonsters.push_back(pMonster_LeafNinja);
	m_iNumEnemy++;
	CGameObject* pMonster_Samurai	= m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 1);
	pMonsters.push_back(pMonster_Samurai);
	m_iNumEnemy++;
	CGameObject* pBoss_Naruto		= m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 2);
	pMonsters.push_back(pBoss_Naruto);
	m_iNumEnemy++;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(const wstring& strLayerTag)
{
	CItem::ITEM_DESC desc;

	desc.fpos = { 0.f, 0.f, 0.f };
	desc.iItem_Index = 4;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Item"),&desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
{
	return S_OK;
}

void CLevel_GamePlay::Add_Run_Time_Object()
{
	if (m_fRun_Time > 1.f && !m_bCreated[0])
	{
		m_bCreated[0] = true;

		CUI_System::UI_System_DESC desc;

		desc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		desc.vSize = { 1000.f, 200.f };
		desc.strPrototypeTag = TEXT("Prototype_Component_Texture_Battle_Start");
		desc.fRenderingTime = 3.f;
		desc.fAppearTime = 0.7f;

		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_System_UI"), TEXT("Prototype_GameObject_UI_System"), &desc);
	}

	if (m_fEnd_Time > 2.f && !m_bCreated[1])
	{
		m_bCreated[1] = true;
		
		CUI_System::UI_System_DESC desc;
		
		desc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		desc.vSize = { 1000.f, 200.f };
		desc.strPrototypeTag = TEXT("Prototype_Component_Texture_Battle_Victory");
		desc.fRenderingTime = 3.f;
		desc.fAppearTime = 0.7f;
		
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_System_UI"), TEXT("Prototype_GameObject_UI_System"), &desc);
	}
}

_bool CLevel_GamePlay::Check_Can_NextLevel()
{
	_uint DeadCheck = { 0 };

	for (_uint i = 0 ; i< m_iNumEnemy ; i++)
	{
		if (pMonsters[i]->Get_DeadCheck())
			DeadCheck++;
	}

	if(DeadCheck == m_iNumEnemy)
		return true;

	else
		return false;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}

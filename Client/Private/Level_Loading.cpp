#include "stdafx.h"
#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_CustomRoom.h"
#include "Level_GamePlay.h"
#include "BackGround.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(Ready_Loading()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_UI()))
	//	return E_FAIL;


	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		//if (GetKeyState(VK_RETURN) & 0x8000)
		//{
			CLevel* pLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_CUSTOMROOM:
				pLevel = CLevel_CustomRoom::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;

			}
			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel)))
				return;
		}
	//}
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

#ifdef _DEBUG
	m_pLoader->Output_LoadingText();
#endif
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	CBackGround::BACKGROUND_DESC		BackGroundDesc = {};

	BackGroundDesc.fSpeedPerSec = 10.f;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	BackGroundDesc.MyLevel = LEVEL_LOADING;

	uniform_int_distribution <int>	IndexRange(0, 13);

	BackGroundDesc.Loading_Texture_Index = IndexRange(m_RandomNumber);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, TEXT("Layer_Loading"), TEXT("Prototype_GameObject_BackGround"), &BackGroundDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Loading()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/T_UI_LoadingScreen_%02d.png"), 14))))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

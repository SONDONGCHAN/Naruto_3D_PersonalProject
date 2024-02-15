#include "stdafx.h"
#include "Player.h"
#include "Player_Naruto.h"
#include "Player_Custom.h"



CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;
	m_pMapTransform = pGameObjectDesc->pMapTransform;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Character()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
	m_pCurrentCharacter->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	Check_Tag();
	m_pCurrentCharacter->Tick(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pCurrentCharacter->Late_Tick(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	m_pCurrentCharacter->Render();

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_T))
	{
		m_iCurrent_Character_Index++;

		if (m_iCurrent_Character_Index > 1)
			m_iCurrent_Character_Index = 0;
		
		if (m_iCurrent_Character_Index == 1)
			dynamic_cast<CPlayer_Custom*>(m_pCurrentCharacter)->Set_State(PLAYER_EXIT);

		else if (m_iCurrent_Character_Index == 0)
			dynamic_cast<CPlayer_Naruto*>(m_pCurrentCharacter)->Set_State(PLAYER_EXIT);

		m_bTagging = true;
	}
}

void CPlayer::Check_Tag()
{
	if (!m_bTagging)
		return;

	_float4x4 OldCharacterWorldMat;
	XMStoreFloat4x4(&OldCharacterWorldMat, m_pCurrentCharacter->Get_TranformCom()->Get_WorldMatrix());

	if (m_iCurrent_Character_Index == 1)
	{
		if (!(dynamic_cast<CPlayer_Custom*>(m_pCurrentCharacter)->Get_BodyModel()->Get_Current_Animation()->Get_Finished()))
			return;

		_int NaviIndex = m_pCurrentCharacter->Get_Navigation()->Get_CurrentIndex();
		m_pCurrentCharacter = m_Charaters.find(L"Character_Naruto")->second;
		dynamic_cast<CPlayer_Naruto*>(m_pCurrentCharacter)->Set_State(PLAYER_APPEAR);
		m_pCurrentCharacter->Get_Navigation()->Set_CurrentIndex(NaviIndex);

	}
	else if (m_iCurrent_Character_Index == 0)
	{
		if (!(dynamic_cast<CPlayer_Naruto*>(m_pCurrentCharacter)->Get_BodyModel()->Get_Current_Animation()->Get_Finished()))
			return;

		_int NaviIndex = m_pCurrentCharacter->Get_Navigation()->Get_CurrentIndex();
		m_pCurrentCharacter = m_Charaters.find(L"Character_Custom")->second;
		dynamic_cast<CPlayer_Custom*>(m_pCurrentCharacter)->Set_State(PLAYER_APPEAR);
		m_pCurrentCharacter->Get_Navigation()->Set_CurrentIndex(NaviIndex);
	}

	m_pCurrentCharacter->Get_TranformCom()->Set_World(OldCharacterWorldMat);

	m_bTagging = false;
}

HRESULT CPlayer::Add_Character()
{
	CLandObject::LANDOBJ_DESC			Player_Naruto_Desc{};
	Player_Naruto_Desc.pMapTransform = m_pMapTransform;

	CPlayer_Naruto* pNaruto = dynamic_cast<CPlayer_Naruto*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Naruto"), &Player_Naruto_Desc));
	if (nullptr == pNaruto)
		return E_FAIL;

	m_Charaters.emplace(TEXT("Character_Naruto"), pNaruto);


	CPlayer_Custom* pCustom = dynamic_cast<CPlayer_Custom*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Custom"), &Player_Naruto_Desc));
	if (nullptr == pCustom)
		return E_FAIL;
	
	m_Charaters.emplace(TEXT("Character_Custom"), pCustom);


	m_pCurrentCharacter = pCustom;

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	for (auto& Pair : m_Charaters)
		Safe_Release(Pair.second);
	m_Charaters.clear();

	//Safe_Release(m_pCurrentCharacter);
	Safe_Release(m_pMapTransform);


	__super::Free();
}

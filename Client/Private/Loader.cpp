#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Trail_Line.h"
#include "Particle_Rect.h"
#include "Particle_Point.h"
#include "Body_Monster_LeafNinja.h"
#include "Body_Monster_Samurai.h"
#include "Body_Boss_Naruto.h"
#include "Body_Player_Naruto.h"
#include "Body_Player_Custom_Face.h"
#include "Body_Player_Custom_HeadGear.h"
#include "Body_Player_Custom_Lower.h"
#include "Body_Player_Custom_Upper.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Player_Naruto.h"
#include "Player_Custom.h"
#include "Map.h"
#include "Monster_LeafNinja.h"
#include "Monster_Samurai.h"
#include "Boss_Naruto.h"
#include "Item.h"
#include "Deco.h"
#include "Rasengun.h"
#include "RasenShuriken.h"
#include "Rasengun_Super.h"
#include "FlameBomb.h"
#include "Wood_Dragon.h"
#include "Wood_Swap.h"
#include "Kamui.h"
#include "Chidori.h"
#include "Wood_Hand.h"
#include "Weapon.h"
#include "UI_Player_Status.h"
#include "UI_Player_Skills.h"




CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY wThreadMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	pLoader->Loading_Selector();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0 , wThreadMain,this, 0 , nullptr);
	
	if (0 == m_hThread)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLoader::Loading_Selector()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = {};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_CUSTOMROOM:
		hr = Loading_For_CustomRoomLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

#ifdef _DEBUG
void CLoader::Output_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}
#endif

HRESULT CLoader::Loading_For_LogoLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Logo_Main.png"), 1))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_CustomRoomLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_CustomRoom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CustomRoom/T_UI_Fcility_VRNijutsuTougijou_BC.png"), 1))))
		return E_FAIL;


	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	///* For.Prototype_Component_Player_Custom_Face_Byaku */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Face_Byaku"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Face/Player_Custom_Face_Byaku/Player_Custom_Face_Byaku.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_Face_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Face_Mask"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Face/Player_Custom_Face_Mask/Player_Custom_Face_Mask.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_HeadGear_AllBack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_HeadGear_AllBack"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_HeadGear/Player_Custom_HeadGear_AllBack/Player_Custom_HeadGear_AllBack.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_HeadGear_HalfUpBun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_HeadGear_HatTurban"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_HeadGear/Player_Custom_HeadGear_HatTurban/Player_Custom_HeadGear_HatTurban.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_Lower_PantsCuffed */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Lower_PantsCuffed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Lower/Player_Custom_Lower_PantsCuffed/Player_Custom_Lower_PantsCuffed.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_Lower_PantsCut */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Lower_PantsCut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Lower/Player_Custom_Lower_PantsCut/Player_Custom_Lower_PantsCut.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_Upper_Jacket */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Upper_Jacket"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Upper/Player_Custom_Upper_Jacket/Player_Custom_Upper_Jacket.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Player_Custom_Upper_Vest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Upper_Vest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Custom/Player_Custom_Upper/Player_Custom_Upper_Vest/Player_Custom_Upper_Vest.dat"))))
		return E_FAIL;

	///////////Weapon/////////////
	///* For.Prototype_Component_Weapon_NinjaSword*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_NinjaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Weapon/Weapon_NinjaSword/Weapon_NinjaSword.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Weapon_NinjaSwordSheath*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_NinjaSwordSheath"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Weapon/Weapon_NinjaSwordSheath/Weapon_NinjaSwordSheath.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Weapon_SamuraiSword*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_SamuraiSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Weapon/Weapon_SamuraiSword/Weapon_SamuraiSword.dat"))))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");
	///////////Buffer///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Shader_VtxAnimMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxAnimMesh_OutLine*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh_OutLine"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh_OutLine.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxPoint_Rect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Rect.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_GameObject_Player_Custom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Custom"),
		CPlayer_Custom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Player_Custom_Face */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player_Custom_Face"),
		CBody_Player_Custom_Face::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Player_Custom_HeadGear */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player_Custom_HeadGear"),
		CBody_Player_Custom_HeadGear::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Player_Custom_Lower */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player_Custom_Lower"),
		CBody_Player_Custom_Lower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Player_Custom_Upper */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player_Custom_Upper"),
		CBody_Player_Custom_Upper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_For_GamePlayLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	// 플레이어 상태 UI
	/* For.Prototype_Component_Texture_Status */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Status"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_SP_Gauge_Base_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Rasengun_Super */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Rasengun_Super"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_Rasengan_Super_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Status_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_LifeGauge_Base2.png"), 1))))
		return E_FAIL;

	// 스킬 UI
	/* For.Prototype_Component_Texture_Icon_Skill_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Skill_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Skill_Gauge_Base_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Rasengun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Rasengun"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_Rasengan_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_RasenShuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_RasenShuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_TrueRasenShuriken_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_WoodSwap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_WoodSwap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Clone_BC.png"), 1))))
		return E_FAIL;
	

	
	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	//////////Player//////////
	///* For.Prototype_Component_Player_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_Naruto"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Naruto/Naruto.dat"))))
		return E_FAIL;


	//////////Monster//////////
	///* For.Prototype_Component_Body_Monster_LeafNinja */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Body_Monster_LeafNinja"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Monster/Monster_LeafNinja/Monster_LeafNinja.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Body_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Body_Monster_Samurai"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Monster/Monster_Samurai/Monster_Samurai.dat"))))
		return E_FAIL;

	//////////Skill///////////
	///* For.Prototype_Component_Body_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Skill_Wood_Dragon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/Wood_Dragon/Wood_Dragon.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Skill_WoodHand_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Skill_WoodHand_L"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/WoodHand_L/WoodHand_L.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Skill_WoodHand_R */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Skill_WoodHand_R"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/WoodHand_R/WoodHand_R.dat"))))
		return E_FAIL;
	///////////Weapon/////////////


	////////////Map//////////////
	///* For.Prototype_Component_Map_KonohaVillage_Robby */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Map_KonohaVillage_Robby"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Map/All_Maps/KonohaVillage_Robby.dat"))))
	//	return E_FAIL;

	/* For.Prototype_Component_Map_Stadium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Map_Stadium"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Map/All_Maps/Map_Stadium.dat"))))
		return E_FAIL;


	///////////Particle///////////
	/* For.Prototype_Component_VIBuffer_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 2000))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navi_KonohaVillage_Robby.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Navi_Map_Stadium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navi_Map_Stadium"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navi_Map_Stadium.dat")))))
		return E_FAIL;


	m_strLoadingText = TEXT("충돌체를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"),VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPoint_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Trail.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;


	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

	// /* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_GameObject_Player_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Naruto"),
		CPlayer_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Player_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player_Naruto"),
		CBody_Player_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_GameObject_Monster_LeafNinja */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_LeafNinja"),
		CMonster_LeafNinja::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Body_Monster_LeafNinja */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Monster_LeafNinja"),
		CBody_Monster_LeafNinja::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_GameObject_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Samurai"),
		CMonster_Samurai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Monster_Samurai"),
		CBody_Monster_Samurai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_GameObject_Boss_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Naruto"),
		CBoss_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Boss_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Boss_Naruto"),
		CBody_Boss_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// /* For.Prototype_GameObject_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
	 CItem::Create(m_pDevice, m_pContext))))
	 return E_FAIL;
	
	// ====이펙트====
	/* For.Prototype_GameObject_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rect"),
	 CParticle_Rect::Create(m_pDevice, m_pContext))))
	 return E_FAIL;
	/* For.Prototype_GameObject_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Point"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Trail_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail_Line"),
		CTrail_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// ====UI=====
	/* For.Prototype_GameObject_UI_Player_Status*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Status"),
		CUI_Player_Status::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Player_Skills*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Skills"),
		CUI_Player_Skills::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_strLoadingText = TEXT("스킬를(을) 로딩 중 입니다.");
	
	/* For.Prototype_GameObject_Skill_Rasengun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Rasengun"),
		CRasengun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_RasenShuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_RasenShuriken"),
		CRasenShuriken::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Skill_Rasengun_Super */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Rasengun_Super"),
		CRasengun_Super::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_Wood_Swap*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Wood_Swap"),
		CWood_Swap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_FlameBomb */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_FlameBomb"),
		CFlameBomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_Wood_Dragon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Wood_Dragon"),
		CWood_Dragon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_Kamui*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Kamui"),
		CKamui::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_Chidori*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Chidori"),
		CChidori::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_Wood_Hand*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Wood_Hand"),
		CWood_Hand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}


CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	
	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

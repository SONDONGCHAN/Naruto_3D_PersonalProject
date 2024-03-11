#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Trail_Line.h"
#include "Particle_Rect.h"
#include "Particle_Point.h"
#include "Body_Monster_LeafNinja.h"
#include "Body_Monster_Samurai.h"
#include "Body_Boss_Naruto.h"
#include "Body_Boss_Kurama.h"
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
#include "Boss_Kurama.h"
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
#include "Kurama_Scratch.h"
#include "Weapon.h"
#include "UI_Player_Status.h"
#include "UI_Player_Skills.h"
#include "UI_Player_Custom.h"
#include "UI_Monster_Status.h"
#include "UI_System.h"
#include "UI_Boss_Status.h"
#include "EventTrigger.h"
#include "Effect_Mesh.h"
#include "Sky.h"

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
	case LEVEL_BOSS:
		hr = Loading_For_BossStageLevel();
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CustomRoom/UI/T_UI_Common_RoleItemWindow_BC.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CustomRoom/UI/T_UI_Common_RoleItem_WinTitle_BC.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Parts"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CustomRoom/UI/T_ui_Menu_Btn_05_Def_BC.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Parts_Sel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CustomRoom/UI/T_ui_Menu_Btn_05_Sel_BC.png"), 1))))
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
	/* For.Prototype_Component_Shader_VtxMesh_OutLine*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_OutLine"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_OutLine.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
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

	// ====UI=====
	/* For.Prototype_GameObject_UI_Player_Custom*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Custom"),
		CUI_Player_Custom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Circle.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Circle_Noise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Circle_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Circle_Noise.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Battle_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Start"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Announce_BattleStart_BC_LOCL.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Battle_Victory */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle_Victory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Announce_Win_BC_LOCL.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_HIT */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HIT"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Announce_Hit_Text_LOCL_0.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_HITS*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HITS"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Announce_Hit_Small_LOCL.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_KO*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KO"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Announce_KO_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Number*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/Number/T_UI_Announce_Hit_Digit_%d.png"), 10))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Boss_Base_Hp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Base_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/Boss/T_UI_Boss_Gauge1.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Boss_Face_Naruto*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Face_Naruto"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/Boss/T_UI_Dialog_Character_D34_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Boss_Face_Kurama*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Face_Kurama"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/Boss/T_UI_Boss_Kurama_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Boss_Hp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/Boss/T_UI_Boss_Gauge2_M.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlueSky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlueSky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/BlueSky.dds"), 1))))
		return E_FAIL;


	// 플레이어 상태 UI
	/* For.Prototype_Component_Texture_Status */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Status"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_SP_Gauge_Base_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Status_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Status_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_LifeGauge_Base2.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Rasengun_Super */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Rasengun_Super"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_Rasengan_Super_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Kamui */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Kamui"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_SP_KamuiKakashi_BC.png"), 1))))
		return E_FAIL;

	// 몬스터 UI
	/* For.Prototype_Component_Texture_Monster_Hp_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_Hp_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_LifeGauge_frame_M.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Monster_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_LifeGauge_Base_BC.png"), 1))))
		return E_FAIL;

	// 스킬 UI
	/* For.Prototype_Component_Texture_Icon_Skill_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Skill_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Base_UI/T_UI_Skill_Gauge_Base_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Rasengun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Rasengun"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_Rasengan_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_RasenShuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_RasenShuriken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_TrueRasenShuriken_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_FireBall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_FireBall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_FireBall_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_Chidori */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Chidori"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_Chidori_BC.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Icon_WoodHand */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_WoodHand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Skill_LaughingMonk_BC.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Icon_WoodSwap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_WoodSwap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Icon/T_UI_Clone_BC.png"), 1))))
		return E_FAIL;
	
	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Calculator*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Calculator"),
		CCalculator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	//////////Player//////////
	///* For.Prototype_Component_Player_Naruto */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Player_Naruto"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Player/Naruto/Naruto.dat"))))
		return E_FAIL;


	//////////Monster//////////
	///* For.Prototype_Component_Body_Monster_LeafNinja */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Body_Monster_LeafNinja"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Monster/Monster_LeafNinja/Monster_LeafNinja.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Body_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Body_Monster_Samurai"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Monster/Monster_Samurai/Monster_Samurai.dat"))))
		return E_FAIL;

	//////////Skill///////////
	///* For.Prototype_Component_Body_Monster_Samurai */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Skill_Wood_Dragon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/Wood_Dragon/Wood_Dragon.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Skill_WoodHand_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Skill_WoodHand_L"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/WoodHand_L/WoodHand_L.dat"))))
		return E_FAIL;
	///* For.Prototype_Component_Skill_WoodHand_R */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Skill_WoodHand_R"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Skill/WoodHand_R/WoodHand_R.dat"))))
		return E_FAIL;
	///////////Weapon/////////////


	////////////Map//////////////
	///* For.Prototype_Component_Map_Stadium */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Map_Stadium"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Map/All_Maps/Map_Stadium.dat"))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Map_Stadium"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Map/All_Maps/Map_Stadium_N.dat"))))
		return E_FAIL;

	/////////////Effect/////////////////
	//////////FireBall/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Main"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/FireBall/FireBall_Main/FireBall_Main.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Aura"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/FireBall/FireBall_Aura/FireBall_Aura.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Ring"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/FireBall/FireBall_Ring/FireBall_Ring.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Effect_Fire_Main.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Main_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Effect_Fire_Main_2.png"), 1))))
	 	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Texture/ExplosionMask.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Small"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Texture/Fire_Small.png"), 1))))
		return E_FAIL;
   
	//////////Chidori/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_1.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_2.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_3.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_4.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_5.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_6.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_7.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_8.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Chidori//Chidori_9.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chidori_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Effect_Chidori_Mask.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chidori_Mask_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Effect_Chidori_Mask_2.png"), 1))))
		return E_FAIL;

	//////////WoodHand/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockWave_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/ShockWave_Mask_.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke_Sprite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Texture/Smoke_Sprite.png"), 1))))
		return E_FAIL;

	//////////Kamui/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kamui"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Kamui/Kamui_Boom.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_kamui_Boom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Kamui_Boom_.png"), 1))))
		return E_FAIL;

	//////////Rasengun/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Main"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun/Rasengun_Main.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Charge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun/Rasengun_Charge.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Rush"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun/Rasengun_Rush.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Boom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun/Rasengun_Boom.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasengun_Main.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Boom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/ShockWave_Mask_.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Rush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasengun_Rush_Mask_.png"), 1))))
		return E_FAIL;

	//////////RasenShuriken/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Wing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/RasenShuriken/RasenShuriken_Wing.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Ring"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/RasenShuriken/RasenShuriken_Ring.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Deco"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/RasenShuriken/RasenShuriken_Deco.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Wing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasenshuriken_Wing.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasenshuriken_Ring.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Deco"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasenshuriken_Deco.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/RasenShuriken_Dissolve.png"), 1))))
		return E_FAIL;

	//////////Rasengun_Super/////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Super"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun_Super/Rasengun_Super.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Super_"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Rasengun_Super/Rasengun_Super_.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Super_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Rasengun_Super_Noise.png"), 1))))
		return E_FAIL;

	////////////WoodSwap///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wood_Swap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/WoodSwap/Wood_Swap.dat"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wood_Swap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Wood_Swap.png"), 1))))
		return E_FAIL;

	////////////Kurama_Scratch///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Scratch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Kurama/Kurama_Scratch.dat"))))
		return E_FAIL;

	////////////Kurama_Claw///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Claw"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Kurama/Kurama_Claw_.dat"))))
		return E_FAIL;

	////////////Kurama_Rush///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Rush"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Kurama/Kurama_Rush.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Kurama_Rush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/Crack.png"), 1))))
		return E_FAIL;

	////////////Kurama_Kick///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Kick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Skill/Kurama/Kurama_Kick.dat"))))
		return E_FAIL;

	////////////CheckPoint///////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_CheckPoint"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Deco/CheckPoint/CheckPoint.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Check_Point"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skill/Masking/CheckPoint.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Line"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Deco/Line/Line.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Line_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Deco/Line/Line_2.dat"))))
		return E_FAIL;


	m_strLoadingText = TEXT("이펙트(을) 로딩 중 입니다.");
	///////////Particle///////////
	/* For.Prototype_Component_VIBuffer_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Navi_Map_Stadium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navi_Map_Stadium"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navi_Map_Stadium.dat")))))
		return E_FAIL;



	m_strLoadingText = TEXT("충돌체를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"),VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPoint_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Trail.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_Effect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
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
	/* For.Prototype_GameObject_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Point"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Trail_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail_Line"),
		CTrail_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Effect_Mesh */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Mesh"),
		CEffect_Mesh::Create(m_pDevice, m_pContext))))
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
	/* For.Prototype_GameObject_UI_Monster_Hp*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster_Hp"),
		CUI_Monster_Status::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_System*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_System"),
		CUI_System::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CUI_Boss_Status*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CUI_Boss_Status"),
		CUI_Boss_Status::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
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

HRESULT CLoader::Loading_For_BossStageLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	////////////Map//////////////
	///* For.Prototype_Component_Map_Stadium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Map_Konaha_Village"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Models/NonAnim/Map/All_Maps/Konaha_Village_N.dat"))))
		return E_FAIL;

	///////////Boss///////////
	///* For.Prototype_Component_Boss_Kurama*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Boss_Kurama"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Models/Anim/Monster/Boss_Kurama/Boss_Kurama.dat"))))
		return E_FAIL;


	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");
	////////////Map//////////////
	/* For.Prototype_Component_Navi_Map_Stadium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Navi_Map_Konoha_Village"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navi_Map_Konoha_Village.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");
	// /* For.Prototype_GameObject_Boss_Kurama */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Kurama"),
		CBoss_Kurama::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Boss_Kurama"),
		CBody_Boss_Kurama::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EventTrigger"),
		CEventTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	
	m_strLoadingText = TEXT("스킬를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Skill_Kurama_Scratch */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Scratch"),
		CKurama_Scratch::Create(m_pDevice, m_pContext))))
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

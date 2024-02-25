#include "stdafx.h"
#include "Body_Player_Custom_Lower.h"
#include "GameInstance.h"
#include "Player_Custom.h"

CBody_Player_Custom_Lower::CBody_Player_Custom_Lower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject(pDevice, pContext)
{
}

CBody_Player_Custom_Lower::CBody_Player_Custom_Lower(const CBody_Player_Custom_Lower& rhs)
	:CPartObject(rhs)
{
}

HRESULT CBody_Player_Custom_Lower::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player_Custom_Lower::Initialize(void* pArg)
{
	BODY_PLAYER_DESC* pBodyPlayerDesc = (BODY_PLAYER_DESC*)pArg;

	m_pPlayerState = pBodyPlayerDesc->pPlayerState;
	m_pParentTransform = pBodyPlayerDesc->pParentTransform;
	m_pPlayerGravity = pBodyPlayerDesc->pPlayerGravity;
	m_pPlayerGForcePerSec = pBodyPlayerDesc->pPlayerGForcePersec;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pBodyPlayerDesc->Current_LEVEL)))
		return E_FAIL;

	if (FAILED(Add_Models(pBodyPlayerDesc->Current_LEVEL)))
		return E_FAIL;

	for (_int i = 0; i < m_Num_Models; i++)
	{
		m_LowerModels[i]->Set_OffRootAnim(true);
		m_LowerModels[i]->Set_Animation(139, 0.f, true, 1.0f, false, false);

	}

	if (pBodyPlayerDesc->Current_LEVEL != LEVEL_CUSTOMROOM) {
		m_CurrentModel_Index = g_iPartsIndex[2];
		m_pCurrentModel = m_LowerModels[m_CurrentModel_Index];
	}
	else
		m_pCurrentModel = m_LowerModels[0];


	return S_OK;
}

void CBody_Player_Custom_Lower::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Player_Custom_Lower::Tick(_float fTimeDelta)
{
}

void CBody_Player_Custom_Lower::Late_Tick(_float fTimeDelta)
{
	Set_Animation_State(fTimeDelta);
	m_LowerModels[m_CurrentModel_Index]->Play_Animation(fTimeDelta, m_fAnimationPlayRatio);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CBody_Player_Custom_Lower::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_LowerModels[m_CurrentModel_Index]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pCurrentModel->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pCurrentModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pCurrentModel->Render(i)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_uint Index = { 0 };

		if (FAILED(m_pCurrentModel->Bind_Material_ShaderResource(m_pShaderOutLine, i, 1, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pCurrentModel->Bind_Material_ShaderResource(m_pShaderCom, i, 6, "g_NormalTexture")))
			Index = 0;
		else
			Index = 1;

		if (FAILED(m_pCurrentModel->Bind_BoneMatrices(m_pShaderOutLine, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderOutLine->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pCurrentModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_Player_Custom_Lower::Set_Animation_State(_float fTimeDelta)
{
	if (*m_pPlayerState & PLAYER_STATE_IDLE) {
		m_pCurrentModel->Set_Animation(139, 0.1f, true, 1.0f, false, false);
	}
	else if (*m_pPlayerState & PLAYER_STATE_FALL_LOOP) {
		m_pCurrentModel->Set_Animation(128, 0.f, true, 1.0f, true, false);
	}
	else if (*m_pPlayerState & PLAYER_STATE_LAND) {
		m_pCurrentModel->Set_Animation(135, 0.1f, false, 2.5f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_DASH_FRONT) {
		m_pCurrentModel->Set_Animation(118, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_DASH_BACK) {
		m_pCurrentModel->Set_Animation(117, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.0f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_DASH_LEFT) {
		m_pCurrentModel->Set_Animation(119, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.0f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_DASH_RIGHT) {
		m_pCurrentModel->Set_Animation(120, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.0f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_RUN) {
		m_pCurrentModel->Set_Animation(177, 0.1f, true, 1.0f, false, false);
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_1) {
		m_pCurrentModel->Set_Animation(26, 0.05f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_2) {
		m_pCurrentModel->Set_Animation(28, 0.05f, false, 3.5f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_3) {
		m_pCurrentModel->Set_Animation(185, 0.05f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 0.6f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_4) {
		m_pCurrentModel->Set_Animation(20, 0.05f, false, 3.f, false, false);
		m_fAnimationPlayRatio = 0.8f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_STRONG_1) {
		m_pCurrentModel->Set_Animation(32, 0.05f, false, 4.f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_STRONG_2) {
		m_pCurrentModel->Set_Animation(33, 0.05f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_COMBO_STRONG_3) {
		m_pCurrentModel->Set_Animation(34, 0.05f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.8f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_FLAME_BOMB) {
		m_pCurrentModel->Set_Animation(165, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_KAMUI) {
		m_pCurrentModel->Set_Animation(166, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_CHIDORI_CHARGE) {
		m_pCurrentModel->Set_Animation(162, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_CHIDORI_RUN_LOOP) {
		m_pCurrentModel->Set_Animation(164, 0.1f, true, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_CHIDORI_ATTACK) {
		m_pCurrentModel->Set_Animation(159, 0.1f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_WOOD_HAND) {
		m_pCurrentModel->Set_Animation(168, 0.1f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_FLAME_BOMB) {
		m_pCurrentModel->Set_Animation(158, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_CHIDORI_CHARGE) {
		m_pCurrentModel->Set_Animation(155, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_CHIDORI_RUN_LOOP) {
		m_pCurrentModel->Set_Animation(157, 0.1f, true, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_CHIDORI_ATTACK) {
		m_pCurrentModel->Set_Animation(152, 0.1f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_JUMP) {
		m_pCurrentModel->Set_Animation(140, 0.1f, false, 3.0f, false, false);
		m_fAnimationPlayRatio = 0.2f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_JUMP_DOUBLE) {
		m_pCurrentModel->Set_Animation(123, 0.1f, false, 6.0f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_COMBO_1) {
		m_pCurrentModel->Set_Animation(9, 0.1f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.5f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_COMBO_2) {
		m_pCurrentModel->Set_Animation(7, 0.1f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.6f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_COMBO_3) {
		m_pCurrentModel->Set_Animation(13, 0.1f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.9f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_AERIAL_DASH_START) {
		m_pCurrentModel->Set_Animation(2, 0.1f, false, 6.f, false, true, 0);
		m_fAnimationPlayRatio = 1.f;
		*m_pPlayerGravity = -(fTimeDelta * *m_pPlayerGForcePerSec);
	}
	else if (*m_pPlayerState & PLAYER_STATE_CHAKRAJUMP_START) {
		m_pCurrentModel->Set_Animation(115, 0.1f, false, 3.f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_CHAKRAJUMP_LOOP) {
		m_pCurrentModel->Set_Animation(114, 0.1f, true, 1.5f, false, false);
	}
	else if (*m_pPlayerState & PLAYER_STATE_STRUCK_LEFT) {
		m_pCurrentModel->Set_Animation(80, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_STATE_STRUCK_RIGHT) {
		m_pCurrentModel->Set_Animation(88, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_THROW) {
		m_pCurrentModel->Set_Animation(38, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_BEATEN_START) {
		m_pCurrentModel->Set_Animation(95, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_BEATEN_END) {
		m_pCurrentModel->Set_Animation(93, 0.05f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_GET_UP) {
		m_pCurrentModel->Set_Animation(130, 0.05f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_pPlayerState & PLAYER_APPEAR) {
		m_pCurrentModel->Set_Animation(127, 0.05f, false, 1.5f, true, false);
		m_fAnimationPlayRatio = 1.f;
		}
	else if (*m_pPlayerState & PLAYER_EXIT) {
			m_pCurrentModel->Set_Animation(169, 0.05f, false, 2.5f, true, false);
			m_fAnimationPlayRatio = 1.f;
			}
}

void CBody_Player_Custom_Lower::Set_Model(_bool UpDown)
{
	if (UpDown)
	{
		m_CurrentModel_Index++;

		if (m_CurrentModel_Index >= m_Num_Models)
			m_CurrentModel_Index = 0;
	}
	else
	{
		m_CurrentModel_Index--;

		if (m_CurrentModel_Index < 0)
			m_CurrentModel_Index = m_Num_Models - 1;
	}
	g_iPartsIndex[2] = m_CurrentModel_Index;
	m_pCurrentModel = m_LowerModels[m_CurrentModel_Index];
}

CModel* CBody_Player_Custom_Lower::Get_CurrentModel()
{
	return __super ::Get_CurrentModel();
}

HRESULT CBody_Player_Custom_Lower::Add_Component(LEVEL Current_LEVEL)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh_OutLine"),
		TEXT("Com_Shader_OutLine"), reinterpret_cast<CComponent**>(&m_pShaderOutLine))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player_Custom_Lower::Add_Models(LEVEL Current_LEVEL)
{
	CModel* m_pModel_PantsCuffed = { nullptr };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Lower_PantsCuffed"),
		TEXT("Com_Model_Lower_PantsCuffed"), reinterpret_cast<CComponent**>(&m_pModel_PantsCuffed))))
		return E_FAIL;
	m_LowerModels.push_back(m_pModel_PantsCuffed);
	m_Num_Models++;

	CModel* m_pModel_PantsCut = { nullptr };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_Custom_Lower_PantsCut"),
		TEXT("Com_Model_Lower_PantsCut"), reinterpret_cast<CComponent**>(&m_pModel_PantsCut))))
		return E_FAIL;
	m_LowerModels.push_back(m_pModel_PantsCut);
	m_Num_Models++;

	return S_OK;
}

HRESULT CBody_Player_Custom_Lower::Bind_ShaderResources()
{
	_float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
	_float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
	_float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CBody_Player_Custom_Lower* CBody_Player_Custom_Lower::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player_Custom_Lower* pInstance = new CBody_Player_Custom_Lower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player_Custom_Lower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player_Custom_Lower::Clone(void* pArg)
{
	CBody_Player_Custom_Lower* pInstance = new CBody_Player_Custom_Lower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player_Custom_Lower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player_Custom_Lower::Free()
{
	for (auto pModel : m_LowerModels)
		Safe_Release(pModel);
	m_LowerModels.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderOutLine);

	__super::Free();
}

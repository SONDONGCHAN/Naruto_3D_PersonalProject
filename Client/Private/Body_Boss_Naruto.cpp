#include "Body_Boss_Naruto.h"
#include "Boss_Naruto.h"

CBody_Boss_Naruto::CBody_Boss_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

CBody_Boss_Naruto::CBody_Boss_Naruto(const CBody_Boss_Naruto& rhs)
    :CPartObject(rhs)
{
}

HRESULT CBody_Boss_Naruto::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Boss_Naruto::Initialize(void* pArg)
{
    BODY_MONSTER_DESC* pBosyMonsterDesc = (BODY_MONSTER_DESC*)pArg;
    m_iMonsterState = pBosyMonsterDesc->pState;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pModelCom->Set_OffRootAnim(true);
    m_pModelCom->Set_Animation(135, 0.1f, true, 1.0f, false, false);

    return S_OK;
}

void CBody_Boss_Naruto::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Boss_Naruto::Tick(_float fTimeDelta)
{
}

void CBody_Boss_Naruto::Late_Tick(_float fTimeDelta)
{
    Set_Animation_State(fTimeDelta);
    m_pModelCom->Play_Animation(fTimeDelta, m_fAnimationPlayRatio);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CBody_Boss_Naruto::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CBody_Boss_Naruto::Set_Animation_State(_float fTimeDelta)
{
	if (*m_iMonsterState & PLAYER_STATE_IDLE) {
		m_pModelCom->Set_Animation(135, 0.1f, true, 1.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_FALL_LOOP) {
		m_pModelCom->Set_Animation(118, 0.f, true, 1.0f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_LAND) {
		m_pModelCom->Set_Animation(138, 0.1f, false, 2.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_DASH_FRONT) {
		m_pModelCom->Set_Animation(101, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_DASH_BACK) {
		m_pModelCom->Set_Animation(102, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_DASH_LEFT) {
		m_pModelCom->Set_Animation(104, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_DASH_RIGHT) {
		m_pModelCom->Set_Animation(105, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RUN) {
		m_pModelCom->Set_Animation(169, 0.1f, true, 1.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_1) {
		m_pModelCom->Set_Animation(32, 0.05f, false, 4.f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_2) {
		m_pModelCom->Set_Animation(33, 0.05f, false, 4.f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_3) {
		m_pModelCom->Set_Animation(23, 0.05f, false, 4.f, false, false);
		m_fAnimationPlayRatio = 0.8f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_4) {
		m_pModelCom->Set_Animation(25, 0.05f, false, 3.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_STRONG_1) {
		m_pModelCom->Set_Animation(31, 0.05f, false, 4.f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_STRONG_2) {
		m_pModelCom->Set_Animation(29, 0.05f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 0.6f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_COMBO_STRONG_3) {
		m_pModelCom->Set_Animation(30, 0.05f, false, 4.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RASENGUN_CHARGE) {
		m_pModelCom->Set_Animation(152, 0.1f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RASENGUN_RUN_LOOP) {
		m_pModelCom->Set_Animation(155, 0.1f, true, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RASENGUN_ATTACK) {
		m_pModelCom->Set_Animation(150, 0.1f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RASENSHURIKEN) {
		m_pModelCom->Set_Animation(160, 0.1f, false, 2.0f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_RASENSGUN_SUPER) {
		m_pModelCom->Set_Animation(149, 0.1f, false, 1.3f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_JUMP) {
		m_pModelCom->Set_Animation(136, 0.1f, false, 3.0f, false, false);
		m_fAnimationPlayRatio = 0.2f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_JUMP_DOUBLE) {
		m_pModelCom->Set_Animation(108, 0.1f, false, 6.0f, false, false);
		m_fAnimationPlayRatio = 0.5f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_CHAKRAJUMP_START) {
		m_pModelCom->Set_Animation(99, 0.1f, false, 3.f, false, false);
		m_fAnimationPlayRatio = 0.7f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_CHAKRAJUMP_LOOP) {
		m_pModelCom->Set_Animation(98, 0.1f, true, 3.f, false, false);
	}
	else if (*m_iMonsterState & PLAYER_STATE_STRUCK_LEFT) {
		m_pModelCom->Set_Animation(73, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STATE_STRUCK_RIGHT) {
		m_pModelCom->Set_Animation(81, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_STRUCK_STRONG_LEFT) {
		m_pModelCom->Set_Animation(76, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
		}
	else if (*m_iMonsterState & PLAYER_STRUCK_STRONG_RIGHT) {
		m_pModelCom->Set_Animation(74, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
		}
	else if (*m_iMonsterState & PLAYER_THROW) {
		m_pModelCom->Set_Animation(34, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_BEATEN_START) {
		m_pModelCom->Set_Animation(88, 0.05f, false, 2.f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_BEATEN_END) {
		m_pModelCom->Set_Animation(86, 0.05f, false, 2.f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_GET_UP) {
		m_pModelCom->Set_Animation(120, 0.05f, false, 1.5f, false, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_APPEAR) {
		m_pModelCom->Set_Animation(112, 0.05f, false, 1.5f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_EXIT) {
		m_pModelCom->Set_Animation(113, 0.05f, false, 1.5f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_ELECTRICSHOCK) {
		m_pModelCom->Set_Animation(63, 0.05f, false, 0.8f, true, false);
		m_fAnimationPlayRatio = 1.f;
	}
	else if (*m_iMonsterState & PLAYER_DEAD) {
		m_pModelCom->Set_Animation(77, 0.05f, false, 1.5f, true, false);
	}

	
}

HRESULT CBody_Boss_Naruto::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_Naruto"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Boss_Naruto::Bind_ShaderResources()
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

	return S_OK;
}

CBody_Boss_Naruto* CBody_Boss_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Boss_Naruto* pInstance = new CBody_Boss_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Boss_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Boss_Naruto::Clone(void* pArg)
{
	CBody_Boss_Naruto* pInstance = new CBody_Boss_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Boss_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Boss_Naruto::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	__super::Free();
}

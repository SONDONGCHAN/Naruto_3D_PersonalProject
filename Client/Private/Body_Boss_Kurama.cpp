#include "Body_Boss_Kurama.h"
#include "Boss_Kurama.h"

CBody_Boss_Kurama::CBody_Boss_Kurama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

CBody_Boss_Kurama::CBody_Boss_Kurama(const CBody_Boss_Kurama& rhs)
    :CPartObject(rhs)
{
}

HRESULT CBody_Boss_Kurama::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Boss_Kurama::Initialize(void* pArg)
{
    BODY_MONSTER_DESC* pBosyMonsterDesc = (BODY_MONSTER_DESC*)pArg;
    m_iMonsterState = pBosyMonsterDesc->pState;
    
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    if (FAILED(Add_Component()))
        return E_FAIL;
    
    m_pModelCom->Set_OffRootAnim(true);
    m_pModelCom->Set_Animation(21, 0.1f, true, 1.0f, false, false);
    
    return S_OK;
}

void CBody_Boss_Kurama::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Boss_Kurama::Tick(_float fTimeDelta)
{
}

void CBody_Boss_Kurama::Late_Tick(_float fTimeDelta)
{
    Set_Animation_State(fTimeDelta);
    m_pModelCom->Play_Animation(fTimeDelta, m_fAnimationPlayRatio);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CBody_Boss_Kurama::Render()
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

void CBody_Boss_Kurama::Set_Animation_State(_float fTimeDelta)
{
    if (*m_iMonsterState & BOSS_IDLE) {
        m_pModelCom->Set_Animation(21, 0.1f, true, 1.0f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_RUN) {
        m_pModelCom->Set_Animation(31, 0.1f, true, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_DASH_LEFT) {
        m_pModelCom->Set_Animation(22, 0.1f, false, 2.0f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_DASH) {
        m_pModelCom->Set_Animation(5, 0.1f, false, 7.0f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_APPEAR) {
        m_pModelCom->Set_Animation(12, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_APPEAR2) {
        m_pModelCom->Set_Animation(18, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_DEAD) {
        m_pModelCom->Set_Animation(10, 0.1f, false, 1.5f, true, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_DEAD_LOOP) {
        m_pModelCom->Set_Animation(11, 0.1f, true, 1.0f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_SCRATCH) {
        m_pModelCom->Set_Animation(0, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_KICK) {
        m_pModelCom->Set_Animation(1, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_BITE) {
        m_pModelCom->Set_Animation(3, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_RUSH_ATTACK) {
        m_pModelCom->Set_Animation(4, 0.1f, false, 1.2f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_SCRATCH_FAR) {
        m_pModelCom->Set_Animation(6, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_LASER) {
        m_pModelCom->Set_Animation(29, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_LASER_AERIAL) {
        m_pModelCom->Set_Animation(26, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_ATTACK_FIREBALL) {
        m_pModelCom->Set_Animation(27, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }
    else if (*m_iMonsterState & BOSS_PHASECHANGE) {
        m_pModelCom->Set_Animation(28, 0.1f, false, 1.5f, false, false);
        m_fAnimationPlayRatio = 1.f;
    }

}

HRESULT CBody_Boss_Kurama::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Boss_Kurama"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Boss_Kurama::Bind_ShaderResources()
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

	return S_OK;
}

CBody_Boss_Kurama* CBody_Boss_Kurama::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Boss_Kurama* pInstance = new CBody_Boss_Kurama(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_Boss_Kurama");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Boss_Kurama::Clone(void* pArg)
{
    CBody_Boss_Kurama* pInstance = new CBody_Boss_Kurama(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody_Boss_Kurama");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Boss_Kurama::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}

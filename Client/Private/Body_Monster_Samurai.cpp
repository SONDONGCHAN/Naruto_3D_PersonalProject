#include "stdafx.h"
#include "Body_Monster_Samurai.h"
#include "GameInstance.h"
CBody_Monster_Samurai::CBody_Monster_Samurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

CBody_Monster_Samurai::CBody_Monster_Samurai(const CBody_Monster_Samurai& rhs)
    :CPartObject(rhs)
{
}

HRESULT CBody_Monster_Samurai::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Monster_Samurai::Initialize(void* pArg)
{
    BODY_MONSTER_DESC* pBosyMonsterDesc = (BODY_MONSTER_DESC*)pArg;
    m_iMonsterState = pBosyMonsterDesc->pState;


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pModelCom->Set_OffRootAnim(true);
    m_pModelCom->Set_Animation(103, 0.1f, true, 1.0f, false, false);

    return S_OK;
}

void CBody_Monster_Samurai::Priority_Tick(_float fTimeDelta)
{

}

void CBody_Monster_Samurai::Tick(_float fTimeDelta)
{

}

void CBody_Monster_Samurai::Late_Tick(_float fTimeDelta)
{
    Set_Animation_State(fTimeDelta);
    m_pModelCom->Play_Animation(fTimeDelta, m_fAnimationPlayRatio);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CBody_Monster_Samurai::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    
    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderOutLine, i, 1, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderOutLine, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderOutLine->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        _uint Index = { 0 };

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 6, "g_NormalTexture")))
            Index = 0;
        else
            Index = 1;

    
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
    
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    
    return S_OK;
}

void CBody_Monster_Samurai::Set_Animation_State(_float fTimeDelta)
{

    if (*m_iMonsterState & MONSTER_IDLE) {
        m_pModelCom->Set_Animation(103, 0.1f, true, 1.0f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_RUN) {
        m_pModelCom->Set_Animation(110, 0.1f, true, 1.0f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_RUSH_ATTACK_PUNCH) {
        m_pModelCom->Set_Animation(107, 0.1f, false, 2.f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_RUSH_ATTACK_KICK) {
        m_pModelCom->Set_Animation(16, 0.1f, false, 2.f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_COMBO_ATTACK_1) {
        m_pModelCom->Set_Animation(18, 0.1f, false, 3.f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_COMBO_ATTACK_2) {
        m_pModelCom->Set_Animation(16, 0.1f, false, 3.f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_HANDSEAL) {
        m_pModelCom->Set_Animation(106, 0.1f, false, 1.2f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_STRUCK_LEFT) {
        m_pModelCom->Set_Animation(68, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_STRUCK_RIGHT) {
        m_pModelCom->Set_Animation(76, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_STRUCK_STRONG_LEFT) {
        m_pModelCom->Set_Animation(71, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_STRUCK_STRONG_RIGHT) {
        m_pModelCom->Set_Animation(69, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_THROW) {
        m_pModelCom->Set_Animation(20, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_BEATEN_START) {
        m_pModelCom->Set_Animation(83, 0.05f, false, 2.f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_BEATEN_END) {
        m_pModelCom->Set_Animation(81, 0.05f, false, 2.f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_ELECTRICSHOCK) {
        m_pModelCom->Set_Animation(57, 0.05f, false, 0.8f, true, false);
    }
    else if (*m_iMonsterState & MONSTER_GET_UP) {
        m_pModelCom->Set_Animation(101, 0.05f, false, 1.5f, false, false);
    }
    else if (*m_iMonsterState & MONSTER_DEAD) {
        m_pModelCom->Set_Animation(72, 0.05f, false, 1.5f, true, false);
    }
}

HRESULT CBody_Monster_Samurai::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh_OutLine"),
        TEXT("Com_Shader_OutLine"), reinterpret_cast<CComponent**>(&m_pShaderOutLine))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Body_Monster_Samurai"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Monster_Samurai::Bind_ShaderResources()
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
    //    return E_FAIL;

    if (FAILED(m_pShaderOutLine->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

CBody_Monster_Samurai* CBody_Monster_Samurai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Monster_Samurai* pInstance = new CBody_Monster_Samurai(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_Monster_Samurai");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Monster_Samurai::Clone(void* pArg)
{
    CBody_Monster_Samurai* pInstance = new CBody_Monster_Samurai(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody_Monster_Samurai");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Monster_Samurai::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pShaderOutLine);

    __super::Free();
}

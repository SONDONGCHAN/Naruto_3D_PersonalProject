#include "stdafx.h"
#include "Wood_Dragon.h"
#include "GameInstance.h"

CWood_Dragon::CWood_Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CWood_Dragon::CWood_Dragon(const CWood_Dragon& rhs)
    :CSkill(rhs)
{
}

HRESULT CWood_Dragon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWood_Dragon::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(0, 0.f, true, 2.f, false, false);

    m_fSkill_Power = 5.f;

    return S_OK;
}

void CWood_Dragon::Priority_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta, 1.f);
   // RootAnimation();
}

void CWood_Dragon::Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
}

void CWood_Dragon::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (myState == STATE_HIT)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
}

HRESULT CWood_Dragon::Render()
{
    if (myState == STATE_HIT)
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
    }

    return S_OK;
}

void CWood_Dragon::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
} 

void CWood_Dragon::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CWood_Dragon::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CWood_Dragon::State_Control(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
        // 이펙트 채워넣기
    }
    else if (myState == STATE_HIT)
    {
        m_fDurTime += fTimeDelta;
        
        m_pTransformCom->Go_Straight_Custom(fTimeDelta, 7.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        
        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Wood_Dragon_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"Wood_Dragon_Collider");
        
        if (m_fDurTime > 4.f)
            Set_Next_State();
        
        // 이펙트 채워넣기
    }
}

void CWood_Dragon::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (WOOD_DRAGON_STATE)((_uint)myState + 1);

    if (myState == STATE_HIT)
    {
        m_pGameInstance->PlaySoundW("Wood_Dragon_Fear", SOUND_SKILL, 1.f, true);
        m_pColliderMain->Set_Extents_OBB({1.5f, 1.5f, 8.f});
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Extents_OBB({0.f, 0.f, 0.f});
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Wood_Dragon_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"Wood_Dragon_Collider");
        m_fDurTime = 0;
        m_bIsHit = false;
    }
}

void CWood_Dragon::Set_State()
{
    myState = STATE_MAKING;
    m_pGameInstance->PlaySoundW("Wood_Dragon_Human", SOUND_SKILL, 1.f, true);

}

void CWood_Dragon::RootAnimation()
{
    // 움직임을 World이동으로치환
    _float3 fmovevalue = m_pModelCom->Get_FramePos();
    _float3 fInversemovevalue;
    fInversemovevalue.x = -fmovevalue.y;
    fInversemovevalue.y = -fmovevalue.z;
    fInversemovevalue.z = -fmovevalue.x;
    m_pTransformCom->SetAnimationMove(XMLoadFloat3(&fInversemovevalue));
}

void CWood_Dragon::Particles_Priority_Tick(_float fTimeDelta)
{
}

void CWood_Dragon::Particles_Tick(_float fTimeDelta)
{
}

void CWood_Dragon::Particles_Late_Tick(_float fTimeDelta)
{
}


HRESULT CWood_Dragon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Skill_Wood_Dragon"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::OBB_DESC		BoundingDesc{};
    BoundingDesc.vExtents = { 2.f, 2.f, 6.f };
    BoundingDesc.vRadians = { 0.f, 0.f, 0.f};
    BoundingDesc.vCenter = _float3(0.f, 0.f, -4.f);
    if (FAILED( __super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Wood_Dragon_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CWood_Dragon::Bind_ShaderResources()
{
    _float4x4       WorldMatrix;
    XMStoreFloat4x4( &WorldMatrix,m_pTransformCom->Get_WorldMatrix());
    _float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
    _float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
    _float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
    //    return E_FAIL;


    return S_OK;
}

CWood_Dragon* CWood_Dragon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWood_Dragon* pInstance = new CWood_Dragon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWood_Dragon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWood_Dragon::Clone(void* pArg)
{
    CWood_Dragon* pInstance = new CWood_Dragon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWood_Dragon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWood_Dragon::Free()
{
    Safe_Release(m_pColliderMain);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}

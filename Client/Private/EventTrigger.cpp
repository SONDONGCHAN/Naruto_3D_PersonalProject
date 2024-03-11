#include "EventTrigger.h"
#include "Boss_Kurama.h"
#include "Monster_LeafNinja.h"
#include "Monster_Samurai.h"






CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
    :CGameObject(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{
    EVENTTRIGGER_DESC* pTriggerDesc = (EVENTTRIGGER_DESC*)pArg;

    m_MyDesc = *pTriggerDesc;

    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_MyDesc.vPos);
    m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

    if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_1)
        m_bVitalize = true;

    return S_OK;
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{   
    if (m_bVitalize)
    {
        if (!m_bUsed)
        {
            m_pGameInstance->Check_Collision_For_MyEvent(m_MyDesc.Current_Level, m_pColliderMain, L"Player_Main_Collider");
        }
        m_CheckPoint_Particles->Priority_Tick(fTimeDelta);
    }
}

void CEventTrigger::Tick(_float fTimeDelta)
{
    if (m_bVitalize)
    {
        m_Effect_CheckPoint->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_CheckPoint->Tick(fTimeDelta);

        m_Effect_Line->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_Line->Tick(fTimeDelta);


        if (m_bUsed)
        {
            m_CheckPoint_Particles->Tick(fTimeDelta);
            m_Effect_CheckPoint->Scale_Change(fTimeDelta);
            m_Effect_Line->Scale_Change(fTimeDelta);
        }
    }
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{
    if (m_bVitalize)
    {
        if (m_bUsed)
            m_CheckPoint_Particles->Late_Tick(fTimeDelta);

        m_Effect_CheckPoint->Late_Tick(fTimeDelta);
        m_Effect_Line->Late_Tick(fTimeDelta);

        if (!m_bUsed)
        {
#ifdef _DEBUG
            m_pGameInstance->Add_DebugComponent(m_pColliderMain);
#endif
        }
    }
}

HRESULT CEventTrigger::Render()
{
    return S_OK;
}

HRESULT CEventTrigger::Add_Component()
{
    CBounding_Sphere::SPHERE_DESC		TriggerBoundingDesc{};
    TriggerBoundingDesc.fRadius = 3.f;
    TriggerBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &TriggerBoundingDesc)))
        return E_FAIL;
    m_pGameInstance->Add_Collider(m_MyDesc.Current_Level, L"Trigger_Collider", m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);
    
    return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{
    return S_OK;
}

HRESULT CEventTrigger::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_CHECKPOINT;
    Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_END;
    Effect_Desc_1.MySpinOption = CEffect_Mesh::SPIN_Y;
    Effect_Desc_1.vMyScale = _vector{ 12.f, 8.f, 12.f, 1.f };
    m_Effect_CheckPoint = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_CheckPoint)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_LINE;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_END;
    Effect_Desc_2.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_2.vMyScale = _vector{ 30.f, 1000.f, 30.f, 1.f };
    m_Effect_Line = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_Line)
        return E_FAIL;

    return S_OK;
}

HRESULT CEventTrigger::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc1{};
    InstanceDesc1.iNumInstance = 300;
    InstanceDesc1.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vRange = _float3(6.f, 3.f, 6.f);
    InstanceDesc1.vSize = _float2(0.04f, 0.05f);
    InstanceDesc1.vSpeed = _float2(1.f, 1.5f);
    InstanceDesc1.vLifeTime = _float2(2.f, 2.5f);
    InstanceDesc1.isLoop = false;
    InstanceDesc1.vColor = _float4(80.f/255.f, 180.f/255.f, 1.f, 0.7f);
    InstanceDesc1.fDuration = 2.6f;
    InstanceDesc1.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc1.MyOption_Shape = CVIBuffer_Instancing::SHAPE_RECTANGLE;
    InstanceDesc1.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc1.MyOption_Size = CVIBuffer_Instancing::SIZE_DIMINISH;
    InstanceDesc1.strTextureTag = L"Prototype_Component_Texture_Circle_Noise";
    InstanceDesc1.vSpriteRatio = _float2(1.f, 1.f);
    
    m_CheckPoint_Particles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc1));
    if (nullptr == m_CheckPoint_Particles)
        return E_FAIL;

    return S_OK;
}

void CEventTrigger::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
    m_CheckPoint_Particles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pGameInstance->PlaySoundW("Check_Point", SOUND_ETC_2, 1.f, true);

    if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_1)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 1))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_2)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 2))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_3)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 3))->Set_Vitalize();

        dynamic_cast<CMonster_LeafNinja*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_Monster", 0))->Set_Vitalize();
        dynamic_cast<CMonster_Samurai*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_Monster", 1))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_4)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 4))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_5)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 5))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_6)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 6))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_7)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 7))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_CHECK_POINT_8)
    {
        m_bUsed = true;
        dynamic_cast<CEventTrigger*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, L"Layer_EventTrigger", 8))->Set_Vitalize();
    }
    else if (m_MyDesc.Trigger_Type == TRIGGER_BOSS)
    {
        m_bUsed = true;
        m_pGameInstance->StopSound(SOUND_BGM);
        dynamic_cast<CBoss_Kurama*>(m_pGameInstance->Get_GameObject(m_MyDesc.Current_Level, TEXT("Layer_Monster"), 2))->Set_Appear();
    }

}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEventTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
    CEventTrigger* pInstance = new CEventTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEventTrigger");
        Safe_Release(pInstance);
        
    }

    return pInstance;
}

void CEventTrigger::Free()
{
    Safe_Release(m_pColliderMain);
    Safe_Release(m_Effect_CheckPoint);
    Safe_Release(m_Effect_Line);
    Safe_Release(m_CheckPoint_Particles);

    __super::Free();
}

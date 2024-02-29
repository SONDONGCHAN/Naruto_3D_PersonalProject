#include "stdafx.h"
#include "FlameBomb.h"

CFlameBomb::CFlameBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CFlameBomb::CFlameBomb(const CFlameBomb& rhs)
    :CSkill(rhs)
{
}

HRESULT CFlameBomb::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFlameBomb::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;

    return S_OK;
}

void CFlameBomb::Priority_Tick(_float fTimeDelta)
{
}

void CFlameBomb::Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
    m_Effect_Fireball_Main->State_Tick(m_pTransformCom->Get_WorldMatrix());
    m_Effect_Fireball_Ring-> State_Tick(m_pTransformCom->Get_WorldMatrix());
    m_Effect_Fireball_Main->Tick(fTimeDelta);
    m_Effect_Fireball_Ring->Tick(fTimeDelta);

}

void CFlameBomb::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
        m_Effect_Fireball_Ring->Late_Tick(fTimeDelta);

    if ( myState == STATE_DETECTING )
        m_Effect_Fireball_Main->Late_Tick(fTimeDelta);

#ifdef _DEBUG
    if ( myState == STATE_DETECTING || myState == STATE_HIT)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
}

HRESULT CFlameBomb::Render()
{
    return S_OK;
}

void CFlameBomb::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
    if (m_User_Type == USER_PLAYER)
    {
        if (strColliderLayerTag == L"Monster_Main_Collider")
        {
            if (myState == STATE_DETECTING)
            {
                m_bIsHit = true;
                Set_Next_State();
            }
        }
    }

    else if (m_User_Type == USER_MONSTER)
    {
        if (strColliderLayerTag == L"Player_Main_Collider")
        {
            if (myState == STATE_DETECTING)
            {
                m_bIsHit = true;
                Set_Next_State();
            }
        }
    }
}

void CFlameBomb::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CFlameBomb::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CFlameBomb::State_Control(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
        _matrix Mat = m_pParentTransform->Get_WorldMatrix();
        m_Effect_Fireball_Ring->Get_StartMat(Mat);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        // 이펙트 채워넣기
    }
    else if (myState == STATE_DETECTING)
    {
        m_fDurTime += fTimeDelta;

        if (m_bTargeting)
        {
            m_pTransformCom->MoveTo(m_vTarget_Pos, 20.f, fTimeDelta);

            if (XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_vTarget_Pos)) < 0.4f)
            {
                m_bIsHit = true;
                Set_Next_State();
            }
        }
        else
            m_pTransformCom->Go_Straight_Custom(fTimeDelta, 20.f);

        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider");

        if (m_fDurTime > 1.f)
        {
            m_bIsHit = true;
            Set_Next_State();
        }

        // 이펙트 채워넣기
    }
    else if (myState == STATE_HIT)
    {
        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

        m_fDurTime += fTimeDelta;

        if (m_fDurTime > 0.1f)
            Set_Next_State();

        // 이펙트 채워넣기
    }
}

void CFlameBomb::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (FLAMEBOMB_STATE)((_uint)myState + 1);

    if (myState == STATE_DETECTING)
    {
        m_pColliderMain->Set_Radius(1.f);
    }

    else if (myState == STATE_HIT)
    {
        m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
        m_pColliderMain->Set_Radius(3.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
        m_BasicParticles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

        m_fDurTime = 0;
        m_bTargeting = false;
        m_bIsHit = false;
    }
}

void CFlameBomb::Set_State()
{
    myState = STATE_MAKING;
}

void CFlameBomb::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

void CFlameBomb::Particles_Priority_Tick(_float fTimeDelta)
{
    m_BasicParticles->Priority_Tick(fTimeDelta);
}

void CFlameBomb::Particles_Tick(_float fTimeDelta)
{
    m_BasicParticles->Tick(fTimeDelta);
}

void CFlameBomb::Particles_Late_Tick(_float fTimeDelta)
{
    m_BasicParticles->Late_Tick(fTimeDelta);
}

HRESULT CFlameBomb::Add_Components()
{
    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("FlameBomb_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CFlameBomb::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_FIREBALL_MAIN;
    Effect_Desc_1.vMyScale = _vector{0.5f, 0.5f, 0.5f, 1.f};
    m_Effect_Fireball_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_Fireball_Main)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_FIREBALL_RING;
    Effect_Desc_2.vMyScale = _vector{ 0.5f, 0.5f, 0.5f, 1.f };
    m_Effect_Fireball_Ring = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_Fireball_Ring)
        return E_FAIL;

    return S_OK;
}

HRESULT CFlameBomb::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
    InstanceDesc.iNumInstance = 70;
    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
    InstanceDesc.vSize = _float2(0.08f, 0.09f);
    InstanceDesc.vSpeed = _float2(2.5f, 3.5f);
    InstanceDesc.vLifeTime = _float2(0.7f, 1.0f);
    InstanceDesc.isLoop = false;
    InstanceDesc.vColor = _float4(1.f, 210.f/255.f, 0.f, 1.f);
    InstanceDesc.fDuration = 1.3f;
    InstanceDesc.MyOption = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";

    m_BasicParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
    if (nullptr == m_BasicParticles)
        return E_FAIL;

    return S_OK;
}

CFlameBomb* CFlameBomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFlameBomb* pInstance = new CFlameBomb(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CFlameBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFlameBomb::Clone(void* pArg)
{
    CFlameBomb* pInstance = new CFlameBomb(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CFlameBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFlameBomb::Free()
{
    Safe_Release(m_pColliderMain);
    Safe_Release(m_Effect_Fireball_Main);
    Safe_Release(m_Effect_Fireball_Ring);
    Safe_Release(m_BasicParticles);

    __super::Free();
}

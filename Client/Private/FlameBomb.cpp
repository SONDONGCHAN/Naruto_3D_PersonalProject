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
    if (myState == STATE_DETECTING)
    {

    }
}

void CFlameBomb::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    State_Control(fTimeDelta);
    m_Effect_Fireball_Main-> State_Tick(m_pTransformCom->Get_WorldMatrix());

    _matrix RingMat = m_pParentTransform->Get_WorldMatrix();
    _vector vDir = XMVector3Normalize(RingMat.r[2]);
    RingMat.r[3].m128_f32[1] += 1.f;
    RingMat.r[3] += (vDir*0.3f);
    m_Effect_Fireball_Ring-> State_Tick(RingMat);
    m_Effect_Fireball_Main-> Tick(fTimeDelta);
    m_Effect_Fireball_Ring-> Tick(fTimeDelta);

    if (myState == STATE_MAKING)
    {
        m_Effect_Fireball_Ring->Scale_Change(fTimeDelta);
    }
    if (myState == STATE_DETECTING)
    {
        m_Effect_Fireball_Main->Scale_Change(fTimeDelta);
    }
}

void CFlameBomb::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
        m_Effect_Fireball_Ring->Late_Tick(fTimeDelta);

    if (myState == STATE_DETECTING)
    {
        m_Effect_Fireball_Main->Late_Tick(fTimeDelta);
    }
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
        m_TraceParticles->Set_Loop_ON(true);
        m_Effect_Fireball_Main->Start_Trigger();
        m_pColliderMain->Set_Radius(1.f);
    }

    else if (myState == STATE_HIT)
    {
        m_TraceParticles->Set_Loop_ON(false);
        m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
        m_pColliderMain->Set_Radius(3.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
        m_BasicParticles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        m_ExplosionParticles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
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
    m_Effect_Fireball_Ring->Start_Trigger();
}

void CFlameBomb::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

void CFlameBomb::Particles_Priority_Tick(_float fTimeDelta)
{
    m_BasicParticles->Priority_Tick(fTimeDelta);
    m_ExplosionParticles->Priority_Tick(fTimeDelta);
    m_TraceParticles->Priority_Tick(fTimeDelta);
}

void CFlameBomb::Particles_Tick(_float fTimeDelta)
{
    m_BasicParticles->Tick(fTimeDelta);
    m_ExplosionParticles->Tick(fTimeDelta);
    m_TraceParticles->Tick(fTimeDelta);
}

void CFlameBomb::Particles_Late_Tick(_float fTimeDelta)
{
    m_BasicParticles->Late_Tick(fTimeDelta);
    m_ExplosionParticles->Late_Tick(fTimeDelta);
    m_TraceParticles->Late_Tick(fTimeDelta);
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
    Effect_Desc_1.MyType        = CEffect_Mesh::EFFECT_FIREBALL_MAIN;
    Effect_Desc_1.MyUVOption    = CEffect_Mesh::MOVE_X;
    Effect_Desc_1.vMyScale      = _vector{0.5f, 0.5f, 0.5f, 1.f};
    m_Effect_Fireball_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_Fireball_Main)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_FIREBALL_RING;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_Y_INVERSE;
    Effect_Desc_2.vMyScale = _vector{ 0.5f, 0.5f, 0.5f, 1.f };
    m_Effect_Fireball_Ring = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_Fireball_Ring)
        return E_FAIL;

    return S_OK;
}

HRESULT CFlameBomb::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
    InstanceDesc.iNumInstance = 150;
    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
    InstanceDesc.vSize = _float2(0.08f, 0.09f);
    InstanceDesc.vSpeed = _float2(2.5f, 3.5f);
    InstanceDesc.vLifeTime = _float2(0.7f, 1.0f);
    InstanceDesc.isLoop = false;
    InstanceDesc.vColor = _float4(1.f, 210.f/255.f, 0.f, 1.f);
    InstanceDesc.fDuration = 1.3f;
    InstanceDesc.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
    InstanceDesc.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";

    m_BasicParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
    if (nullptr == m_BasicParticles)
        return E_FAIL;


    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc2{};
    InstanceDesc2.iNumInstance = 50;
    InstanceDesc2.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc2.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc2.vRange = _float3(7.f, 7.f, 7.f);
    InstanceDesc2.vSize = _float2(3.f, 4.f);
    InstanceDesc2.vSpeed = _float2(0.2f, 0.4f);
    InstanceDesc2.vLifeTime = _float2(1.5f, 2.5f);
    InstanceDesc2.isLoop = false;
    InstanceDesc2.vColor = _float4(1.f, 120.f / 255.f, 0.f, 1.f);
    InstanceDesc2.fDuration = 2.6f;
    InstanceDesc2.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc2.MyOption_Shape = CVIBuffer_Instancing::SHAPE_SQUARE;
    InstanceDesc2.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_SPRITE;
    InstanceDesc2.strTextureTag = L"Prototype_Component_Texture_FireBall_Explosion";
    InstanceDesc2.vSpriteRatio = _float2(8.f, 8.f);

    m_ExplosionParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc2));
    if (nullptr == m_ExplosionParticles)
        return E_FAIL;


    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc3{};
    InstanceDesc3.iNumInstance = 150;
    InstanceDesc3.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc3.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc3.pCenter = &m_MyPos;
    InstanceDesc3.vRange = _float3(1.f, 1.f, 1.f);
    InstanceDesc3.vSize = _float2(1.5f, 1.5f);
    InstanceDesc3.vSpeed = _float2(0.3f, 0.5f);
    InstanceDesc3.vLifeTime = _float2(0.1f, 2.f);
    InstanceDesc3.isLoop = true;
    InstanceDesc3.vColor = _float4(1.f, 110.f / 255.f, 0.f, 0.7f);
    InstanceDesc3.fDuration = 2.1f;
    InstanceDesc3.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc3.MyOption_Shape = CVIBuffer_Instancing::SHAPE_SQUARE;
    InstanceDesc3.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_SPRITE;
    InstanceDesc3.strTextureTag = L"Prototype_Component_Texture_FireBall_Small";
    InstanceDesc3.vSpriteRatio = _float2(8.f, 8.f);
    
    m_TraceParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc3));
    if (nullptr == m_TraceParticles)
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
    Safe_Release(m_ExplosionParticles);
    Safe_Release(m_TraceParticles);

    __super::Free();
}

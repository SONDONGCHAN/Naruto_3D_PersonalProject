#include "stdafx.h"
#include "RasenShuriken.h"
#include "GameInstance.h"

CRasenShuriken::CRasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CRasenShuriken::CRasenShuriken(const CRasenShuriken& rhs)
    :CSkill(rhs)
{
}

HRESULT CRasenShuriken::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRasenShuriken::Initialize(void* pArg)
{
    SKILL_RASENSHURIKEN_DESC* pRasenShurikenDesc = (SKILL_RASENSHURIKEN_DESC*)pArg;

    m_pSocketMatrix = pRasenShurikenDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_pTransformCom->Set_World(m_WorldMatrix);
    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.0f, 0.4f, 1.f));
    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

    return S_OK;
}

void CRasenShuriken::Priority_Tick(_float fTimeDelta)
{
}

void CRasenShuriken::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (myState == STATE_MAKING || myState == STATE_DETECTING  )
    {
        if (myState == STATE_MAKING)
        {
            _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
            XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
            m_pTransformCom->Set_World(m_WorldMatrix);
        }

        _matrix ParentMakingMat = m_pParentTransform->Get_WorldMatrix();
        _matrix MakingMat = m_pTransformCom->Get_WorldMatrix();
        ParentMakingMat.r[3] = MakingMat.r[3];

        m_Effect_RasenShuriken_Main->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Main->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Main->Scale_Change(fTimeDelta);

        m_Effect_RasenShuriken_Wing->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Wing->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Wing->Scale_Change(fTimeDelta);

        m_Effect_RasenShuriken_Ring->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Ring->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Ring->Scale_Change(fTimeDelta);
    }

    else if (myState == STATE_HIT || myState == STATE_DISSOLVE)
    {
        _matrix ParentMakingMat = m_pParentTransform->Get_WorldMatrix();
        _matrix MakingMat = m_pTransformCom->Get_WorldMatrix();
        ParentMakingMat.r[3] = MakingMat.r[3];
        
        m_Effect_RasenShuriken_Boom->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Boom->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Boom->Scale_Change(fTimeDelta);

       Deco_Control_Tick(fTimeDelta);
    }
    State_Control(fTimeDelta);
}

void CRasenShuriken::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        m_Effect_RasenShuriken_Main->Late_Tick(fTimeDelta);
        m_Effect_RasenShuriken_Wing->Late_Tick(fTimeDelta);
        m_Effect_RasenShuriken_Ring->Late_Tick(fTimeDelta);

    }

    else if (myState == STATE_HIT || myState == STATE_DISSOLVE)
    {
        m_Effect_RasenShuriken_Boom->Late_Tick(fTimeDelta);
        Deco_Control_Late_Tick(fTimeDelta);
    }

    if (myState == STATE_DETECTING || myState == STATE_HIT)
    {

#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
#endif  
    }

}

HRESULT CRasenShuriken::Render()
{
    return S_OK;
}

void CRasenShuriken::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

    if (m_User_Type == USER_MONSTER)
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

void CRasenShuriken::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CRasenShuriken::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CRasenShuriken::State_Control(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
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

        if (m_fDurTime > 1.2f)
        {
            m_bIsHit = true;
            Set_Next_State();
        }
        // 이펙트 채워넣기
    }

    else if (myState == STATE_HIT)
    {
        if (m_fDurTime < 0.01f)
            m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"RasenShuriken_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"RasenShuriken_Collider");

        m_fDurTime += fTimeDelta;

        if (m_fDurTime > 2.f)
            Set_Next_State();
        // 이펙트 채워넣기
    }
    else if (myState == STATE_DISSOLVE)
    {
        m_fDurTime += fTimeDelta;

        if (m_fDurTime > 1.f)
            Set_Next_State();
    }
}

void CRasenShuriken::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (RASENSHURIKEN_STATE)((_uint)myState + 1);

    if (myState == STATE_DETECTING)
    {
        m_pColliderMain->Set_Radius(1.f);
    }
    else if (myState == STATE_HIT)
    {
        m_pColliderMain->Set_Radius(4.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
        m_Effect_RasenShuriken_Boom->Start_Trigger();
        m_Effect_RasenShuriken_Deco_1->Start_Trigger();
        m_Effect_RasenShuriken_Deco_2->Start_Trigger();
        m_Effect_RasenShuriken_Deco_3->Start_Trigger();
        m_Effect_RasenShuriken_Deco_4->Start_Trigger();
        m_Effect_RasenShuriken_Deco_5->Start_Trigger();
        m_Effect_RasenShuriken_Deco_6->Start_Trigger();

        _matrix ParentMakingMat = m_pParentTransform->Get_WorldMatrix();
        _matrix MakingMat = m_pTransformCom->Get_WorldMatrix();
        ParentMakingMat.r[3] = MakingMat.r[3];       
        m_Effect_RasenShuriken_Boom->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_1->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_2->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_3->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_4->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_5->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_6->State_Tick(ParentMakingMat);

        m_BoomParticles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    }
    else if (myState == STATE_DISSOLVE)
    {
        m_DissolveParticles->Trigger(m_pTransformCom->Get_WorldMatrix().r[3]);

        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"RasenShuriken_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"RasenShuriken_Collider");

        m_fDurTime = 0;

        m_Effect_RasenShuriken_Boom->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_1->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_2->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_3->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_4->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_5->End_Trigger(1.f);
        m_Effect_RasenShuriken_Deco_6->End_Trigger(1.f);       
    }
    else if (myState == STATE_FINISH)
    {
        m_fCheckDelay = 0.f;
        m_fDurTime = 0;
        m_bTargeting = false;
        m_bIsHit = false;
    }
}

void CRasenShuriken::Set_State()
{
    myState = STATE_MAKING;
    m_Effect_RasenShuriken_Main->Start_Trigger();
    m_Effect_RasenShuriken_Wing->Start_Trigger();
    m_Effect_RasenShuriken_Ring->Start_Trigger();
}

void CRasenShuriken::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

void CRasenShuriken::Particles_Priority_Tick(_float fTimeDelta)
{
    m_BoomParticles->Priority_Tick(fTimeDelta);
    m_DissolveParticles->Priority_Tick(fTimeDelta);
}

void CRasenShuriken::Particles_Tick(_float fTimeDelta)
{
    m_BoomParticles->Tick(fTimeDelta);
    m_DissolveParticles->Tick(fTimeDelta);
}

void CRasenShuriken::Particles_Late_Tick(_float fTimeDelta)
{
    m_BoomParticles->Late_Tick(fTimeDelta);
    m_DissolveParticles->Late_Tick(fTimeDelta);
}

void CRasenShuriken::Deco_Control_Tick(_float fTimeDelta)
{
     m_fCheckDelay += fTimeDelta;

    _matrix ParentMakingMat = m_pParentTransform->Get_WorldMatrix();
    _matrix MakingMat = m_pTransformCom->Get_WorldMatrix();
    ParentMakingMat.r[3] = MakingMat.r[3];

    m_Effect_RasenShuriken_Deco_1->Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_1->State_Tick(ParentMakingMat);
    m_Effect_RasenShuriken_Deco_1->Scale_Change(fTimeDelta);

    if (m_fCheckDelay > 0.1f)
    {
        m_Effect_RasenShuriken_Deco_2->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Deco_2->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_2->Scale_Change(fTimeDelta);
    }
    if (m_fCheckDelay > 0.2f)
    {
        m_Effect_RasenShuriken_Deco_3->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Deco_3->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_3->Scale_Change(fTimeDelta);
    }
    if (m_fCheckDelay > 0.3f)
    {
        m_Effect_RasenShuriken_Deco_4->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Deco_4->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_4->Scale_Change(fTimeDelta);
    }
    if (m_fCheckDelay > 0.4f)
    {
        m_Effect_RasenShuriken_Deco_5->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Deco_5->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_5->Scale_Change(fTimeDelta);
    }
    if (m_fCheckDelay > 0.5f)
    {
        m_Effect_RasenShuriken_Deco_6->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Deco_6->State_Tick(ParentMakingMat);
        m_Effect_RasenShuriken_Deco_6->Scale_Change(fTimeDelta);
    }
}

void CRasenShuriken::Deco_Control_Late_Tick(_float fTimeDelta)
{
    m_Effect_RasenShuriken_Deco_1->Late_Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_2->Late_Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_3->Late_Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_4->Late_Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_5->Late_Tick(fTimeDelta);
    m_Effect_RasenShuriken_Deco_6->Late_Tick(fTimeDelta);
}

HRESULT CRasenShuriken::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("RasenShuriken_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CRasenShuriken::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_MAIN;
    Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_1.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_1.vMyScale = _vector{ 0.1f, 0.1f, 0.1f, 1.f };
    m_Effect_RasenShuriken_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_RasenShuriken_Main)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_WING;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_2.MySpinOption = CEffect_Mesh::SPIN_Y;
    Effect_Desc_2.vMyScale = _vector{ 3.f, 3.f, 3.f, 1.f };
    m_Effect_RasenShuriken_Wing = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_RasenShuriken_Wing)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_3{};
    Effect_Desc_3.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_RING;
    Effect_Desc_3.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_3.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_3.vMyScale = _vector{ 3.f, 3.f, 3.f, 1.f };
    m_Effect_RasenShuriken_Ring = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_3));
    if (nullptr == m_Effect_RasenShuriken_Ring)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_4{};
    Effect_Desc_4.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_BOOM;
    Effect_Desc_4.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_4.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_4.vMyScale = _vector{ 3.f, 3.f, 3.f, 1.f };
    m_Effect_RasenShuriken_Boom = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_4));
    if (nullptr == m_Effect_RasenShuriken_Boom)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Deco_Desc_1{};
    Deco_Desc_1.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_1.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_1.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_1.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_1 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_1));
    if (nullptr == m_Effect_RasenShuriken_Deco_1)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Deco_Desc_2{};
    Deco_Desc_2.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_2.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_2.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_2.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_2 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_2));
    if (nullptr == m_Effect_RasenShuriken_Deco_2)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Deco_Desc_3{};
    Deco_Desc_3.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_3.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_3.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_3.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_3 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_3));
    if (nullptr == m_Effect_RasenShuriken_Deco_3)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Deco_Desc_4{};
    Deco_Desc_4.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_4.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_4.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_4.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_4 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_4));
    if (nullptr == m_Effect_RasenShuriken_Deco_4)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Deco_Desc_5{};
    Deco_Desc_5.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_5.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_5.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_5.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_5 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_5));
    if (nullptr == m_Effect_RasenShuriken_Deco_5)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Deco_Desc_6{};
    Deco_Desc_6.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Deco_Desc_6.MyUVOption = CEffect_Mesh::MOVE_X;
    Deco_Desc_6.MySpinOption = CEffect_Mesh::SPIN_RANDOM;
    Deco_Desc_6.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_RasenShuriken_Deco_6 = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Deco_Desc_6));
    if (nullptr == m_Effect_RasenShuriken_Deco_6)
        return E_FAIL;

    return S_OK;
}

HRESULT CRasenShuriken::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
    InstanceDesc.iNumInstance = 150;
    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
    InstanceDesc.vSize = _float2(0.08f, 0.09f);
    InstanceDesc.vSpeed = _float2(2.5f, 3.5f);
    InstanceDesc.vLifeTime = _float2(1.f, 1.3f);
    InstanceDesc.isLoop = false;
    InstanceDesc.vColor = _float4(121.f / 255.f, 237.f / 255.f, 1.f, 0.7f);
    InstanceDesc.fDuration = 1.4f;
    InstanceDesc.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
    InstanceDesc.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";

    m_BoomParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
    if (nullptr == m_BoomParticles)
        return E_FAIL;


    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc1{};
    InstanceDesc1.iNumInstance = 500;
    InstanceDesc1.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vRange = _float3(3.f, 3.f, 3.f);
    InstanceDesc1.vSize = _float2(0.04f, 0.05f);
    InstanceDesc1.vSpeed = _float2(1.5f, 2.f);
    InstanceDesc1.vLifeTime = _float2(2.5f, 3.f);
    InstanceDesc1.isLoop = false;
    InstanceDesc1.vColor = _float4(121.f / 255.f, 237.f / 255.f, 1.f, 0.7f);
    InstanceDesc1.fDuration = 3.1f;
    InstanceDesc1.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc1.MyOption_Shape = CVIBuffer_Instancing::SHAPE_RECTANGLE;
    InstanceDesc1.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc1.MyOption_Size = CVIBuffer_Instancing::SIZE_DIMINISH;
    InstanceDesc1.strTextureTag = L"Prototype_Component_Texture_Circle_Noise";
    InstanceDesc1.vSpriteRatio = _float2(1.f, 1.f);
    
    m_DissolveParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc1));
    if (nullptr == m_DissolveParticles)
        return E_FAIL;
    
    return S_OK;
}

CRasenShuriken* CRasenShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRasenShuriken* pInstance = new CRasenShuriken(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRasenShuriken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRasenShuriken::Clone(void* pArg)
{
    CRasenShuriken* pInstance = new CRasenShuriken(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CRasenShuriken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRasenShuriken::Free()
{
    Safe_Release(m_Effect_RasenShuriken_Main);
    Safe_Release(m_Effect_RasenShuriken_Wing);
    Safe_Release(m_Effect_RasenShuriken_Ring);

    Safe_Release(m_Effect_RasenShuriken_Boom);

    Safe_Release(m_Effect_RasenShuriken_Deco_1);
    Safe_Release(m_Effect_RasenShuriken_Deco_2);
    Safe_Release(m_Effect_RasenShuriken_Deco_3);
    Safe_Release(m_Effect_RasenShuriken_Deco_4);
    Safe_Release(m_Effect_RasenShuriken_Deco_5);
    Safe_Release(m_Effect_RasenShuriken_Deco_6);

    Safe_Release(m_BoomParticles);
    Safe_Release(m_DissolveParticles);

    Safe_Release(m_pColliderMain);

    __super::Free();
}

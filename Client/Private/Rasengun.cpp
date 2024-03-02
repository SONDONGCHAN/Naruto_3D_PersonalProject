#include "Rasengun.h"

CRasengun::CRasengun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CRasengun::CRasengun(const CRasengun& rhs)
    :CSkill(rhs)
{
}

HRESULT CRasengun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRasengun::Initialize(void* pArg)
{
    SKILL_RASENGUN_DESC* pRasengunDesc = (SKILL_RASENGUN_DESC*)pArg;
    m_pSocketMatrix = pRasengunDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components())) 
        return E_FAIL;

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity() ) ;
    m_pTransformCom->Set_World(m_WorldMatrix);
    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.15f, 0.15f, 1.f));
    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

    return S_OK;
}

void CRasengun::Priority_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        m_Effect_Rasengun_Main->Priority_Tick(fTimeDelta);

        if (myState == STATE_MAKING)
            m_Effect_Rasengun_Charge->Priority_Tick(fTimeDelta);

        else if (myState == STATE_DETECTING)
            m_Effect_Rasengun_Rush->Priority_Tick(fTimeDelta);
    }

    if (myState == STATE_HIT)
        m_Effect_Rasengun_Boom->Priority_Tick(fTimeDelta);
}

void CRasengun::Tick(_float fTimeDelta)
{
    __super:: Tick(fTimeDelta);

    _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
   XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());   
    m_pTransformCom->Set_World(m_WorldMatrix);

    State_Control(fTimeDelta);

    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        m_Effect_Rasengun_Main->Tick(fTimeDelta);
        m_Effect_Rasengun_Main->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_Rasengun_Main->Scale_Change(fTimeDelta);

        if (myState == STATE_MAKING)
        {
            m_Effect_Rasengun_Charge->Tick(fTimeDelta);

            _matrix ParentMakingMat = m_pParentTransform->Get_WorldMatrix();
            _matrix MakingMat       = m_pTransformCom->Get_WorldMatrix();
            ParentMakingMat.r[3] = MakingMat.r[3];
            m_Effect_Rasengun_Charge->State_Tick(ParentMakingMat);

            m_Effect_Rasengun_Charge->Scale_Change(fTimeDelta);
        }
        else if (myState == STATE_DETECTING)
        {
            m_Effect_Rasengun_Rush->Tick(fTimeDelta);
            _matrix ParentRushMat = m_pParentTransform->Get_WorldMatrix();
            _matrix RushMat = m_pTransformCom->Get_WorldMatrix();
            ParentRushMat.r[3] = RushMat.r[3];
            m_Effect_Rasengun_Rush->State_Tick(ParentRushMat);
        }
    }

    if (myState == STATE_HIT)
    {
        m_Effect_Rasengun_Boom->Tick(fTimeDelta);

        _matrix ParentBoomMat = m_pParentTransform->Get_WorldMatrix();
        _vector vDir = XMVector3Normalize(ParentBoomMat.r[2]);
        _matrix BoomMat = m_pTransformCom->Get_WorldMatrix();
        ParentBoomMat.r[3] = BoomMat.r[3];
        ParentBoomMat.r[3] += vDir * 1.0f;

        m_Effect_Rasengun_Boom->State_Tick(ParentBoomMat);
        m_Effect_Rasengun_Boom->Scale_Change(fTimeDelta);

    }
    
}

void CRasengun::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        m_Effect_Rasengun_Main->Late_Tick(fTimeDelta);

        if (myState == STATE_MAKING)
            m_Effect_Rasengun_Charge->Late_Tick(fTimeDelta);
        else if (myState == STATE_DETECTING)
            m_Effect_Rasengun_Rush->Late_Tick(fTimeDelta);
    }

    if (myState == STATE_HIT)
    {
        m_Effect_Rasengun_Boom->Late_Tick(fTimeDelta);
    }

#ifdef _DEBUG
    if (myState == STATE_DETECTING || myState == STATE_HIT)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
}

HRESULT CRasengun::Render()
{
    return S_OK;
}

void CRasengun::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

void CRasengun::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CRasengun::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{

}

void CRasengun::State_Control(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        // 손위치로 업데이트
        // 이펙트 채워넣기
    }
    else if (myState == STATE_DETECTING)
    {
        m_fDurTime += fTimeDelta;

        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider");

        if (m_fDurTime > 1.5f)
        {
            m_bIsHit = true;
            Set_Next_State();
        }

        // 이펙트 채워넣기
    }
    else if (myState == STATE_HIT)
    {
        m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Collider");

        m_fDurTime += fTimeDelta;

        if (m_fDurTime > 0.2f)
        {
            if (m_fDurTime < 0.22f)
                m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);

            Set_Next_State();
        }
        // 이펙트 채워넣기
    }
}

void CRasengun::Set_Next_State()
{
    if(myState!= STATE_END)
        myState = (RASENGUN_STATE)((_uint)myState + 1);

    if (myState == STATE_DETECTING)
    {
        m_pColliderMain->Set_Radius(1.0f);
    }

    else if (myState == STATE_HIT)
    {
        m_Effect_Rasengun_Boom->Start_Trigger();
        m_BoomParticles->Trigger(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

        m_pColliderMain->Set_Radius(2.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;   
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Collider");

        m_fDurTime = 0;
        m_bIsHit = false;
    }
}

void CRasengun::Set_State()
{
    myState = STATE_MAKING; 
    m_Effect_Rasengun_Main->Start_Trigger();
    m_Effect_Rasengun_Charge->Start_Trigger();

}

void CRasengun::Particles_Priority_Tick(_float fTimeDelta)
{
    m_BoomParticles->Priority_Tick(fTimeDelta);
}

void CRasengun::Particles_Tick(_float fTimeDelta)
{
    m_BoomParticles->Tick(fTimeDelta);
}

void CRasengun::Particles_Late_Tick(_float fTimeDelta)
{
    m_BoomParticles->Late_Tick(fTimeDelta);
}

HRESULT CRasengun::Add_Components()
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

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Rasengun_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CRasengun::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_RASENGUN_MAIN;
    Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_1.vMyScale = _vector{ 0.08f, 0.08f, 0.08f, 1.f };
    m_Effect_Rasengun_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_Rasengun_Main)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_RASENGUN_CHARGE;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_X_INVERSE;
    Effect_Desc_2.vMyScale = _vector{ 50.f, 50.f, 50.f, 1.f };
    m_Effect_Rasengun_Charge = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_Rasengun_Charge)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_3{};
    Effect_Desc_3.MyType = CEffect_Mesh::EFFECT_RASENGUN_RUSH;
    Effect_Desc_3.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_3.vMyScale = _vector{ 1.2f, 1.2f, 1.2f, 1.f };
    m_Effect_Rasengun_Rush = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_3));
    if (nullptr == m_Effect_Rasengun_Rush)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_4{};
    Effect_Desc_4.MyType = CEffect_Mesh::EFFECT_RASENGUN_BOOM;
    Effect_Desc_4.MyUVOption = CEffect_Mesh::MOVE_END;
    Effect_Desc_4.vMyScale = _vector{ 20.f, 20.f, 20.f, 1.f };
    m_Effect_Rasengun_Boom = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_4));
    if (nullptr == m_Effect_Rasengun_Boom)
        return E_FAIL;

    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc1{};
    InstanceDesc1.iNumInstance = 150;
    InstanceDesc1.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vRange = _float3(5.f, 5.f, 5.f);
    InstanceDesc1.vSize = _float2(0.01f, 0.06f);
    InstanceDesc1.vSpeed = _float2(0.3f, 0.7f);
    InstanceDesc1.vLifeTime = _float2(2.5f, 3.f);
    InstanceDesc1.isLoop = false;
    InstanceDesc1.vColor = _float4( 121.f / 255.f, 237.f / 255.f, 1.f, 0.7f );
    InstanceDesc1.fDuration = 3.1f;
    InstanceDesc1.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc1.MyOption_Shape = CVIBuffer_Instancing::SHAPE_RECTANGLE;
    InstanceDesc1.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc1.strTextureTag = L"Prototype_Component_Texture_Circle_Noise";
    InstanceDesc1.vSpriteRatio = _float2(1.f, 1.f);

    m_BoomParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc1));
    if (nullptr == m_BoomParticles)
        return E_FAIL;

    return S_OK;
}

HRESULT CRasengun::Add_Particles()
{
    return S_OK;
}

CRasengun* CRasengun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRasengun* pInstance = new CRasengun(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRasengun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRasengun::Clone(void* pArg)
{
    CRasengun* pInstance = new CRasengun(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CRasengun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRasengun::Free()
{
    Safe_Release(m_pColliderMain);
    Safe_Release(m_Effect_Rasengun_Main);
    Safe_Release(m_Effect_Rasengun_Charge);
    Safe_Release(m_Effect_Rasengun_Rush);
    Safe_Release(m_Effect_Rasengun_Boom);
    Safe_Release(m_BoomParticles);

    __super::Free();
}

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
    //m_pTransformCom->Rotation(XMVectorSet(, , , 0.f), XMConvertToRadians());
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.2f, 0.2f, 1.f));

    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

    m_fSkill_Power = 5.f;

    return S_OK;
}

void CRasenShuriken::Priority_Tick(_float fTimeDelta)
{
}

void CRasenShuriken::Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        if (myState == STATE_MAKING)
        {
            _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
            XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
            m_pTransformCom->Set_World(m_WorldMatrix);
        }
        m_Effect_RasenShuriken_Main->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_RasenShuriken_Main->Tick(fTimeDelta);
        m_Effect_RasenShuriken_Main->Scale_Change(fTimeDelta);

    }
    else if (myState == STATE_HIT)
    {

    }
    State_Control(fTimeDelta);
}

void CRasenShuriken::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING || myState == STATE_DETECTING)
    {
        m_Effect_RasenShuriken_Main->Late_Tick(fTimeDelta);
    }
    else if (myState == STATE_HIT)
    {

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

        if (m_fDurTime > 1.5f)
            Set_Next_State();

        // 이펙트 채워넣기
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
        m_pColliderMain->Set_Radius(3.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"RasenShuriken_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"RasenShuriken_Collider");

        m_fDurTime = 0;
        m_bTargeting = false;
        m_bIsHit = false;
    }
}

void CRasenShuriken::Set_State()
{
    myState = STATE_MAKING;
    m_Effect_RasenShuriken_Main->Start_Trigger();
}

void CRasenShuriken::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

void CRasenShuriken::Particles_Priority_Tick(_float fTimeDelta)
{
}

void CRasenShuriken::Particles_Tick(_float fTimeDelta)
{
}

void CRasenShuriken::Particles_Late_Tick(_float fTimeDelta)
{
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
    Effect_Desc_1.vMyScale = _vector{ 0.12f, 0.12f, 0.12f, 1.f };
    m_Effect_RasenShuriken_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_RasenShuriken_Main)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_WING;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_2.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
    m_Effect_RasenShuriken_Wing = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_RasenShuriken_Wing)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_3{};
    Effect_Desc_3.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_RING;
    Effect_Desc_3.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_3.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
    m_Effect_RasenShuriken_Ring = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_3));
    if (nullptr == m_Effect_RasenShuriken_Ring)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_4{};
    Effect_Desc_4.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_BOOM;
    Effect_Desc_4.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_4.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
    m_Effect_RasenShuriken_Boom = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_4));
    if (nullptr == m_Effect_RasenShuriken_Boom)
        return E_FAIL;

    CEffect_Mesh::EFFECT_DESC Effect_Desc_5{};
    Effect_Desc_5.MyType = CEffect_Mesh::EFFECT_RASENSHURIKEN_DECO;
    Effect_Desc_5.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_5.vMyScale = _vector{ 1.f, 1.f, 1.f, 1.f };
    m_Effect_RasenShuriken_Deco = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_5));
    if (nullptr == m_Effect_RasenShuriken_Deco)
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
    Safe_Release(m_Effect_RasenShuriken_Deco);

    Safe_Release(m_BoomParticles);

    Safe_Release(m_pColliderMain);

    __super::Free();
}

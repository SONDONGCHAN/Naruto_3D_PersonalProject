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

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_pTransformCom->Set_World(m_WorldMatrix);

    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    //m_pTransformCom->Rotation(XMVectorSet(, , , 0.f), XMConvertToRadians());
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.5f, 0.2f, 1.f));

    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

    m_fSkill_Power = 5.f;

    return S_OK;
}

void CRasenShuriken::Priority_Tick(_float fTimeDelta)
{
}

void CRasenShuriken::Tick(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
        _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
        XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
        m_pTransformCom->Set_World(m_WorldMatrix);
    }
    State_Control(fTimeDelta);
}

void CRasenShuriken::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (myState == STATE_DETECTING || myState == STATE_HIT)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
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
    if (FAILED( __super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("RasenShuriken_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

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
    Safe_Release(m_pColliderMain);

    __super::Free();
}

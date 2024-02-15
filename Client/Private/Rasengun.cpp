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

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity() ) ;
    m_pTransformCom->Set_World(m_WorldMatrix);

    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    //m_pTransformCom->Rotation(XMVectorSet(, , , 0.f), XMConvertToRadians());
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.2f, 0.2f, 1.f));

    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

    m_fSkill_Power = 10.f;

    return S_OK;
}

void CRasengun::Priority_Tick(_float fTimeDelta)
{

}

void CRasengun::Tick(_float fTimeDelta)
{
    _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
   XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());   
    m_pTransformCom->Set_World(m_WorldMatrix);

    State_Control(fTimeDelta);
}

void CRasengun::Late_Tick(_float fTimeDelta)
{
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
            m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Monster_Main_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Player_Main_Collider");

        if (m_fDurTime > 1.5f)
        {
            m_bIsHit = true;
            Set_Next_State();
        }

        // 이펙트 채워넣기
    }
    else if (myState == STATE_HIT)
    {
        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Collider");

        m_fDurTime += fTimeDelta;

        if(m_fDurTime > 0.1f)
            Set_Next_State();

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
        m_pColliderMain->Set_Radius(2.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;   
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Collider");

        m_fDurTime = 0;
        m_bIsHit = false;
    }
}

void CRasengun::Set_State()
{
    myState = STATE_MAKING; 
}


HRESULT CRasengun::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(TEXT("Rasengun_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

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
    __super::Free();
}

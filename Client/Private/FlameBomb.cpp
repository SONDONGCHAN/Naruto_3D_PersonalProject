#include "stdafx.h"
#include "FlameBomb.h"
#include "GameInstance.h"

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

    m_fSkill_Power = 10.f;

    return S_OK;
}

void CFlameBomb::Priority_Tick(_float fTimeDelta)
{
}

void CFlameBomb::Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
}

void CFlameBomb::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (myState == STATE_DETECTING || myState == STATE_HIT)
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
            m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Monster_Main_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Player_Main_Collider");

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
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

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
        m_pColliderMain->Set_Radius(3.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

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

HRESULT CFlameBomb::Add_Components()
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

    m_pGameInstance->Add_Collider(TEXT("FlameBomb_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

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

    __super::Free();
}

#include "Kamui.h"

CKamui::CKamui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CKamui::CKamui(const CKamui& rhs)
    :CSkill(rhs)
{
}

HRESULT CKamui::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKamui::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CKamui::Priority_Tick(_float fTimeDelta)
{
}

void CKamui::Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
}

void CKamui::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (myState == STATE_HIT)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
}

HRESULT CKamui::Render()
{
    return S_OK;
}

void CKamui::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CKamui::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CKamui::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CKamui::State_Control(_float fTimeDelta)
{
    if (myState == STATE_MAKING)
    {
        // 이펙트 채워넣기
    }
    else if (myState == STATE_HIT)
    {
        m_fDurTime += fTimeDelta;
    
        if (m_bTargeting)
        {
            m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Kamui_Collider");
        }
    
        if (m_fDurTime > 1.1f)
            Set_Next_State();
    
        // 이펙트 채워넣기
    }
}

void CKamui::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (KAMUI_STATE)((_uint)myState + 1);

    if (myState == STATE_HIT)
    {
        m_pTransformCom->Set_Pos(m_vTarget_Pos);
        m_pColliderMain->Set_Radius(0.1f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Kamui_Collider");
        m_fDurTime = 0;
        m_bTargeting = false;
    }
}

void CKamui::Set_State()
{
    myState = STATE_MAKING;
}

void CKamui::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

HRESULT CKamui::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;
    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Kamui_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

CKamui* CKamui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKamui* pInstance = new CKamui(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CKamui");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKamui::Clone(void* pArg)
{
    CKamui* pInstance = new CKamui(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CKamui");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKamui::Free()
{
    Safe_Release(m_pColliderMain);

    __super::Free();
}

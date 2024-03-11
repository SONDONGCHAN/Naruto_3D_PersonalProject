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
    
    if (FAILED(Add_Effects()))
        return E_FAIL;

    return S_OK;
}

void CKamui::Priority_Tick(_float fTimeDelta)
{
}

void CKamui::Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
    m_Effect_Kamui_Boom->State_Tick(m_pTransformCom->Get_WorldMatrix());
    m_Effect_Kamui_Boom->Tick(fTimeDelta);
    m_Effect_Kamui_ShockWave->State_Tick(m_pTransformCom->Get_WorldMatrix());
    m_Effect_Kamui_ShockWave->Tick(fTimeDelta);
    if (myState == STATE_HIT)
    {
        m_Effect_Kamui_Boom->Scale_Change(fTimeDelta);
    }
    else if (myState == STATE_FINISH)
    {
        m_Effect_Kamui_ShockWave->Scale_Change(fTimeDelta);
    }
}

void CKamui::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_HIT)
    {
        m_Effect_Kamui_Boom->Late_Tick(fTimeDelta);
#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
#endif  

    }
    else if (myState == STATE_FINISH)
    {
        m_Effect_Kamui_ShockWave->Late_Tick(fTimeDelta);
    }
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

    }
    else if (myState == STATE_HIT)
    {
        m_fDurTime += fTimeDelta;
    
        if (m_bTargeting && m_fDurTime < 0.8f)
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
        m_Effect_Kamui_Boom->Start_Trigger();
        m_pTransformCom->Set_Pos(m_vTarget_Pos);
        _vector vDir = m_pParentTransform->Get_State(CTransform::STATE_LOOK);
        m_pTransformCom->Set_Look(vDir);
        m_pColliderMain->Set_Radius(8.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_Effect_Kamui_ShockWave->Start_Trigger();
        m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Kamui_Collider");
        m_fDurTime = 0;
        m_bTargeting = false;
    }
    else if (myState == STATE_END)
    {
        m_Effect_Kamui_ShockWave->Start_Trigger();
        m_Effect_Kamui_ShockWave->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_Kamui_ShockWave->Late_Tick(1.f / 60.f);
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

void CKamui::Particles_Priority_Tick(_float fTimeDelta)
{
}

void CKamui::Particles_Tick(_float fTimeDelta)
{
}

void CKamui::Particles_Late_Tick(_float fTimeDelta)
{
}

HRESULT CKamui::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 6.5f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;
    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Kamui_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CKamui::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_KAMUI;
    Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_1.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_1.vMyScale = _vector{ 25.f, 25.f, 5.f, 1.f };
    m_Effect_Kamui_Boom = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_Kamui_Boom)
        return E_FAIL;
    
    CEffect_Mesh::EFFECT_DESC Effect_Desc_2{};
    Effect_Desc_2.MyType = CEffect_Mesh::EFFECT_KAMUI_SHOCK;
    Effect_Desc_2.MyUVOption = CEffect_Mesh::MOVE_END;
    Effect_Desc_2.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_2.vMyScale = _vector{ 20.f, 20.f, 3.f, 1.f };
    m_Effect_Kamui_ShockWave = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_2));
    if (nullptr == m_Effect_Kamui_ShockWave)
        return E_FAIL;

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
    Safe_Release(m_Effect_Kamui_Boom);
    Safe_Release(m_Effect_Kamui_ShockWave);

    __super::Free();
}

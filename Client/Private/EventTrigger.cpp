#include "EventTrigger.h"
#include "Boss_Kurama.h"


CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
    :CGameObject(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{
    EVENTTRIGGER_DESC* pTriggerDesc = (EVENTTRIGGER_DESC*)pArg;

    m_MyDesc.Current_Level  = pTriggerDesc->Current_Level;
    m_MyDesc.vPos           = pTriggerDesc->vPos;

    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_MyDesc.vPos);
    m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{
    if (!m_bUsed)
    {
        m_pGameInstance->Check_Collision_For_MyEvent(m_MyDesc.Current_Level, m_pColliderMain, L"Player_Main_Collider");
    }
}

void CEventTrigger::Tick(_float fTimeDelta)
{
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{
    if (!m_bUsed)
    {
#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
#endif
    }
}

HRESULT CEventTrigger::Render()
{
    return S_OK;
}

HRESULT CEventTrigger::Add_Component()
{
    CBounding_Sphere::SPHERE_DESC		TriggerBoundingDesc{};
    TriggerBoundingDesc.fRadius = 2.f;
    TriggerBoundingDesc.vCenter = _float3(0.f, TriggerBoundingDesc.fRadius, 0.f);
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &TriggerBoundingDesc)))
        return E_FAIL;
    m_pGameInstance->Add_Collider(m_MyDesc.Current_Level, L"Trigger_Collider", m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);
    
    return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{
    return S_OK;
}

void CEventTrigger::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
    dynamic_cast<CBoss_Kurama*>(m_pGameInstance->Get_GameObject(m_MyDesc.Current_Level, TEXT("Layer_Monster"), 2))->Set_Appear();
        m_bUsed = true;
}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEventTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
    CEventTrigger* pInstance = new CEventTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEventTrigger");
        Safe_Release(pInstance);
        
    }

    return pInstance;
}

void CEventTrigger::Free()
{
    Safe_Release(m_pColliderMain);
    __super::Free();
}

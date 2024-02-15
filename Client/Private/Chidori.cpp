#include "Chidori.h"

CChidori::CChidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CChidori::CChidori(const CChidori& rhs)
    :CSkill(rhs)
{
}

HRESULT CChidori::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChidori::Initialize(void* pArg)
{
    SKILL_CHIDORI_DESC* pChidoriDesc = (SKILL_CHIDORI_DESC*)pArg;
    m_pSocketMatrix = pChidoriDesc->pSocketMatrix;
    
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    if (FAILED(Add_Components()))
        return E_FAIL;
    
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_pTransformCom->Set_World(m_WorldMatrix);
    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.2f, 0.2f, 1.f));
    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();
    
    m_fSkill_Power = 10.f;
   
    return S_OK;
}

void CChidori::Priority_Tick(_float fTimeDelta)
{
}

void CChidori::Tick(_float fTimeDelta)
{
    _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
    m_pTransformCom->Set_World(m_WorldMatrix);
    
    State_Control(fTimeDelta);
}

void CChidori::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (myState == STATE_DETECTING)
    {
        m_pGameInstance->Add_DebugComponent(m_pColliderMain);
    }
#endif  
}

HRESULT CChidori::Render()
{
    return S_OK;
}

void CChidori::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
    if (strColliderLayerTag == L"Monster_Main_Collider")
    {
        if (myState == STATE_DETECTING)
        {
            m_bIsHit = true;
        }
    }
}

void CChidori::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CChidori::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CChidori::State_Control(_float fTimeDelta)
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
    
        m_pGameInstance->Check_Collision_For_MyEvent(m_pColliderMain, L"Monster_Main_Collider");
        m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Chidori_Collider");

        if (m_fDurTime > 0.4f)
        {
            m_bIsHit = true;
        }
    
        // 이펙트 채워넣기
    }
}

void CChidori::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (CHIDORI_STATE)((_uint)myState + 1);

    if (myState == STATE_DETECTING)
    {
        m_pColliderMain->Set_Radius(1.5f);
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Chidori_Collider");

        m_fDurTime = 0;
        m_bIsHit = false;
    }
}

void CChidori::Set_State()
{
    myState = STATE_MAKING;
}

HRESULT CChidori::Add_Components()
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
    
    m_pGameInstance->Add_Collider(TEXT("Chidori_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);
    
    return S_OK;
}

CChidori* CChidori::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChidori* pInstance = new CChidori(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CChidori");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChidori::Clone(void* pArg)
{
    CChidori* pInstance = new CChidori(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CChidori");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChidori::Free()
{
    Safe_Release(m_pColliderMain);
    __super::Free();
}

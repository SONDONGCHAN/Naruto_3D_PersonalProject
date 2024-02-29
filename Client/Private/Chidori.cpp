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

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;
    
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_pTransformCom->Set_World(m_WorldMatrix);
    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
    m_OriginalMat = m_pTransformCom->Get_WorldMatrix();
       
    return S_OK;
}

void CChidori::Priority_Tick(_float fTimeDelta)
{
    m_BasicParticles->Priority_Tick(fTimeDelta);

}

void CChidori::Tick(_float fTimeDelta)
{
    _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
    m_pTransformCom->Set_World(m_WorldMatrix);
    
    __super::Tick(fTimeDelta);


    State_Control(fTimeDelta);

    m_Effect_Chidori_Main->State_Tick(XMLoadFloat4x4(&m_WorldMatrix));
    m_Effect_Chidori_Main->Tick(fTimeDelta);
    m_BasicParticles->Tick(fTimeDelta);
}

void CChidori::Late_Tick(_float fTimeDelta)
{
    m_Effect_Chidori_Main->Late_Tick(fTimeDelta);
    m_BasicParticles->Late_Tick(fTimeDelta);

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
    
        m_pGameInstance->Check_Collision_For_MyEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider");
        m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"Chidori_Collider");

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
        m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level,m_pColliderMain, L"Monster_Main_Collider", L"Chidori_Collider");

        m_fDurTime = 0;
        m_bIsHit = false;
    }
}

void CChidori::Set_State()
{
    myState = STATE_MAKING;
}

void CChidori::Particles_Priority_Tick(_float fTimeDelta)
{
}

void CChidori::Particles_Tick(_float fTimeDelta)
{
}

void CChidori::Particles_Late_Tick(_float fTimeDelta)
{
}

HRESULT CChidori::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.4f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;
    
    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Chidori_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);
    
    return S_OK;
}

HRESULT CChidori::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc{};
    Effect_Desc.MyType = CEffect_Mesh::EFFECT_CHIDORI;
    Effect_Desc.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc.vMyScale = _vector{ 0.4f, 0.4f, 0.4f, 1.f };
    m_Effect_Chidori_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc));
    if (nullptr == m_Effect_Chidori_Main)
        return E_FAIL;

    return S_OK;
}

HRESULT CChidori::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
    InstanceDesc.iNumInstance = 20;
    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc.pCenter = &m_MyPos;
    InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
    InstanceDesc.vSize = _float2(0.02f, 0.021f);
    InstanceDesc.vSpeed = _float2(1.5f, 2.5f);
    InstanceDesc.vLifeTime = _float2(0.1f, 1.0f);
    InstanceDesc.isLoop = true;
    InstanceDesc.vColor = _float4(13.f / 255.f, 154.f / 255.f, 1.f, 1.f);
    InstanceDesc.fDuration = 1.3f;
    InstanceDesc.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
    InstanceDesc.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";
    
    m_BasicParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
    if (nullptr == m_BasicParticles)
        return E_FAIL;

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
    Safe_Release(m_Effect_Chidori_Main);
    Safe_Release(m_BasicParticles);
    Safe_Release(m_pColliderMain);

    __super::Free();
}

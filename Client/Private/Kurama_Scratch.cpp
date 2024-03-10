#include "Kurama_Scratch.h"

CKurama_Scratch::CKurama_Scratch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CKurama_Scratch::CKurama_Scratch(const CKurama_Scratch& rhs)
	:CSkill(rhs)
{
}

HRESULT CKurama_Scratch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKurama_Scratch::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

    if (FAILED(Add_Effects()))
        return E_FAIL;

    if (FAILED(Add_Particles()))
        return E_FAIL;

	return S_OK;
}

void CKurama_Scratch::Priority_Tick(_float fTimeDelta)
{
}

void CKurama_Scratch::Tick(_float fTimeDelta)
{
	State_Control(fTimeDelta);

    if (STATE_DETECTING == myState)
    {
        m_Effect_Scratch_Main->State_Tick(m_pTransformCom->Get_WorldMatrix());
        m_Effect_Scratch_Main->Tick(fTimeDelta);
        m_Effect_Scratch_Main->Scale_Change(fTimeDelta);
    }
}

void CKurama_Scratch::Late_Tick(_float fTimeDelta)
{

	if (myState == STATE_DETECTING || myState == STATE_HIT)
	{
        if (STATE_DETECTING == myState)
        {
            m_Effect_Scratch_Main->Late_Tick(fTimeDelta);
        }
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pColliderMain);
#endif  

	}
}

HRESULT CKurama_Scratch::Render()
{
	return S_OK;
}

void CKurama_Scratch::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
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

void CKurama_Scratch::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CKurama_Scratch::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CKurama_Scratch::State_Control(_float fTimeDelta)
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
            m_pTransformCom->LookAt(m_vTarget_Pos);

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
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

        m_fDurTime += fTimeDelta;

        if (m_fDurTime > 0.1f)
            Set_Next_State();

        // 이펙트 채워넣기
    }
}

void CKurama_Scratch::Set_Next_State()
{
    if (myState != STATE_END)
        myState = (KURAMA_SCRATCH_STATE)((_uint)myState + 1);

    if (myState == STATE_DETECTING)
    {
        m_pGameInstance->PlaySoundW("Boss_Scratch", SOUND_SKILL, 1.f, true);

        m_Effect_Scratch_Main->Start_Trigger();

        m_pColliderMain->Set_Radius(1.f);
    }

    else if (myState == STATE_HIT)
    {
        m_pGameInstance->PlaySoundW("Boss_Scratch_Hit", SOUND_SKILL_2, 1.f, true);
        m_BoomParticles->Trigger(m_pTransformCom->Get_WorldMatrix().r[3]);
        m_pColliderMain->Set_Radius(3.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
        m_fDurTime = 0;
    }
    else if (myState == STATE_FINISH)
    {
        m_pColliderMain->Set_Radius(0.f);
        m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

        if (m_User_Type == USER_PLAYER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Monster_Main_Collider", L"FlameBomb_Collider");
        else if (m_User_Type == USER_MONSTER)
            m_pGameInstance->Check_Collision_For_TargetEvent(m_Current_Level, m_pColliderMain, L"Player_Main_Collider", L"FlameBomb_Collider");

        m_fDurTime = 0;
        m_bTargeting = false;
        m_bIsHit = false;
    }
}

void CKurama_Scratch::Set_State()
{
    myState = STATE_MAKING;
}

void CKurama_Scratch::Set_Targeting(_vector Target_Pos)
{
    m_bTargeting = true;
    m_vTarget_Pos = Target_Pos;
}

void CKurama_Scratch::Particles_Priority_Tick(_float fTimeDelta)
{
    m_BoomParticles->Priority_Tick(fTimeDelta);
}

void CKurama_Scratch::Particles_Tick(_float fTimeDelta)
{
    m_BoomParticles->Tick(fTimeDelta);
}

void CKurama_Scratch::Particles_Late_Tick(_float fTimeDelta)
{
    m_BoomParticles->Late_Tick(fTimeDelta);
}

HRESULT CKurama_Scratch::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
    BoundingDesc.fRadius = 0.f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_Current_Level, TEXT("Kurama_Scratch_Collider"), m_pColliderMain);
    m_pColliderMain->Set_Collider_GameObject(this);

    return S_OK;
}

HRESULT CKurama_Scratch::Add_Effects()
{
    CEffect_Mesh::EFFECT_DESC Effect_Desc_1{};
    Effect_Desc_1.MyType = CEffect_Mesh::EFFECT_KURAMA_SCRATCH;
    Effect_Desc_1.MyUVOption = CEffect_Mesh::MOVE_X;
    Effect_Desc_1.MySpinOption = CEffect_Mesh::SPIN_NONE;
    Effect_Desc_1.vMyScale = _vector{ 10.f, 10.f, 10.f, 0.8f };
    m_Effect_Scratch_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc_1));
    if (nullptr == m_Effect_Scratch_Main)
        return E_FAIL;

    return S_OK;
}

HRESULT CKurama_Scratch::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc{};
    InstanceDesc.iNumInstance = 150;
    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
    InstanceDesc.vSize = _float2(0.05f, 0.08f);
    InstanceDesc.vSpeed = _float2(2.5f, 3.5f);
    InstanceDesc.vLifeTime = _float2(0.7f, 1.0f);
    InstanceDesc.isLoop = false;
    InstanceDesc.vColor = _float4(138.f / 255.f, 43.f / 255.f, 226.f / 255.f, 1.0f);
    InstanceDesc.fDuration = 1.1f;
    InstanceDesc.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc.MyOption_Shape = CVIBuffer_Instancing::SHAPE_NIDDLE;
    InstanceDesc.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_NONE_SPRITE;
    InstanceDesc.strTextureTag = L"Prototype_Component_Texture_Circle";
    InstanceDesc.vSpriteRatio = _float2(1.f, 1.f);
    
    m_BoomParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc));
    if (nullptr == m_BoomParticles)
        return E_FAIL;

    return S_OK;
}

CKurama_Scratch* CKurama_Scratch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKurama_Scratch* pInstance = new CKurama_Scratch(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CKurama_Scratch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKurama_Scratch::Clone(void* pArg)
{
    CKurama_Scratch* pInstance = new CKurama_Scratch(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CKurama_Scratch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKurama_Scratch::Free()
{
    Safe_Release(m_pColliderMain);
    Safe_Release(m_Effect_Scratch_Main); 
    Safe_Release(m_BoomParticles);
 
    __super::Free();
}

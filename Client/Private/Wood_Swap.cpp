#include "Wood_Swap.h"
#include "GameInstance.h"

CWood_Swap::CWood_Swap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSkill(pDevice, pContext)
{
}

CWood_Swap::CWood_Swap(const CWood_Swap& rhs)
    :CSkill(rhs)
{
}

HRESULT CWood_Swap::Initialize_Prototype()
{
    m_RandomNumber = mt19937_64(m_RandomDevice());

    return S_OK;
}

HRESULT CWood_Swap::Initialize(void* pArg)
{
    SKILL_WOOD_SWAP_DESC* pWood_Swap_Desc= (SKILL_WOOD_SWAP_DESC*)pArg;

    m_pUser_Navigation = pWood_Swap_Desc->pUser_Navigation;

    //Safe_AddRef(m_pUser_Navigation);

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

void CWood_Swap::Priority_Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
}

void CWood_Swap::Tick(_float fTimeDelta)
{

    if (myState == STATE_APPEAR || myState == STATE_RENDER)
    {
        m_Effect_WoodSwap_Main->State_Tick(m_MyMat);
        m_Effect_WoodSwap_Main->Tick(fTimeDelta);
        m_Effect_WoodSwap_Main->Scale_Change(fTimeDelta);
    }
}

void CWood_Swap::Late_Tick(_float fTimeDelta)
{
    if (myState == STATE_APPEAR || myState == STATE_RENDER)
    {
        m_Effect_WoodSwap_Main->Late_Tick(fTimeDelta);
    }


}

HRESULT CWood_Swap::Render()
{
    if (myState == STATE_RENDER)
    {

    }
    return S_OK;
}

void CWood_Swap::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Swap::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Swap::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Swap::State_Control(_float fTimeDelta)
{
    if (myState == STATE_APPEAR)
    {
        m_MyMat = m_pParentTransform->Get_WorldMatrix();
        m_MyMat.r[3].m128_f32[1] += 0.7f;
        m_SmokeParticles->Trigger(m_MyMat.r[3]);
        m_Effect_WoodSwap_Main->Start_Trigger();

        Set_User_Pos();
        Set_Next_State();
    }
    else if (myState == STATE_RENDER)
    {
        m_fDurTime += fTimeDelta;

        if(m_fDurTime>1.5f)
            Set_Next_State();
    }
}

void CWood_Swap::Set_Next_State()
{
    if (myState != STATE_FINISH)
        myState = (WOOD_SWAP_STATE)((_uint)myState + 1);
}

void CWood_Swap::Set_State()
{
    myState = STATE_APPEAR;
    m_fDurTime = 0.f;
    _vector UserPos = m_pParentTransform->Get_State(CTransform::STATE_POSITION);
    m_pTransformCom->Set_Pos(UserPos);
}

void CWood_Swap::Set_User_Pos()
{
    uniform_real_distribution<float>	DirRange(-1.f , 1.f );

    _float fX = DirRange(m_RandomNumber);
    _float fZ = DirRange(m_RandomNumber);

    _vector vTargetDir    = { fX, 0.f, fZ };

    vTargetDir = XMVector3Normalize(vTargetDir);

    uniform_real_distribution<float>	LengthRange(3.f, 5.f);

    _float fDistance = LengthRange(m_RandomNumber);

    uniform_real_distribution<float>	YRange(3.f, 4.f);

    _float fY = YRange(m_RandomNumber);

    //
    fY = 3.f;

    _vector vFinal = (vTargetDir * fDistance);

    _vector vPlayer_Pos = m_pParentTransform->Get_State(CTransform::STATE_POSITION);

    for (_int i = 0; i < 10; i++)
    {
        vPlayer_Pos += vFinal * 0.1f;
        m_pParentTransform->Set_LandObject_Pos(vPlayer_Pos, m_pUser_Navigation);
    }
    vPlayer_Pos.m128_f32[1] += fY;

    m_pParentTransform->Set_LandObject_Pos(vPlayer_Pos, m_pUser_Navigation);
}

void CWood_Swap::Particles_Priority_Tick(_float fTimeDelta)
{
    m_SmokeParticles->Priority_Tick(fTimeDelta);
}

void CWood_Swap::Particles_Tick(_float fTimeDelta)
{
    m_SmokeParticles->Tick(fTimeDelta);
}

void CWood_Swap::Particles_Late_Tick(_float fTimeDelta)
{
    m_SmokeParticles->Late_Tick(fTimeDelta);
}

HRESULT CWood_Swap::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
        
    return S_OK;
}

HRESULT CWood_Swap::Add_Effects()
{   
    CEffect_Mesh::EFFECT_DESC Effect_Desc{};
    Effect_Desc.MyType = CEffect_Mesh::EFFECT_WOOD_SWAP;
    Effect_Desc.MyUVOption = CEffect_Mesh::MOVE_END;
    Effect_Desc.MySpinOption = CEffect_Mesh::SPIN_Y;
    Effect_Desc.vMyScale = _vector{ 1.2f, 1.5f, 1.2f, 1.f };
    m_Effect_WoodSwap_Main = dynamic_cast<CEffect_Mesh*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &Effect_Desc));
    if (nullptr == m_Effect_WoodSwap_Main)
        return E_FAIL;
    
    return S_OK;
}

HRESULT CWood_Swap::Add_Particles()
{
    CVIBuffer_Instancing::INSTANCE_DESC  InstanceDesc1{};
    InstanceDesc1.iNumInstance = 20;
    InstanceDesc1.vPivot = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.vCenter = _float3(0.f, 0.f, 0.f);
    InstanceDesc1.pCenter = &m_MyPos;
    InstanceDesc1.vRange = _float3(3.f, 3.f, 3.f);
    InstanceDesc1.vSize = _float2(2.f, 2.f);
    InstanceDesc1.vSpeed = _float2(0.1f, 0.2f);
    InstanceDesc1.vLifeTime = _float2(1.5f, 2.f);
    InstanceDesc1.isLoop = false;
    InstanceDesc1.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    InstanceDesc1.fDuration = 2.1f;
    InstanceDesc1.MyOption_Moving = CVIBuffer_Instancing::OPTION_SPREAD;
    InstanceDesc1.MyOption_Shape = CVIBuffer_Instancing::SHAPE_SQUARE;
    InstanceDesc1.MyOption_Texture = CVIBuffer_Instancing::TEXTURE_SPRITE;
    InstanceDesc1.strTextureTag = L"Prototype_Component_Texture_Smoke_Sprite";
    InstanceDesc1.vSpriteRatio = _float2(4.f, 4.f);

    m_SmokeParticles = dynamic_cast<CParticle_Point*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Point"), &InstanceDesc1));
    if (nullptr == m_SmokeParticles)
        return E_FAIL;
    
    return S_OK;

}

CWood_Swap* CWood_Swap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWood_Swap* pInstance = new CWood_Swap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWood_Swap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWood_Swap::Clone(void* pArg)
{
    CWood_Swap* pInstance = new CWood_Swap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWood_Swap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWood_Swap::Free()
{
    Safe_Release(m_Effect_WoodSwap_Main);
    Safe_Release(m_SmokeParticles);

   // Safe_Release(m_pUser_Navigation);
    __super::Free();
}

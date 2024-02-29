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

    return S_OK;
}

void CWood_Swap::Priority_Tick(_float fTimeDelta)
{
    State_Control(fTimeDelta);
}

void CWood_Swap::Tick(_float fTimeDelta)
{
}

void CWood_Swap::Late_Tick(_float fTimeDelta)
{
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

    uniform_real_distribution<float>	LengthRange(5.f, 10.f);

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
}

void CWood_Swap::Particles_Tick(_float fTimeDelta)
{
}

void CWood_Swap::Particles_Late_Tick(_float fTimeDelta)
{
}

HRESULT CWood_Swap::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;
        
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
   // Safe_Release(m_pUser_Navigation);

    __super::Free();
}

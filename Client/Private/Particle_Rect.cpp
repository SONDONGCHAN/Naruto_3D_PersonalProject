#include "Particle_Rect.h"
#include "GameInstance.h"

CParticle_Rect::CParticle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CParticle_Rect::CParticle_Rect(const CParticle_Rect& rhs)
    : CGameObject(rhs)
{
}

HRESULT CParticle_Rect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Rect::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};
    
    GameObjectDesc.fSpeedPerSec = 10.0f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Rect::Priority_Tick(_float fTimeDelta)
{
}

void CParticle_Rect::Tick(_float fTimeDelta)
{
    m_pVIBufferCom->Tick_Spread(fTimeDelta);
   // m_pVIBufferCom->Tick_Drop(fTimeDelta);
}

void CParticle_Rect::Late_Tick(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
        return;
}

HRESULT CParticle_Rect::Render()
{
    _float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
    _float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
    _float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Rect::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
    InstanceDesc.vPivot = _float3(0.f, 10.f, 0.f);
    InstanceDesc.vCenter = _float3(0.0f, 10.f, 0.f);
    InstanceDesc.vRange = _float3(10.0f, 2.f, 10.f);
    InstanceDesc.vSize = _float2(0.2f, 0.7f);
    InstanceDesc.vSpeed = _float2(2.f, 5.f);
    InstanceDesc.vLifeTime = _float2(3.f, 5.f);
    InstanceDesc.isLoop = true;
    InstanceDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&InstanceDesc)))
        return E_FAIL;


    return S_OK;
}

CParticle_Rect* CParticle_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Rect* pInstance = new CParticle_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CParticle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Rect::Clone(void* pArg)
{
    CParticle_Rect* pInstance = new CParticle_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Rect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}

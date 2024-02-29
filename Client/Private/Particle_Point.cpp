#include "stdafx.h"
#include "..\Public\Particle_Point.h"
#include "GameInstance.h"

CParticle_Point::CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle_Point::CParticle_Point(const CParticle_Point& rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Point::Initialize(void* pArg)
{
    m_Option_Texture    = ((CVIBuffer_Instancing::INSTANCE_DESC*)pArg)->MyOption_Texture;
    m_pCenterPos        = ((CVIBuffer_Instancing::INSTANCE_DESC*)pArg)->pCenter;
    m_isLoop            = ((CVIBuffer_Instancing::INSTANCE_DESC*)pArg)->isLoop;

    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 10.0f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

void CParticle_Point::Priority_Tick(_float fTimeDelta)
{
}

void CParticle_Point::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Tick_Particle(fTimeDelta);
    Sprite_Tick(fTimeDelta);
}

void CParticle_Point::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CParticle_Point::Render()
{
    _uint Shader_Path = { 0 };

    if (m_Option_Texture == CVIBuffer_Instancing::TEXTURE_SPRITE)
        Shader_Path = 1;

    _float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
    _float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
    _float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();
    
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
        return E_FAIL;
    
    if (m_isLoop)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vCenterPos", m_pCenterPos, sizeof(_float4))))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vCenterPos", &m_vCenterPos, sizeof(_float4))))
            return E_FAIL;
    }
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteRatio", &m_vSpriteRatio, sizeof(_float2))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteCurrnetRatio", &m_vSpriteCurrentRatio, sizeof(_float2))))
        return E_FAIL;
    
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(Shader_Path)))
        return E_FAIL;
    
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    
    return S_OK;
}

_bool CParticle_Point::Trigger(_vector vCenterPos)
{
    XMStoreFloat4(&m_vCenterPos, vCenterPos);

    return  m_pVIBufferCom->Trigger(vCenterPos);
}

void CParticle_Point::Sprite_Tick(_float fTimeDelta)
{
    if (m_Option_Texture == CVIBuffer_Instancing::TEXTURE_SPRITE)
    {
        m_vSpriteTimeCal += fTimeDelta;

        if (m_vSpriteTimeCal >= 0.05f)
        {
            m_vSpriteTimeCal = 0.f;

            m_vSpriteCurrentRatio.x += 1.f;

            if (m_vSpriteCurrentRatio.x >= 8.f)
            {
                m_vSpriteCurrentRatio.x = 0.f;
                m_vSpriteCurrentRatio.y += 1.f;
            }

            if (m_vSpriteCurrentRatio.y >= 8.f)
                m_vSpriteCurrentRatio.y = 0.f;
        }
    }
}

HRESULT CParticle_Point::Add_Component(void* pArg)
{
    const wstring& _strTag = ((CVIBuffer_Instancing::INSTANCE_DESC*)pArg )->strTextureTag;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, _strTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), pArg)))
        return E_FAIL;


    return S_OK;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Point* pInstance = new CParticle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Point::Clone(void* pArg)
{
    CParticle_Point* pInstance = new CParticle_Point(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Point::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}

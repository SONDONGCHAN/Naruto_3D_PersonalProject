#include "stdafx.h"
#include "Weapon.h"

#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
    : CPartObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    WEAPON_DESC* pWeaponDesc = (WEAPON_DESC*)pArg;

    m_pSocketMatrix = pWeaponDesc->pSocketMatrix;
    _float4 mRotation = pWeaponDesc->vRotation;
    _float3 mLocation = pWeaponDesc->vLocation;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pWeaponDesc->strPrototypeTag)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
    m_pTransformCom->Rotation(XMVectorSet(mRotation.x, mRotation.y, mRotation.z, 0.f), XMConvertToRadians(mRotation.w));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(mLocation.x, mLocation.y, mLocation.z, 1.f));

    return S_OK;
}

void CWeapon::Priority_Tick(_float fTimeDelta)
{
}

void CWeapon::Tick(_float fTimeDelta)
{
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
    _matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

     for (size_t i = 0; i < 3; i++)
    {
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CWeapon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWeapon::Add_Component(const wstring& _strPrototypeTag)
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, _strPrototypeTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    _float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
    _float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
    _float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();


    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
    //    return E_FAIL;

    _float fAlphaDiscard = 0.1f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaDiscard", &fAlphaDiscard, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon* pInstance = new CWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

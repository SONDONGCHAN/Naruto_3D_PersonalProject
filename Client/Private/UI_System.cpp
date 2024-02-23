#include "UI_System.h"

CUI_System::CUI_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI(pDevice, pContext)
{
}

CUI_System::CUI_System(const CUI& rhs)
    : CUI(rhs)
{
}

HRESULT CUI_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_System::Initialize(void* pArg)
{
    UI_System_DESC* pSystem_Desc = (UI_System_DESC*)pArg;

    m_UI_Desc.vPos      = pSystem_Desc->vPos;
    m_UI_Desc.vSize     = pSystem_Desc->vSize;
    m_fRenderingTime    = pSystem_Desc->fRenderingTime;
    m_fAppearTime       = pSystem_Desc->fAppearTime;

    m_CurrentSize.x       = m_UI_Desc.vSize.x * 2.f;
    m_CurrentSize.y       = m_UI_Desc.vSize.y * 2.f;

    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(Add_Component(pSystem_Desc->strPrototypeTag)))
        return E_FAIL;

    return S_OK;
}

void CUI_System::Priority_Tick(_float fTimeDelta)
{
}

void CUI_System::Tick(_float fTimeDelta)
{
}

void CUI_System::Late_Tick(_float fTimeDelta)
{
    if (m_CurrentAlpha < 1 && !m_isAlphaEND)
    {
        m_CurrentAlpha += (fTimeDelta / m_fAppearTime);
        m_CurrentSize.x -= m_UI_Desc.vSize.x * (fTimeDelta / m_fAppearTime);
        m_CurrentSize.y -= m_UI_Desc.vSize.y * (fTimeDelta / m_fAppearTime);
    }
    else
    {
        if (!m_isAlphaEND)
            m_isAlphaEND = true;

        if(m_fRenderingTime>0)
            m_fRenderingTime -= fTimeDelta;

        else
        {
            if (m_CurrentAlpha > 0)
            {
                m_CurrentAlpha -= (fTimeDelta / m_fAppearTime);
                m_CurrentSize.x += m_UI_Desc.vSize.x * (fTimeDelta / m_fAppearTime);
                m_CurrentSize.y += m_UI_Desc.vSize.y * (fTimeDelta / m_fAppearTime);
            }
            else
                m_RendEND = true;
        }
       
    }

    if (!m_RendEND)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
            return;
    }
}

HRESULT CUI_System::Render()
{
    __super::Render();

     m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI_Desc.vPos.x - g_iWinSizeX * 0.5f, -(m_UI_Desc.vPos.y) + g_iWinSizeY * 0.5f, 0.f, 1.f));
     XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

     if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
         return E_FAIL;

     if (FAILED(m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
         return E_FAIL;

     if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_CurrentSize), sizeof(_float2))))
         return E_FAIL;

     if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &(m_CurrentAlpha), sizeof(_float))))
         return E_FAIL;

     if (FAILED(m_pShaderCom->Begin(4)))
         return E_FAIL;

     if (FAILED(m_pVIBufferCom->Bind_Buffers()))
         return E_FAIL;

     if (FAILED(m_pVIBufferCom->Render()))
         return E_FAIL;

    return S_OK;
}

HRESULT CUI_System::Add_Component(const wstring& _strPrototypeTag)
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, _strPrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
        return E_FAIL;

    return S_OK;
}

CUI_System* CUI_System::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_System* pInstance = new CUI_System(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_System::Clone(void* pArg)
{
    CUI_System* pInstance = new CUI_System(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_System::Free()
{
    Safe_Release(m_pTexture);

    __super::Free();
}

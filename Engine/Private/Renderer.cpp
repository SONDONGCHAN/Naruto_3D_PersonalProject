#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    D3D11_VIEWPORT			ViewPortDesc{};

    _uint		iNumViewports = 1;

    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

    /* 내 게임에 필요한 렌더타겟들을 추가한다.*/

#pragma region MRT_GAMEOBJECTS

    /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
        return E_FAIL;

    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;

#pragma endregion

#pragma region MRT_LightAcc

    /* For.Target_Shade*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

#pragma endregion

#pragma region MRT_Bloom

    /* For.Target_Effect*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Blur_X*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Blur_Y*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
        return E_FAIL;

#pragma endregion

#pragma region MRT_Distortion
    /* For.Target_Distortion */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

#pragma endregion
    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader_Deferred = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader_Deferred)
        return E_FAIL;

    m_pShader_PostProcessing = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader_PostProcessing)
        return E_FAIL;


    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix._11 = ViewPortDesc.Width;
    m_WorldMatrix._22 = ViewPortDesc.Height;

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Specular"), 150.f, 150.f, 100.f, 100.f)))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Effect"), 250.f, 50.f, 100.f, 100.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Blur_Y"), 250.f, 150.f, 100.f, 100.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Distortion"), 250.f, 250.f, 100.f, 100.f)))
        return E_FAIL;

#endif // _DEBUG


    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
    if (eRenderGroup >= RENDER_END)
        return E_FAIL;

    m_RenderObjects[eRenderGroup].push_back(pGameObject);

    Safe_AddRef(pGameObject);

    return S_OK;
}

HRESULT CRenderer::Render()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;
    if (FAILED(Render_NonBlend()))
        return E_FAIL;
    if (FAILED(Render_Lights()))
        return E_FAIL;
    if (FAILED(Render_NonLight()))
         return E_FAIL;
    if (FAILED(Render_Bloom()))
        return E_FAIL;
    if (FAILED(Render_Final()))
        return E_FAIL;
    if (FAILED(Render_PostProcessing()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;
    if (FAILED(Render_UI()))
        return E_FAIL;
    
#ifdef _DEBUG
    if (FAILED(Render_Debug()))
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
    m_DebugCom.push_back(pComponent);

    Safe_AddRef(pComponent);

    return S_OK;
}
#endif // _DEBUG
HRESULT CRenderer::Render_Priority()
{
    for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_PRIORITY].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_ProjMatrix_Float())))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_ViewMatrix_Float())))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    /* 노말 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Normal"), m_pShader_Deferred, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTexture")))
        return E_FAIL;

    /* 빛들을 하나씩 그린다.(사각형버퍼를 셰이드타겟에 그린다.) */
    m_pGameInstance->Render_Light(m_pShader_Deferred, m_pVIBuffer);

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Final()
{
    /* 사각형을 직교투영으로 화면에 꽉 채워서 그린다. */
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    /* 디퓨즈 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture")))
        return E_FAIL;
    /* 셰이더 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Shade"), m_pShader_Deferred, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Specular"), m_pShader_Deferred, "g_SpecularTexture")))
        return E_FAIL;
    //if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Blur_Y"), m_pShader_Deferred, "g_BlurTexture")))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Effect"), m_pShader_Deferred, "g_EffectTexture")))
    //    return E_FAIL;


    m_pShader_Deferred->Begin(3);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
        return E_FAIL;

    m_RenderObjects[RENDER_NONLIGHT].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            return (pSour)->Get_CamDistance() > (pDest)->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    
        Safe_Release(pGameObject);
    }
    
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            return (pSour)->Get_CamDistance() > (pDest)->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pGameObject : m_RenderObjects[RENDER_UI])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_UI].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Effect"), m_pShader_Deferred, "g_EffectTexture")))
        return E_FAIL;
    
    m_pVIBuffer->Bind_Buffers();
    
    m_pShader_Deferred->Begin(4);
    
    m_pVIBuffer->Render();
    
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    
    //=============================================================================================

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Blur_X"), m_pShader_Deferred, "g_EffectTexture")))
        return E_FAIL;
    
    m_pVIBuffer->Bind_Buffers();
    
    m_pShader_Deferred->Begin(5);
    
    m_pVIBuffer->Render();
    
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
    if (m_RenderObjects[RENDER_NONLIGHT].empty())
        return S_OK;

    m_RenderObjects[RENDER_NONLIGHT].clear();

    /* 사각형을 직교투영으로 화면에 꽉 채워서 그린다. */
    if (FAILED(m_pShader_PostProcessing->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_PostProcessing->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_PostProcessing->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    
    m_pGameInstance->Copy_BackBufferTexture();

    if (FAILED(m_pShader_PostProcessing->Bind_SRV("g_BackBufferTexture", m_pGameInstance->Get_BackBufferCopySRV())))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Blur_Y"), m_pShader_PostProcessing, "g_BlurTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Effect"), m_pShader_PostProcessing, "g_EffectTexture")))
        return E_FAIL;
       
    m_pShader_PostProcessing->Begin(0);
    
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();



    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    
    m_pGameInstance->Render_MRT(TEXT("MRT_GameObjects"), m_pShader_Deferred, m_pVIBuffer);
    m_pGameInstance->Render_MRT(TEXT("MRT_LightAcc"), m_pShader_Deferred, m_pVIBuffer);
    m_pGameInstance->Render_MRT(TEXT("MRT_Effect"), m_pShader_Deferred, m_pVIBuffer);
    m_pGameInstance->Render_MRT(TEXT("MRT_Blur_Y"), m_pShader_Deferred, m_pVIBuffer);

    for (auto& pComponent : m_DebugCom)
    {
        if (pComponent == nullptr)
            continue;

        pComponent->Render();

        Safe_Release(pComponent);
    }

    m_DebugCom.clear();

    return S_OK;
}
#endif // _DEBUG
CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pInstance);
    }
    
    return pInstance;
}

void CRenderer::Free()
{
    Safe_Release(m_pShader_Deferred);
    Safe_Release(m_pShader_PostProcessing);
    Safe_Release(m_pVIBuffer);
    Safe_Release(m_pGameInstance);

    for (size_t i = 0; i < RENDER_END; i++)
    {
        for (auto& pGameObject : m_RenderObjects[i])
            Safe_Release(pGameObject);

        m_RenderObjects[i].clear();
    }

    for (auto& pComponent : m_DebugCom)
    {
        if (pComponent == nullptr)
            continue;

        Safe_Release(pComponent);
    }

    m_DebugCom.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

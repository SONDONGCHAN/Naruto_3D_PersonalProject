#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->Mouse_Down(DIM_RB))
	//{
	//	CImgUI_Manager::GetInstance()->Set_ImGui_Desc(Picking_Terrain());
	//}
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Add_Component()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_DIFFUSE]))))
		return E_FAIL;

	/* Com_Mask */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
		return E_FAIL;

	/* Com_Brush */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Bind_Mask_Info();

	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == pLightDesc->eType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
		iPassIndex = 1;
	}
	else if (LIGHT_DESC::TYPE_POINT == pLightDesc->eType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 0;
	}

	if (FAILED(m_pShaderCom->Begin(iPassIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_Mask_Info()
{
	if (m_PickPos.x != 0.f || m_PickPos.z != 0.f)
	{
		_float4 vPos = { m_PickPos.x,m_PickPos.y, m_PickPos.z,1.f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBrushPos", &vPos, sizeof(_float4))))
			return E_FAIL;
	}

	//_float2 vScale = CImGui_Manager::GetInstance()->Get_ImGui_Desc().m_vTextureScale;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrushRangeX", &vScale.x, sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrushRangeZ", &vScale.y, sizeof(_float))))
	//	return E_FAIL;

	
	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pTexturecom : m_pTextureCom)
		Safe_Release(pTexturecom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}

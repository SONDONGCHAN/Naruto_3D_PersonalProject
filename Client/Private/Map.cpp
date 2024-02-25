#include "stdafx.h"
#include "Map.h"
#include "GameInstance.h"

#include "Deco.h"


CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CMap::CMap(const CMap& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMap::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CMap::Initialize(void* pArg)
{
	m_pMapIndex = (_uint*)pArg;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CMap::Priority_Tick(_float fTimeDelta)
{
}

void CMap::Tick(_float fTimeDelta)
{
	m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMap::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CMap::Render()
{
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderOutLine, i, 1, "g_DiffuseTexture")))
			return E_FAIL;
	
		if (FAILED(m_pShaderOutLine->Begin(0)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_uint Index = { 0 };

		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 6, "g_NormalTexture")))
			Index = 0;
		else
			Index = 1;

		if (FAILED(m_pShaderCom->Begin(Index)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CMap::Add_Model_Component()
{
	if (*m_pMapIndex == 0)
	{
		/* Com_Model_Map_Stadium */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Map_Stadium"),
			TEXT("Com_Model_Map_Stadium"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		/* Com_Navigatiom */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navi_Map_Stadium"),
			TEXT("Com_Navigatiom"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
	}
	else if (*m_pMapIndex == 1)
	{
		/* Com_Model_Map_Konoha_Village */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Map_Konaha_Village"),
			TEXT("Com_Model_Map_Konoha_Village"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		/* Com_Navigatiom */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navi_Map_Konoha_Village"),
			TEXT("Com_Navigatiom"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CMap::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_OutLine"),
		TEXT("Com_Shader_OutLine"), reinterpret_cast<CComponent**>(&m_pShaderOutLine))))
		return E_FAIL;

	Add_Model_Component();

	return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
{
	_float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
	_float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
	_float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();

	_float			fAlphaDiscard = 0.f;


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaDiscard", &fAlphaDiscard, sizeof(_float))))
		return E_FAIL;

	///////////////

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderOutLine, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShaderOutLine->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderOutLine->Bind_RawValue("g_fAlphaDiscard", &fAlphaDiscard, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap* pInstance = new CMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderOutLine);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}

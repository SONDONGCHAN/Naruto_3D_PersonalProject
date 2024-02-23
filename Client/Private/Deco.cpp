#include "stdafx.h"
#include "Deco.h"
#include "GameInstance.h"

CDeco::CDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDeco::CDeco(const CDeco& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDeco::Initialize_Prototype()
{
	return S_OK;

}

HRESULT CDeco::Initialize(void* pArg)
{
	m_pDecoDesc = (DECO_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_Model_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_pDecoDesc->fGameObjectScale, m_pDecoDesc->fGameObjectScale, m_pDecoDesc->fGameObjectScale);
	m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f}, XMConvertToRadians(m_pDecoDesc->fRot.y));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_pDecoDesc->fpos.x, m_pDecoDesc->fpos.y, m_pDecoDesc->fpos.z, 1.f));


	return S_OK;
}

void CDeco::Priority_Tick(_float fTimeDelta)
{
}

void CDeco::Tick(_float fTimeDelta)
{
}

void CDeco::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CDeco::Render()
{
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, 1 , "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CDeco::Add_Model_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Deco_Fence01"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeco::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeco::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CDeco* CDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeco* pInstance = new CDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDeco::Clone(void* pArg)
{
	CDeco* pInstance = new CDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeco::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
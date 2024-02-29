#include "Effect_Mesh.h"
#include "GameInstance.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Mesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
	m_MyDesc.MyType		= pDesc->MyType;
	m_MyDesc.vMyScale = pDesc->vMyScale;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Mesh::Priority_Tick(_float fTimeDelta)
{
}

void CEffect_Mesh::Tick(_float fTimeDelta)
{
	Move_UV(fTimeDelta);
}

void CEffect_Mesh::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_Mesh::Render()
{
	if (m_MyDesc.MyType == EFFECT_FIREBALL_MAIN)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
			
			if (FAILED(Bind_ShaderResources())) // 투영, 뷰만 바인딩
				return E_FAIL;

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
					return E_FAIL;
				
			for (_uint j = 0; j < iNumMeshes; j++)
			{
				m_vTextures[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0);

				if (FAILED(m_pShaderCom->Bind_RawValue("g_UVMovement", &m_vUVMovement, sizeof(_float2))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
					return E_FAIL;

				m_fBrightness = 3.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;			
			}

		}

	}
	else if (m_MyDesc.MyType == EFFECT_FIREBALL_RING)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
			
			if (FAILED(Bind_ShaderResources())) // 투영, 뷰만 바인딩
				return E_FAIL;
			
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_StartMat)))
				return E_FAIL;

			for (_uint j = 0; j < iNumMeshes; j++)
			{
				m_vTextures[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0);
			
				if (FAILED(m_pShaderCom->Bind_RawValue("g_UVMovement", &m_vUVMovement, sizeof(_float2))))
					return E_FAIL;
			
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))


					return E_FAIL;
			
				m_fBrightness = 3.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
			
				if (FAILED(m_pShaderCom->Begin(1)))
					return E_FAIL;
			
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}

		}
	}

	return S_OK;
}

void CEffect_Mesh::State_Tick(_fmatrix WorldMatrix)
{
	_matrix scalingMatrix = XMMatrixScalingFromVector(m_MyDesc.vMyScale);

	_matrix scaledWorldMatrix = scalingMatrix * WorldMatrix;
	
	_float4x4 Effect_WorldMat;
	XMStoreFloat4x4(&Effect_WorldMat, scaledWorldMatrix);
	
	m_pTransformCom->Set_World(Effect_WorldMat);
}

void CEffect_Mesh::Move_UV(_float fTimeDelta)
{
	m_vUVMovement.x += m_vUVSpeed * fTimeDelta;
	m_vUVMovement.y += m_vUVSpeed * fTimeDelta;

	if (m_vUVMovement.x > 1.f)
		m_vUVMovement.x -= 1.f;

	if (m_vUVMovement.y > 1.f)
		m_vUVMovement.y -= 1.f;
}

void CEffect_Mesh::Get_StartMat(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_StartMat, WorldMatrix);
}

HRESULT CEffect_Mesh::Add_Component()
{
	// 셰이더
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (m_MyDesc.MyType == EFFECT_FIREBALL_MAIN)
	{
		// 모델
		CModel* m_pModel_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Main"),
			TEXT("Com_Model_Main"), reinterpret_cast<CComponent**>(&m_pModel_Main))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Main);
		
		CModel* m_pModel_Aura;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Aura"),
			TEXT("Com_Model_Aura"), reinterpret_cast<CComponent**>(&m_pModel_Aura))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Aura);
		
		// 텍스쳐
		CTexture* m_pTexture_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Main_2"),
			TEXT("Com_Texture_Main"), reinterpret_cast<CComponent**>(&m_pTexture_Main))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Main);		
		

	}
	if (m_MyDesc.MyType == EFFECT_FIREBALL_RING)
	{
		CModel* m_pModel_Ring;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Ring"),
			TEXT("Com_Model_Ring"), reinterpret_cast<CComponent**>(&m_pModel_Ring))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Ring);

		CTexture* m_pTexture_Ring;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Main"),
			TEXT("Com_Texture_Ring"), reinterpret_cast<CComponent**>(&m_pTexture_Ring))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Ring);
	}


	return S_OK;
}

HRESULT CEffect_Mesh::Bind_ShaderResources()
{
	_float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
	_float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	return S_OK;
}


CEffect_Mesh* CEffect_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Mesh::Clone(void* pArg)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Mesh::Free()
{
	__super::Free();

	for (auto pModel : m_vModels)
		Safe_Release(pModel);
	m_vModels.clear();

	for (auto pTexture : m_vTextures)
		Safe_Release(pTexture);
	m_vTextures.clear();

	Safe_Release(m_pShaderCom);
}


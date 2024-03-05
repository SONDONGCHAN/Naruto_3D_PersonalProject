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

	m_MyDesc = *pDesc;

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
	Move_UV(m_MyDesc.MyUVOption,fTimeDelta);


	if (m_bEndTrigger)
	{
		if (m_fDiscardColor > 1.f)
		{
			m_bEndTrigger = false;
			return;
		}
		m_fDiscardColor += fTimeDelta* m_fDiscardSpeed;
	}
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
			
			if (FAILED(Bind_ShaderResources()))
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

				m_fBrightness = 1.f;
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
			
			if (FAILED(Bind_ShaderResources()))
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
			
				_float4		vColor = { 1.f, 110.f / 255.f, 0.f, 0.5f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
			
				if (FAILED(m_pShaderCom->Begin(1)))
					return E_FAIL;
			
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_CHIDORI)
	{
		m_ModelCountCheck += (1.f/60.f);
		if (m_ModelCountCheck > 0.05f)
		{
			m_ModelCount++;
			m_ModelCountCheck = 0.f;
		}
		if (m_ModelCount >= m_vModels.size())
			m_ModelCount = 0;

		for (_uint i = m_ModelCount; i < m_ModelCount + 3; i++)
		{
			_uint ModelIndex = i;

			if (i >= m_vModels.size())
				ModelIndex -= m_vModels.size();
			
			_uint	iNumMeshes = m_vModels[ModelIndex]->Get_NumMeshes();
			
			if (FAILED(Bind_ShaderResources()))
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
	
				_float4		vColor = { 13.f/255.f, 154.f/255.f, 1.f, 0.9f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (FAILED(m_pShaderCom->Begin(1)))
					return E_FAIL;
	
				if (FAILED(m_vModels[ModelIndex]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_SHOCKWAVE)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				_float4		vColor = { 0.f, 0.f, 0.f, 0.5f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(2)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_KAMUI)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
		
			if (FAILED(Bind_ShaderResources()))
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
		
				_float4		vColor = { 0.f, 0.f, 0.f, 0.5f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
		
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
		
				if (FAILED(m_pShaderCom->Begin(3)))
					return E_FAIL;
		
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENGUN_MAIN)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
		
			if (FAILED(Bind_ShaderResources()))
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
		
				_float4		vColor = { 121.f/255.f, 237.f/255.f, 1.f, 0.9f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
		
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
		
				if (FAILED(m_pShaderCom->Begin(4)))
					return E_FAIL;
		
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENGUN_CHARGE)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
	
			if (FAILED(Bind_ShaderResources()))
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
	
				_float4		vColor = { 1.f, 1.f, 1.f, 0.3f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (FAILED(m_pShaderCom->Begin(5)))
					return E_FAIL;
	
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_RUSH)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				_float4		vColor = { 1.f, 1.f, 1.f, 0.2f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(6)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_BOOM)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.7f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(7)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_MAIN )
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
	
			if (FAILED(Bind_ShaderResources()))
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
	
				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.9f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (FAILED(m_pShaderCom->Begin(4)))
					return E_FAIL;
	
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_WING )
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
		
			if (FAILED(Bind_ShaderResources()))
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
		
				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.6f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
		
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
		
				if (FAILED(m_pShaderCom->Begin(8)))
					return E_FAIL;
		
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_RING)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.3f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(4)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_BOOM )
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
	
			if (FAILED(Bind_ShaderResources()))
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
	
				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.7f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (m_bEndTrigger == false)
				{
					if (FAILED(m_pShaderCom->Begin(4)))
						return E_FAIL;
				}
				else
				{
					m_vTextures[1]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0);

					if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiscardColor", &m_fDiscardColor, sizeof(_float))))
						return E_FAIL;
					
					if (FAILED(m_pShaderCom->Begin(9)))
						return E_FAIL;
				}

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_DECO || m_MyDesc.MyType == EFFECT_RASENGUNSUPER_NOISE)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
	
			if (FAILED(Bind_ShaderResources()))
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
	
				_float4		vColor = { 1.f, 1.f, 1.f, 1.f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (m_bEndTrigger == false)
				{
					if (FAILED(m_pShaderCom->Begin(10)))
						return E_FAIL;
				}
				else
				{
					m_vTextures[1]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0);
	
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiscardColor", &m_fDiscardColor, sizeof(_float))))
						return E_FAIL;
	
					if (FAILED(m_pShaderCom->Begin(11)))
						return E_FAIL;
				}
	
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUNSUPER_MAIN)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
				return E_FAIL;

			for (_uint j = 0; j < iNumMeshes; j++)
			{
				//m_vTextures[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0);
			
				if (FAILED(m_pShaderCom->Bind_RawValue("g_UVMovement", &m_vUVMovement, sizeof(_float2))))
					return E_FAIL;
			
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
					return E_FAIL;
			
				_float4		vColor = { 121.f / 255.f, 237.f / 255.f, 1.f, 0.7f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
			
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
			
				if (m_bEndTrigger == false)
				{
					if (FAILED(m_pShaderCom->Begin(12)))
						return E_FAIL;
				}
				else
				{
					m_vTextures[0]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0);
			
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiscardColor", &m_fDiscardColor, sizeof(_float))))
						return E_FAIL;
			
					if (FAILED(m_pShaderCom->Begin(13)))
						return E_FAIL;
				}
			
				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_WOOD_SWAP)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(14)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}	
	}
	else if (m_MyDesc.MyType == EFFECT_KURAMA_SCRATCH)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				_float4		vColor = { 138.f/255.f, 43.f/255.f,226.f/255.f, 1.0f };

				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(15)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_KURAMA_CLAW)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources()))
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

				//_float4		vColor = { 1.f, 110.f / 255.f, 0.f, 1.f };
				_float4		vColor = { 0.f, 0.f, 0.f, 1.f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;

				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(16)))
					return E_FAIL;

				if (FAILED(m_vModels[i]->Render(j)))
					return E_FAIL;
			}
		}
	}

	else if (m_MyDesc.MyType == EFFECT_KURAMA_RUSH)
	{
		for (_uint i = 0; i < m_vModels.size(); i++)
		{
			_uint	iNumMeshes = m_vModels[i]->Get_NumMeshes();
	
			if (FAILED(Bind_ShaderResources()))
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
	
				//_float4		vColor = { 1.f, 110.f / 255.f, 0.f, 1.f };
				_float4		vColor = { 0.f, 0.f, 0.f, 1.f };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
					return E_FAIL;
	
				m_fBrightness = 1.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
					return E_FAIL;
	
				if (FAILED(m_pShaderCom->Begin(17)))
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
	_matrix scalingMatrix = XMMatrixScalingFromVector(vCurrentScale);

	_matrix scaledWorldMatrix = scalingMatrix * WorldMatrix;
	
	if (m_MyDesc.MySpinOption == SPIN_X)
	{
		_matrix SpinMatrix = XMMatrixRotationX(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x += m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_Y)
	{
		_matrix SpinMatrix  = XMMatrixRotationY(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x += m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_Z)
	{
		_matrix SpinMatrix = XMMatrixRotationZ(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x += m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_X_INVERSE)
	{
		_matrix SpinMatrix = XMMatrixRotationX(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x -= m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_Y_INVERSE)
	{
		_matrix SpinMatrix = XMMatrixRotationY(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x -= m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_Z_INVERSE)
	{
		_matrix SpinMatrix = XMMatrixRotationZ(XMConvertToRadians(m_vRatation.x));
		scaledWorldMatrix = SpinMatrix * scaledWorldMatrix;
		m_vRatation.x -= m_fSpinSpeed;
	}
	else if (m_MyDesc.MySpinOption == SPIN_RANDOM )
	{
		if (vCurrentScale.m128_f32[0] == 0.f)
		{
			m_vRatation.x = rand() % 360;
			m_vRatation.y = rand() % 360;
			m_vRatation.z = rand() % 360;
		}		
		_matrix SpinMatrixX = XMMatrixRotationX(XMConvertToRadians(m_vRatation.x));
		_matrix SpinMatrixY = XMMatrixRotationY(XMConvertToRadians(m_vRatation.y));
		_matrix SpinMatrixZ = XMMatrixRotationZ(XMConvertToRadians(m_vRatation.z));

		_matrix SpinMatrixFinal = SpinMatrixX * SpinMatrixY * SpinMatrixZ;

		scaledWorldMatrix = SpinMatrixFinal * scaledWorldMatrix;
	}

	_float4x4 Effect_WorldMat;
	XMStoreFloat4x4(&Effect_WorldMat, scaledWorldMatrix);
	
	m_pTransformCom->Set_World(Effect_WorldMat);
}

void CEffect_Mesh::Move_UV(EFFECT_OPTION_UV UVOption, _float fTimeDelta)
{
	if (UVOption == MOVE_X)
	{
		m_vUVMovement.x += m_vUVSpeed * fTimeDelta;

		if (m_vUVMovement.x > 1.f)
			m_vUVMovement.x -= 1.f;
	}
	else if (UVOption == MOVE_X_INVERSE)
	{
		m_vUVMovement.x -= m_vUVSpeed * fTimeDelta;

		if (m_vUVMovement.x < 0.f)
			m_vUVMovement.x += 1.f;
	}
	else if (UVOption == MOVE_Y)
	{
		m_vUVMovement.y += m_vUVSpeed * fTimeDelta;

		if (m_vUVMovement.y > 1.f)
			m_vUVMovement.y -= 1.f;
	}
	else if (UVOption == MOVE_Y_INVERSE)
	{
		m_vUVMovement.y -= m_vUVSpeed * fTimeDelta;

		if (m_vUVMovement.y < 0.f)
			m_vUVMovement.y += 1.f;
	}
	else if (UVOption == MOVE_All)
	{
		m_vUVMovement.x += m_vUVSpeed * fTimeDelta;
		m_vUVMovement.y += m_vUVSpeed * fTimeDelta;

		if (m_vUVMovement.x > 1.f)
			m_vUVMovement.x -= 1.f;

		if (m_vUVMovement.y > 1.f)
			m_vUVMovement.y -= 1.f;
	}
}

void CEffect_Mesh::Start_Trigger()
{
	if (EFFECT_FIREBALL_MAIN == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_FIREBALL_RING == m_MyDesc.MyType)
	{
		m_ScalingRatio = 1.f;
		vCurrentScale = m_MyDesc.vMyScale;
	}
	else if (EFFECT_SHOCKWAVE == m_MyDesc.MyType)
	{
		m_fAlpha = 1.f;
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_KAMUI == m_MyDesc.MyType)
	{
		m_ScalingRatio = 1.f;
		vCurrentScale = m_MyDesc.vMyScale;
	}
	else if (EFFECT_RASENGUN_MAIN == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_RASENGUN_CHARGE == m_MyDesc.MyType)
	{
		m_ScalingRatio = 1.f;
		vCurrentScale = m_MyDesc.vMyScale;
	}
	else if (EFFECT_RASENGUN_BOOM == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_RASENSHURIKEN_MAIN == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_WING == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_RING == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_BOOM == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if ( EFFECT_RASENSHURIKEN_DECO == m_MyDesc.MyType )
	{
		m_fAlpha = 1.f;
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_RASENGUNSUPER_MAIN == m_MyDesc.MyType || EFFECT_RASENGUNSUPER_NOISE == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_WOOD_SWAP == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
	}
	else if (EFFECT_KURAMA_SCRATCH == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
	}
	else if (EFFECT_KURAMA_CLAW == m_MyDesc.MyType)
	{
		m_ScalingRatio = 0.f;
		vCurrentScale = _vector{ 0.f, 0.f, 0.f, 1.f };
		m_fAlpha = 1.f;
	}
	else if (m_MyDesc.MyType == EFFECT_KURAMA_RUSH)
	{
		m_fAlpha = 1.f;
	}
}

void CEffect_Mesh::End_Trigger(_float fDiscardSpeed)
{
	m_fDiscardColor = 0.f;
	m_fDiscardSpeed = fDiscardSpeed;
	m_bEndTrigger = true;
}

void CEffect_Mesh::Scale_Change(_float fTimeDelta)
{
	if(EFFECT_FIREBALL_MAIN == m_MyDesc.MyType)
	{ 
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_FIREBALL_RING == m_MyDesc.MyType)
	{		
		if (vCurrentScale.m128_f32[0] > 0)
		{
			if (m_ScalingRatio > 0.f)
				m_ScalingRatio -= m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 0.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_SHOCKWAVE == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
			{
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			}
			else
				m_ScalingRatio = 1.f;
		
			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
			m_fAlpha	= Lerp(1.f, 0.f, m_ScalingRatio);			
		}
	}
	else if (EFFECT_KAMUI == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] > 0)
		{
			if (m_ScalingRatio > 0.f)
				m_ScalingRatio -= m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 0.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_RASENGUN_MAIN == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_RASENGUN_CHARGE == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] > 0)
		{
			if (m_ScalingRatio > 0.f)
				m_ScalingRatio -= m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 0.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_RASENGUN_BOOM == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_RASENSHURIKEN_MAIN == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_WING == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_RING == m_MyDesc.MyType || EFFECT_RASENSHURIKEN_BOOM == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_RASENSHURIKEN_DECO == m_MyDesc.MyType )
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
			{
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			}
			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
			m_fAlpha = Lerp(1.f, 0.f, m_ScalingRatio);
		}
		else
		{
			m_ScalingRatio = 0.f;
			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
			
		}
	}
	else if (EFFECT_RASENGUNSUPER_MAIN == m_MyDesc.MyType  || EFFECT_RASENGUNSUPER_NOISE == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (EFFECT_WOOD_SWAP == m_MyDesc.MyType)
	{
		if (m_ScalingRatio < 1.f)
			m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
		else
			m_ScalingRatio = 1.f;

		m_fAlpha = Lerp(1.f, 0.f, m_ScalingRatio);
	}

	else if (EFFECT_KURAMA_SCRATCH == m_MyDesc.MyType)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
		}
	}
	else if (m_MyDesc.MyType == EFFECT_KURAMA_CLAW)
	{
		if (vCurrentScale.m128_f32[0] < m_MyDesc.vMyScale.m128_f32[0])
		{
			if (m_ScalingRatio <= 1.f)
				m_ScalingRatio += m_ScalingSpeed * fTimeDelta;
			else
				m_ScalingRatio = 1.f;

			vCurrentScale = m_MyDesc.vMyScale * Lerp(0.f, 1.f, m_ScalingRatio);
			
		}
		else
		{
			if (m_fAlpha > 0.f)
				m_fAlpha -= m_AlphaSpeed * fTimeDelta;
			
			else
				m_fAlpha = 0.f;
		}
	}
	else if (m_MyDesc.MyType == EFFECT_KURAMA_RUSH)
	{
		if (m_fAlpha > 0.f)
			m_fAlpha -= m_AlphaSpeed * fTimeDelta;
		
		else
			m_fAlpha = 0.f;
	}
}

_float CEffect_Mesh::Lerp(_float start, _float end, _float ratio)
{
	return start + ratio * (end - start);
}

HRESULT CEffect_Mesh::Add_Component()
{
	// ¼ÎÀÌ´õ
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (m_MyDesc.MyType == EFFECT_FIREBALL_MAIN)
	{
		// ¸ðµ¨
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
		
		// ÅØ½ºÃÄ
		CTexture* m_pTexture_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireBall_Main_2"),
			TEXT("Com_Texture_Main"), reinterpret_cast<CComponent**>(&m_pTexture_Main))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Main);		
	}

	else if (m_MyDesc.MyType == EFFECT_FIREBALL_RING)
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

	else if (m_MyDesc.MyType == EFFECT_CHIDORI)
	{
		CModel* m_pModel_Chidori_1;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_1"),
			TEXT("Com_Model_Chidori_1"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_1))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_1);

		CModel* m_pModel_Chidori_2;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_2"),
			TEXT("Com_Model_Chidori_2"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_2))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_2);

		CModel* m_pModel_Chidori_3;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_3"),
			TEXT("Com_Model_Chidori_3"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_3))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_3);

		CModel* m_pModel_Chidori_4;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_4"),
			TEXT("Com_Model_Chidori_4"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_4))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_4);

		CModel* m_pModel_Chidori_5;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_5"),
			TEXT("Com_Model_Chidori_5"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_5))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_5);

		CModel* m_pModel_Chidori_6;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_6"),
			TEXT("Com_Model_Chidori_6"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_6))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_6);

		CModel* m_pModel_Chidori_7;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_7"),
			TEXT("Com_Model_Chidori_7"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_7))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_7);

		CModel* m_pModel_Chidori_8;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_8"),
			TEXT("Com_Model_Chidori_8"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_8))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_8);

		CModel* m_pModel_Chidori_9;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chidori_9"),
			TEXT("Com_Model_Chidori_9"), reinterpret_cast<CComponent**>(&m_pModel_Chidori_9))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Chidori_9);

		
		CTexture* m_pTexture_Chidori_Mask;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chidori_Mask"),
			TEXT("Com_Texture_Chidori_Mask"), reinterpret_cast<CComponent**>(&m_pTexture_Chidori_Mask))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Chidori_Mask);

		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 1.f;
	}

	else if (m_MyDesc.MyType == EFFECT_SHOCKWAVE)
	{
		CModel* m_pModel_ShockWave;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Ring"),
			TEXT("Com_Model_Ring"), reinterpret_cast<CComponent**>(&m_pModel_ShockWave))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_ShockWave);
	
		CTexture* m_pTexture_ShockWave;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockWave_Mask"),
			TEXT("Com_Texture_ShockWave_Mask"), reinterpret_cast<CComponent**>(&m_pTexture_ShockWave))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_ShockWave);
	}

	else if (m_MyDesc.MyType == EFFECT_KAMUI)
	{
		CModel* m_pModel_Kamui;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kamui"),
			TEXT("Com_Model_Kamui"), reinterpret_cast<CComponent**>(&m_pModel_Kamui))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Kamui);
		
		CTexture* m_pTexture_Kamui;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_kamui_Boom"),
			TEXT("Com_Texture_Kamui"), reinterpret_cast<CComponent**>(&m_pTexture_Kamui))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Kamui);

		m_ScalingSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 2.f;
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_MAIN)
	{
		CModel* m_pModel_Rasengun_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBall_Main"),
			TEXT("Com_Model_Rasengun_Main"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Main))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Main);
		
		CTexture* m_pTexture_Rasengun_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Main"),
			TEXT("Com_Texture_Rasengun_Main"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Main))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Main);
		
		m_ScalingSpeed = 0.85f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 10.f;
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_CHARGE)
	{
		CModel* m_pModel_Rasengun_Charge;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Charge"),
			TEXT("Com_Model_Rasengun_Charge"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Charge))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Charge);
		
		CTexture* m_pTexture_Rasengun_Charge;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Rush"),
			TEXT("Com_Texture_Rasengun_Charge"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Charge))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Charge);
		
		m_ScalingSpeed = 1.2f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 2.f;
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_RUSH)
	{
		CModel* m_pModel_Rasengun_Rush;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Rush"),
			TEXT("Com_Model_Rasengun_Rush"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Rush))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Rush);
		
		CTexture* m_pTexture_Rasengun_Rush;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Rush"),
			TEXT("Com_Texture_Rasengun_Rush"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Rush))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Rush);
		
		m_ScalingSpeed = 1.2f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 1.5f;
	}

	else if (m_MyDesc.MyType == EFFECT_RASENGUN_BOOM)
	{
		CModel* m_pModel_Rasengun_Boom;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Boom"),
			TEXT("Com_Model_Rasengun_Boom"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Boom))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Boom);

		CTexture* m_pTexture_Rasengun_Boom;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Boom"),
			TEXT("Com_Texture_Rasengun_Boom"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Boom))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Boom);

		m_ScalingSpeed = 3.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 0.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_MAIN)
	{
		CModel* m_pModel_RasenShuriken_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Main"),
			TEXT("Com_Model_RasenShuriken_Main"), reinterpret_cast<CComponent**>(&m_pModel_RasenShuriken_Main))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_RasenShuriken_Main);
	
		CTexture* m_pTexture_RasenShuriken_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Main"),
			TEXT("Com_Texture_RasenShuriken_Main"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Main))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Main);
	
		m_ScalingSpeed = 1.3f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 10.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_WING)
	{
		CModel* m_pModel_RasenShuriken_Wing;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Wing"),
			TEXT("Com_Model_RasenShuriken_Wing"), reinterpret_cast<CComponent**>(&m_pModel_RasenShuriken_Wing))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_RasenShuriken_Wing);
		
		CTexture* m_pTexture_RasenShuriken_Wing;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Wing"),
			TEXT("Com_Texture_RasenShuriken_Wing"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Wing))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Wing);
		
		m_ScalingSpeed = 1.3f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 0.f;
		m_fSpinSpeed = 20.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_RING)
	{
		CModel* m_pModel_RasenShuriken_Ring;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Ring"),
			TEXT("Com_Model_RasenShuriken_Ring"), reinterpret_cast<CComponent**>(&m_pModel_RasenShuriken_Ring))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_RasenShuriken_Ring);
	
		CTexture* m_pTexture_RasenShuriken_Ring;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Ring"),
			TEXT("Com_Texture_RasenShuriken_Ring"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Ring))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Ring);
	
		m_ScalingSpeed = 1.3f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 1.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_BOOM)
	{
		CModel* m_pModel_RasenShuriken_Boom;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Main"),
			TEXT("Com_Model_RasenShuriken_Boom"), reinterpret_cast<CComponent**>(&m_pModel_RasenShuriken_Boom))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_RasenShuriken_Boom);
	
		CTexture* m_pTexture_RasenShuriken_Boom;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Main"),
			TEXT("Com_Texture_RasenShuriken_Boom"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Boom))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Boom);

		CTexture* m_pTexture_RasenShuriken_Dissolve;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Dissolve"),
			TEXT("Com_Texture_RasenShuriken_Dissolve"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Dissolve))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Dissolve);

		m_ScalingSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 5.f;
	}

	else if (m_MyDesc.MyType == EFFECT_RASENSHURIKEN_DECO)
	{
		CModel* m_pModel_RasenShuriken_Deco;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RasenShuriken_Deco"),
			TEXT("Com_Model_RasenShuriken_Deco"), reinterpret_cast<CComponent**>(&m_pModel_RasenShuriken_Deco))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_RasenShuriken_Deco);

		CTexture* m_pTexture_RasenShuriken_Deco;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Deco"),
			TEXT("Com_Texture_RasenShuriken_Deco"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Deco))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Deco);

		CTexture* m_pTexture_RasenShuriken_Dissolve;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Dissolve"),
			TEXT("Com_Texture_RasenShuriken_Dissolve"), reinterpret_cast<CComponent**>(&m_pTexture_RasenShuriken_Dissolve))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_RasenShuriken_Dissolve);
	
		m_ScalingSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 1.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENGUNSUPER_MAIN)
	{
		CModel* m_pModel_Rasengun_Super_Main;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Main"),
			TEXT("Com_Model_Rasengun_Super_Main"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Super_Main))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Super_Main);
		
		CTexture* m_pTexture_Rasengun_Super_Dissolve;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Dissolve"),
			TEXT("Com_Texture_Rasengun_Super_Dissolve"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Super_Dissolve))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Super_Dissolve);
		
		m_ScalingSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 5.f;
	}
	else if (m_MyDesc.MyType == EFFECT_RASENGUNSUPER_NOISE)
	{
		CModel* m_pModel_Rasengun_Super_Noise;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rasengun_Super"),
			TEXT("Com_Model_Rasengun_Super_Noise"), reinterpret_cast<CComponent**>(&m_pModel_Rasengun_Super_Noise))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Rasengun_Super_Noise);

		CTexture* m_pTexture_Rasengun_Super_Noise;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasengun_Super_Noise"),
			TEXT("Com_Texture_Rasengun_Super_Noise"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Super_Noise))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Super_Noise);
	
		CTexture* m_pTexture_Rasengun_Super_Noise_Dissolve;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Dissolve"),
			TEXT("Com_Texture_Rasengun_Super_Noise_Dissolve"), reinterpret_cast<CComponent**>(&m_pTexture_Rasengun_Super_Noise_Dissolve))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Rasengun_Super_Noise_Dissolve);
	
		m_ScalingSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 3.f;
		m_fSpinSpeed = 0.f;
	}

	else if (m_MyDesc.MyType == EFFECT_WOOD_SWAP)
	{
		CModel* m_pModel_Wood_Swap;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wood_Swap"),
			TEXT("Com_Model_Wood_Swap"), reinterpret_cast<CComponent**>(&m_pModel_Wood_Swap))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Wood_Swap);
	
		CTexture* m_pTexture_Wood_Swap;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wood_Swap"),
			TEXT("Com_Texture_Wood_Swap"), reinterpret_cast<CComponent**>(&m_pTexture_Wood_Swap))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Wood_Swap);
	
		m_ScalingSpeed = 0.5f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 0.f;
		m_fSpinSpeed = 1.f;
	}

	else if (m_MyDesc.MyType == EFFECT_KURAMA_SCRATCH)
	{
		CModel* m_pModel_Kurama_Scratch;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Scratch"),
			TEXT("Com_Model_Kurama_Scratch"), reinterpret_cast<CComponent**>(&m_pModel_Kurama_Scratch))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Kurama_Scratch);
    
		CTexture* m_pTexture_Kurama_Scratch;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_kamui_Boom"),
			TEXT("Com_Texture_Kurama_Scratch"), reinterpret_cast<CComponent**>(&m_pTexture_Kurama_Scratch))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Kurama_Scratch);
    
		m_ScalingSpeed = 1.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 2.f;
		m_fSpinSpeed = 0.f;
	}

	else if (m_MyDesc.MyType == EFFECT_KURAMA_CLAW)
	{
		CModel* m_pModel_Kurama_Claw;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Claw"),
			TEXT("Com_Model_Kurama_Claw"), reinterpret_cast<CComponent**>(&m_pModel_Kurama_Claw))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Kurama_Claw);
		
		CTexture* m_pTexture_Kurama_Claw;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rasenshuriken_Deco"),
			TEXT("Com_Texture_Kurama_Claw"), reinterpret_cast<CComponent**>(&m_pTexture_Kurama_Claw))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Kurama_Claw);

		m_ScalingSpeed = 4.f;
		m_AlphaSpeed = 4.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 0.f;
		m_fSpinSpeed = 0.f;
	}

	else if (m_MyDesc.MyType == EFFECT_KURAMA_RUSH)
	{
		CModel* m_pModel_Kurama_Rush;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kurama_Rush"),
			TEXT("Com_Model_Kurama_Claw"), reinterpret_cast<CComponent**>(&m_pModel_Kurama_Rush))))
			return E_FAIL;
		m_vModels.push_back(m_pModel_Kurama_Rush);
		
		CTexture* m_pTexture_Kurama_Rush;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Kurama_Rush"),
			TEXT("Com_Texture_Kurama_Claw"), reinterpret_cast<CComponent**>(&m_pTexture_Kurama_Rush))))
			return E_FAIL;
		m_vTextures.push_back(m_pTexture_Kurama_Rush);
		
		m_ScalingSpeed = 0.f;
		m_AlphaSpeed = 2.f;
		vCurrentScale = m_MyDesc.vMyScale;
		m_vUVSpeed = 0.f;
		m_fSpinSpeed = 0.f;

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


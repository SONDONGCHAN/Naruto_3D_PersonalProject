#include "UI_Player_Skills.h"

CUI_Player_Skills::CUI_Player_Skills(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Player_Skills::CUI_Player_Skills(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Skills::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Skills::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Skills::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Player_Skills::Tick(_float fTimeDelta)
{
}

void CUI_Player_Skills::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return;
}

HRESULT CUI_Player_Skills::Render()
{
	__super::Render();

	_float2 vBaseSize = { 65.f, 65.f };
	_uint i = { 0 };
	_uint iPassIndex = { 0 };
	for (auto pTextures : m_Textures)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI_Descs[i].vPos.x - g_iWinSizeX * 0.5f, -(m_UI_Descs[i].vPos.y) + g_iWinSizeY * 0.5f, 0.f, 1.f));
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
		
		if (FAILED(m_pTextureBase->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &vBaseSize, sizeof(_float2))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;
		
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;
		
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		////

		if (FAILED(pTextures->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_UI_Descs[i].vSize), sizeof(_float2))))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;
	
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;
	
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	
		i++;
	}

	return S_OK;
}

HRESULT CUI_Player_Skills::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Skill_Base"),
		TEXT("Com_Texture_Base"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL;


	UI_Player_Skills_DESC pUI_Desc_1;
	pUI_Desc_1.vPos = { 1180.f, 650.f };
	pUI_Desc_1.vSize = { 50.f, 50.f };
	m_UI_Descs.push_back(pUI_Desc_1);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Rasengun"),
		TEXT("Com_Texture_Icon_Rasengun"), reinterpret_cast<CComponent**>(&m_pTextureIcon_1))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureIcon_1);

	UI_Player_Skills_DESC pUI_Desc_2;
	pUI_Desc_2.vPos = { 1110.f, 650.f };
	pUI_Desc_2.vSize = { 50.f, 50.f };
	m_UI_Descs.push_back(pUI_Desc_2);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_RasenShuriken"),
		TEXT("Com_Texture_Icon_RasenShuriken"), reinterpret_cast<CComponent**>(&m_pTextureIcon_2))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureIcon_2);

	UI_Player_Skills_DESC pUI_Desc_3;
	pUI_Desc_3.vPos = { 1040.f, 650.f };
	pUI_Desc_3.vSize = { 50.f, 50.f };
	m_UI_Descs.push_back(pUI_Desc_3);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_WoodSwap"),
		TEXT("Com_Texture_Icon_WoodSwap"), reinterpret_cast<CComponent**>(&m_pTextureIcon_3))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureIcon_3);

	//UI_Player_Skills_DESC pUI_Desc_5;
	//pUI_Desc_5.vPos = { 910.f, 650.f };
	//pUI_Desc_5.vSize = { 70.f, 70.f };
	//m_UI_Descs.push_back(pUI_Desc_5);
	//if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Hp"),
	//	TEXT("Com_Texture_Status_Hp"), reinterpret_cast<CComponent**>(&m_pTextureIcon_4))))
	//	return E_FAIL;
	//m_Textures.push_back(m_pTextureIcon_4);

	return S_OK;
}

CUI_Player_Skills* CUI_Player_Skills::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Player_Skills* pInstance = new CUI_Player_Skills(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Skills");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CUI_Player_Skills::Clone(void* pArg)
{
	CUI_Player_Skills* pInstance = new CUI_Player_Skills(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Skills");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player_Skills::Free()
{
	for (auto pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	Safe_Release(m_pTextureBase);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}

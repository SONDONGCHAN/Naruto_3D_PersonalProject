#include "UI_Player_Status.h"

CUI_Player_Status::CUI_Player_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Player_Status::CUI_Player_Status(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Status::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Status::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Status::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Player_Status::Tick(_float fTimeDelta)
{
}

void CUI_Player_Status::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return;
}

HRESULT CUI_Player_Status::Render()
{
	__super::Render();

	_uint i = { 0 };
	_uint iPassIndex = { 0 };
	for (auto pTextures : m_Textures)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI_Descs[i].vPos.x - g_iWinSizeX * 0.5f, -(m_UI_Descs[i].vPos.y) + g_iWinSizeY * 0.5f, 0.f, 1.f));
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;

		if (FAILED(pTextures->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_UI_Descs[i].vSize), sizeof(_float2))))
			return E_FAIL;

		if (i == 2)
			iPassIndex = 1;

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

HRESULT CUI_Player_Status::Add_Component()
{
	UI_Player_Status_DESC pUI_Desc_1;
	pUI_Desc_1.vPos = { 230.f, 646.f };
	pUI_Desc_1.vSize = { 400.f, 102.f };
	m_UI_Descs.push_back(pUI_Desc_1);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Status"),
		TEXT("Com_Texture_Status"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL; 
	m_Textures.push_back(m_pTextureBase);
	
	UI_Player_Status_DESC pUI_Desc_2;
	pUI_Desc_2.vPos = { 110.f, 650.f };
	pUI_Desc_2.vSize = { 70.f, 70.f };
	m_UI_Descs.push_back(pUI_Desc_2);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Rasengun_Super"),
		TEXT("Com_Texture_Icon_Rasengun_Super"), reinterpret_cast<CComponent**>(&m_pTextureIcon))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureIcon);
	
	UI_Player_Status_DESC pUI_Desc_3;
	pUI_Desc_3.vPos = { 280.f, 666.f };
	pUI_Desc_3.vSize = { 370.f, 40.f };
	m_UI_Descs.push_back(pUI_Desc_3);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Hp"),
		TEXT("Com_Texture_Status_Hp"), reinterpret_cast<CComponent**>(&m_pTextureHp))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureHp);
	
	return S_OK;
}

CUI_Player_Status* CUI_Player_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Player_Status* pInstance = new CUI_Player_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_Status::Clone(void* pArg)
{
	CUI_Player_Status* pInstance = new CUI_Player_Status(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player_Status::Free()
{
	for (auto pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	Safe_Release(m_pVIBufferCom);

	__super::Free();
}

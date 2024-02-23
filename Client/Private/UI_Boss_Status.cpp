#include "UI_Boss_Status.h"

CUI_Boss_Status::CUI_Boss_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Boss_Status::CUI_Boss_Status(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Boss_Status::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Boss_Status::Initialize(void* pArg)
{
	UI_Boss_Hp_DESC* pHp_Desc = (UI_Boss_Hp_DESC*)pArg;
	m_pCurrent_Hp	= pHp_Desc->pCurrentHp;
	m_pMax_Hp		= pHp_Desc->pMaxHp;
	m_eMyCharacter	= pHp_Desc->eMyCharacter;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	return S_OK;
}

void CUI_Boss_Status::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Boss_Status::Tick(_float fTimeDelta)
{
}

void CUI_Boss_Status::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Boss_Status::Render()
{
	__super::Render();

	_uint i = { 0 };
	for (auto pTextures : m_Textures)
	{
		_uint iPassIndex = { 0 };

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI_Descs[i].vPos.x - g_iWinSizeX * 0.5f, -(m_UI_Descs[i].vPos.y) + g_iWinSizeY * 0.5f, 0.f, 1.f));
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;

		if (FAILED(pTextures->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_UI_Descs[i].vSize), sizeof(_float2))))
			return E_FAIL;


		if (i == 1)
		{
			iPassIndex = 1;

			_float	fRatio = *m_pCurrent_Hp / *m_pMax_Hp;

			if (fRatio <= 0)
				fRatio = 0;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
				return E_FAIL;

			_vector vColor = { 0.f, 0.7f, 0.f, 1.f };

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
				return E_FAIL;
		}

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

HRESULT CUI_Boss_Status::Add_Component()
{
	UI_Boss_UI_DESC pUI_Desc_1;
	pUI_Desc_1.vPos = { g_iWinSizeX *0.5f, 50.f };
	pUI_Desc_1.vSize = { 600.f, 90.f };
	m_UI_Descs.push_back(pUI_Desc_1);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Base_Hp"),
		TEXT("Com_Texture_Base"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureBase);

	UI_Boss_UI_DESC pUI_Desc_2;
	pUI_Desc_2.vPos = { 677.f, 52.f };
	pUI_Desc_2.vSize = { 430.f, 47.f };
	m_UI_Descs.push_back(pUI_Desc_2);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Hp"),
		TEXT("Com_Texture_Status_Hp"), reinterpret_cast<CComponent**>(&m_pTextureHp))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureHp);

	if (m_eMyCharacter == BOSS_NARUTO)
	{
		UI_Boss_UI_DESC pUI_Desc_3;
		pUI_Desc_3.vPos = { 435.f, 50.f };
		pUI_Desc_3.vSize = { 100.f, 60.f };
		m_UI_Descs.push_back(pUI_Desc_3);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Face_Naruto"),
			TEXT("Com_Texture_Face_Icon"), reinterpret_cast<CComponent**>(&m_pTextureIcon))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon);
	}
	else if (m_eMyCharacter == BOSS_FINAL)
	{
		UI_Boss_UI_DESC pUI_Desc_3;
		pUI_Desc_3.vPos = { 300.f, 100.f };
		pUI_Desc_3.vSize = { 150.f, 100.f };
		m_UI_Descs.push_back(pUI_Desc_3);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Face_Naruto"),
			TEXT("Com_Texture_Face_Icon"), reinterpret_cast<CComponent**>(&m_pTextureIcon))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon);
	}
	return S_OK;
}

CUI_Boss_Status* CUI_Boss_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Boss_Status* pInstance = new CUI_Boss_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Boss_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Boss_Status::Clone(void* pArg)
{
	CUI_Boss_Status* pInstance = new CUI_Boss_Status(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Boss_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Boss_Status::Free()
{
	for (auto pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	__super::Free();
}

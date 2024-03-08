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
	UI_Player_CoolTime_DESC* pCoolTime_Desc = (UI_Player_CoolTime_DESC*)pArg;

	for (_uint i = 0; i < 4; i++)
	{
		if (pCoolTime_Desc->pCurrentCoolTime[i] == nullptr)
			continue;

		m_pCurrentCoolTime[i]	= pCoolTime_Desc->pCurrentCoolTime[i];
		m_pCoolTime[i]			= pCoolTime_Desc->pCoolTime[i];
	}
	m_eMyCharacter = pCoolTime_Desc->eMyCharacter;

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
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Player_Skills::Render()
{
	__super::Render();

	_float2 vBaseSize = { 70.f, 70.f };
	_uint i = { 0 };
	_uint iPassIndex = { 0 };
	for (auto pTextures : m_Textures)
	{
		vBaseSize = { 70.f, 70.f };

		if (m_eMyCharacter == PLAYER_NARUTO && i == 2)
			vBaseSize = { 55.f, 55.f };
		if (m_eMyCharacter == PLAYER_CUSTOM && i == 3)
			vBaseSize = { 55.f, 55.f };

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

		// 쿨이 다 돌았으면 fRatio가 0이고, 방금 사용한 상태면 1이다.

		_float	fRatio = *m_pCurrentCoolTime[i] / *m_pCoolTime[i];

		if (fRatio <= 0)
			fRatio = 0;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Begin(2)))
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Skill_Base"),
		TEXT("Com_Texture_Base"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL;

	if (m_eMyCharacter == PLAYER_NARUTO)
	{
		UI_Player_Skills_DESC pUI_Desc_1;
		pUI_Desc_1.vPos = { 1180.f, 650.f };
		pUI_Desc_1.vSize = { 55.f, 55.f };
		m_UI_Descs.push_back(pUI_Desc_1);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Rasengun"),
			TEXT("Com_Texture_Icon_Rasengun"), reinterpret_cast<CComponent**>(&m_pTextureIcon_1))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_1);

		UI_Player_Skills_DESC pUI_Desc_2;
		pUI_Desc_2.vPos = { 1100.f, 650.f };
		pUI_Desc_2.vSize = { 55.f, 55.f };
		m_UI_Descs.push_back(pUI_Desc_2);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_RasenShuriken"),
			TEXT("Com_Texture_Icon_RasenShuriken"), reinterpret_cast<CComponent**>(&m_pTextureIcon_2))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_2);

		UI_Player_Skills_DESC pUI_Desc_3;
		pUI_Desc_3.vPos = { 1020.f, 655.f };
		pUI_Desc_3.vSize = { 40.f, 40.f };
		m_UI_Descs.push_back(pUI_Desc_3);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_WoodSwap"),
			TEXT("Com_Texture_Icon_WoodSwap"), reinterpret_cast<CComponent**>(&m_pTextureIcon_3))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_3);
	}
	else if (m_eMyCharacter == PLAYER_CUSTOM)
	{
		UI_Player_Skills_DESC pUI_Desc_1;
		pUI_Desc_1.vPos = { 1180.f, 650.f };
		pUI_Desc_1.vSize = { 55.f, 55.f };
		m_UI_Descs.push_back(pUI_Desc_1);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_FireBall"),
			TEXT("Com_Texture_Icon_FireBall"), reinterpret_cast<CComponent**>(&m_pTextureIcon_1))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_1);
		
		UI_Player_Skills_DESC pUI_Desc_2;
		pUI_Desc_2.vPos = { 1100.f, 650.f };
		pUI_Desc_2.vSize = { 55.f, 55.f };
		m_UI_Descs.push_back(pUI_Desc_2);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Chidori"),
			TEXT("Com_Texture_Icon_Chidori"), reinterpret_cast<CComponent**>(&m_pTextureIcon_2))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_2);
		
		UI_Player_Skills_DESC pUI_Desc_3;
		pUI_Desc_3.vPos = { 1020.f, 650.f };
		pUI_Desc_3.vSize = { 55.f, 55.f };
		m_UI_Descs.push_back(pUI_Desc_3);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_WoodHand"),
			TEXT("Com_Texture_Icon_WoodHand"), reinterpret_cast<CComponent**>(&m_pTextureIcon_3))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_3);

		UI_Player_Skills_DESC pUI_Desc_4;
		pUI_Desc_4.vPos = { 940.f, 655.f };
		pUI_Desc_4.vSize = { 40.f, 40.f };
		m_UI_Descs.push_back(pUI_Desc_4);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_WoodSwap"),
			TEXT("Com_Texture_Icon_WoodSwap"), reinterpret_cast<CComponent**>(&m_pTextureIcon_4))))
			return E_FAIL;
		m_Textures.push_back(m_pTextureIcon_4);
	}

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

	__super::Free();
}

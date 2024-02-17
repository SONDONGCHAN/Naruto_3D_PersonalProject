#include "UI_Player_Custom.h"

CUI_Player_Custom::CUI_Player_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Player_Custom::CUI_Player_Custom(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Player_Custom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Custom::Initialize(void* pArg)
{
	UI_Player_Custom_DESC* Custom_Desc = (UI_Player_Custom_DESC*)pArg;

	for (_uint i = 0; i < PARTS_END; i++)
	{
		if (Custom_Desc->pParts_Cursor[i] == nullptr)
			continue;

		m_pParts_Cursor[i]	= Custom_Desc->pParts_Cursor[i];
		m_iNum_Parts[i]		= Custom_Desc->iNum_Parts[i];

	}
	m_pTitle_Cursor = Custom_Desc->pTitle_Cursor;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Custom::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Player_Custom::Tick(_float fTimeDelta)
{
}

void CUI_Player_Custom::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Player_Custom::Render()
{
	__super::Render();

	_uint i = { 0 };

	for (auto pTextures : m_Textures)
	{
		wstring UIText = L"";
		_float2 _Pos = m_UI_Descs[i].vPos;
		_uint iLoop = { 1 };
		_uint iIndex_Selected = { 0 };

		if (i == 1)
		{
			if (*m_pTitle_Cursor == 0)
				UIText = L"머리";
			else if (*m_pTitle_Cursor == 1)
				UIText = L"얼굴";
			else if (*m_pTitle_Cursor == 2)
				UIText = L"상의";
			else if (*m_pTitle_Cursor == 3)
				UIText = L"하의";			
		}

		if (i == 2)
		{
			if (*m_pTitle_Cursor == 0){
				iLoop = m_iNum_Parts[PARTS_HEADGEAR];
				iIndex_Selected = *m_pParts_Cursor[PARTS_HEADGEAR];
			}
			else if (*m_pTitle_Cursor == 1) {
				iLoop = m_iNum_Parts[PARTS_FACE];
				iIndex_Selected = *m_pParts_Cursor[PARTS_FACE];
			}
			else if (*m_pTitle_Cursor == 2) {
				iLoop = m_iNum_Parts[PARTS_UPPER];
				iIndex_Selected = *m_pParts_Cursor[PARTS_UPPER];
			}
			else if (*m_pTitle_Cursor == 3) {
				iLoop = m_iNum_Parts[PARTS_LOWER];
				iIndex_Selected = *m_pParts_Cursor[PARTS_LOWER];
			}
		}
		else
			iLoop = 1;


		for (_uint j = 0; j < iLoop; j++)
		{

			if (i == 2)
			{
				if (*m_pTitle_Cursor == 0) {
					if (j == 0)
						UIText = L"헤어스타일 1";
					else if (j == 1)
						UIText = L"헤어스타일 2";
				}
				else if (*m_pTitle_Cursor == 1) {
					if (j == 0)
						UIText = L"백안 얼굴";
					else if(j == 1)
						UIText = L"마스크 얼굴";
				}
				else if (*m_pTitle_Cursor == 2) {
					if (j == 0)
						UIText = L"상의 1";
					else if (j == 1)
						UIText = L"상의 2";
				}
				else if (*m_pTitle_Cursor == 3) {
					if (j == 0)
						UIText = L"하의 1";
					else if (j == 1)
						UIText = L"하의 2";
				}
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_Pos.x - g_iWinSizeX * 0.5f, -(_Pos.y) + g_iWinSizeY * 0.5f, 0.f, 1.f));
			XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				return E_FAIL;

			if (i == 2 && iIndex_Selected == j)
			{
				if (FAILED(m_pTextureParts_Selected->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pTextures->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_UI_Descs[i].vSize), sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Bind_Buffers()))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Maple"), UIText, _Pos, XMVectorSet(0.f, 0.f, 0.f, 1.f), ORIGIN_CENTER, 0.f, 0.5f)))
				return E_FAIL;

			_Pos.y += 40.f;
		}

		i++;
	}


	return S_OK;
}

HRESULT CUI_Player_Custom::Add_Component()
{
	UI_Player_Custom_UI_DESC pUI_Desc_1;
	pUI_Desc_1.vPos = { 980.f, 360.f };
	pUI_Desc_1.vSize = { 500.f, 400.f };
	m_UI_Descs.push_back(pUI_Desc_1);
	if (FAILED(CGameObject::Add_Component(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Base"),
		TEXT("Com_Texture_Base"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureBase);
	
	UI_Player_Custom_UI_DESC pUI_Desc_2;
	pUI_Desc_2.vPos = { 980.f, 190.f };
	pUI_Desc_2.vSize = { 470.f, 50.f };
	m_UI_Descs.push_back(pUI_Desc_2);
	if (FAILED(CGameObject::Add_Component(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Title"),
		TEXT("Com_Texture_Title"), reinterpret_cast<CComponent**>(&m_pTextureTitle))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureTitle);

	UI_Player_Custom_UI_DESC pUI_Desc_3;
	pUI_Desc_3.vPos = { 980.f, 240.f };
	pUI_Desc_3.vSize = { 430.f, 35.f };
	m_UI_Descs.push_back(pUI_Desc_3);
	if (FAILED(CGameObject::Add_Component(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Parts"),
		TEXT("Com_Texture_Parts"), reinterpret_cast<CComponent**>(&m_pTextureParts))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureParts);

	UI_Player_Custom_UI_DESC pUI_Desc_4;
	pUI_Desc_4.vPos = { 980.f, 240.f };
	pUI_Desc_4.vSize = { 430.f, 35.f };
	m_UI_Descs.push_back(pUI_Desc_4);
	if (FAILED(CGameObject::Add_Component(LEVEL_CUSTOMROOM, TEXT("Prototype_Component_Texture_Custom_Parts_Sel"),
		TEXT("Com_Texture_Parts_Sel"), reinterpret_cast<CComponent**>(&m_pTextureParts_Selected))))
		return E_FAIL;

	return S_OK;

}

CUI_Player_Custom* CUI_Player_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Player_Custom* pInstance = new CUI_Player_Custom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Skills");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_Custom::Clone(void* pArg)
{
	CUI_Player_Custom* pInstance = new CUI_Player_Custom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Player_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Player_Custom::Free()
{
	for (auto pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	Safe_Release(m_pTextureParts_Selected);
	
	__super::Free();
}

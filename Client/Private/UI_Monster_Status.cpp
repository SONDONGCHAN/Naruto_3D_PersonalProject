#include "UI_Monster_Status.h"

CUI_Monster_Status::CUI_Monster_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Monster_Status::CUI_Monster_Status(const CUI& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Monster_Status::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Monster_Status::Initialize(void* pArg)
{
	UI_Monster_Hp_DESC* pHp_Desc = (UI_Monster_Hp_DESC*)pArg;
	m_pCurrent_Hp	= pHp_Desc->pCurrentHp;
	m_pMax_Hp		= pHp_Desc->pMaxHp;
	m_pWorldMatrix	= pHp_Desc->pWorldMatrix;
	m_eMyType		= pHp_Desc->eMyType;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1000.f));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CUI_Monster_Status::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Monster_Status::Tick(_float fTimeDelta)
{
}

void CUI_Monster_Status::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Monster_Status::Render()
{
	_vector Monster_Pos;
	memcpy(&Monster_Pos, &((*m_pWorldMatrix).m[3]), sizeof(_float4));


	Monster_Pos.m128_f32[1] += 1.7f;
	_matrix		ViewMatrix	= m_pGameInstance->Get_ViewMatrix();
	_matrix		ProjMatrix	= m_pGameInstance->Get_ProjMatrix();

	Monster_Pos = XMVector3TransformCoord(Monster_Pos, ViewMatrix);
	Monster_Pos = XMVector3TransformCoord(Monster_Pos, ProjMatrix);

	if (Monster_Pos.m128_f32[2] < 0)
		return S_OK;

	_float2 vPos;
	vPos.x = Monster_Pos.m128_f32[0]* g_iWinSizeX * 0.5f;
	vPos.y = Monster_Pos.m128_f32[1]* g_iWinSizeY * 0.5f;

	__super::Render();

	_uint i = { 0 };

	for (auto pTextures : m_Textures)
	{		
		if (   (vPos.x + (m_UI_Descs[i].vSize.x * 0.5f)) < (-(g_iWinSizeX * 0.5f))
			|| (vPos.x - (m_UI_Descs[i].vSize.x * 0.5f)) > (  g_iWinSizeX * 0.5f)
			|| (vPos.y + (m_UI_Descs[i].vSize.y * 0.5f)) < (-(g_iWinSizeY * 0.5f))
			|| (vPos.y - (m_UI_Descs[i].vSize.y * 0.5f)) > (  g_iWinSizeY * 0.5f) )
		return S_OK;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;

		if (FAILED(pTextures->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("vSize", &(m_UI_Descs[i].vSize), sizeof(_float2))))
			return E_FAIL;

		_float	fRatio;
		_vector vColor;

		if (i == 0)
		{
			fRatio = 1.f;
			vColor = { 0.5f, 0.f, 0.f, 1.f };
		}

		else if (i == 1)
		{
			fRatio = *m_pCurrent_Hp / *m_pMax_Hp;

			if (fRatio <= 0)
				fRatio = 0;

			vColor = { 0.f, 0.5f, 0.f, 1.f };

		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))

			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		i++;
	}

	return S_OK;
}

HRESULT CUI_Monster_Status::Add_Component()
{
	UI_Monster_Status_DESC pUI_Desc_1;
	pUI_Desc_1.vSize = { 30.f, 6.5f };
	m_UI_Descs.push_back(pUI_Desc_1);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Hp_Base"),
		TEXT("Com_Texture_Base"), reinterpret_cast<CComponent**>(&m_pTextureBase))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureBase);

	UI_Monster_Status_DESC pUI_Desc_2;
	pUI_Desc_2.vSize = { 30.f, 6.5f };
	m_UI_Descs.push_back(pUI_Desc_2);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Hp"),
		TEXT("Com_Texture_Hp"), reinterpret_cast<CComponent**>(&m_pTextureHp))))
		return E_FAIL;
	m_Textures.push_back(m_pTextureHp);

	return S_OK;
}

CUI_Monster_Status* CUI_Monster_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Monster_Status* pInstance = new CUI_Monster_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Monster_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Monster_Status::Clone(void* pArg)
{
	CUI_Monster_Status* pInstance = new CUI_Monster_Status(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Monster_Status");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Monster_Status::Free()
{
	for (auto pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	__super::Free();
}

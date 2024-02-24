#include "Trail_Line.h"
#include "GameInstance.h"

CTrail_Line::CTrail_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrail_Line::CTrail_Line(const CTrail_Line& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrail_Line::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Line::Initialize(void* pArg)
{
	Trail_Line_DESC* pTrail_Line_Desc = (Trail_Line_DESC*)pArg;

	m_pParentTransform	= pTrail_Line_Desc->pParentTransform;
	m_pSocketMatrix		= pTrail_Line_Desc->pSocketMatrix;
	m_eMyCharacter		= pTrail_Line_Desc->eMyCharacter;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix_Current, XMMatrixIdentity());
	m_pTransformCom->Set_World(m_WorldMatrix_Current);
	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

	if (m_eMyCharacter == PLAYER_CUSTOM) {
		m_vColor = { 113.f / 255.f, 199.f / 255.f, 236.f / 255.f , 1.f };
		m_vThickness = 0.025f;
	}
	else if (m_eMyCharacter == PLAYER_NARUTO) {
		m_vColor = { 252.f / 255.f, 220.f / 255.f, 77.f / 255.f , 1.f };
		m_vThickness = 0.025f;
	}
	else if (m_eMyCharacter == MONSTER_NARUTO) {
		m_vColor = { 255.f / 255.f, 100.f / 255.f, 100.f / 255.f , 1.f };
		m_vThickness = 0.025f;
	}
	else if (m_eMyCharacter == BOSS_KURAMA) {
		m_vColor = { 0.f, 0.f, 0.f , 1.f };
		m_vThickness = 0.05f;
	}

	return S_OK;
}

void CTrail_Line::Priority_Tick(_float fTimeDelta)
{
}

void CTrail_Line::Tick(_float fTimeDelta)
{

}

void CTrail_Line::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTrail_Line::Render()
{	
	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	XMStoreFloat4x4(&m_WorldMatrix_Current, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
	//m_pTransformCom->Set_World(m_WorldMatrix_Current);
	
	_float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
	_float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
	_float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();
	
	for (_uint i = 59; i > 0; i--)
	{
		m_WorldMatrix_Old[i] = m_WorldMatrix_Old[i - 1];
	}
	m_WorldMatrix_Old[0] = m_WorldMatrix_Current;

	
	for (_uint i = 0 ; i < 59 ; i++)
	{
		m_vColor.m128_f32[3] = 1.f - (1.f / 60.f) * i;

		m_vThick.y = m_vThick.x;
		m_vThick.x = (1.f - (1.f / 60.f) * i)  * m_vThickness;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Old[i])))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix_Old", &m_WorldMatrix_Old[i+1])))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vfThickness", &m_vThick, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CTrail_Line::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}


CTrail_Line* CTrail_Line::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail_Line* pInstance = new CTrail_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail_Line");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Line::Clone(void* pArg)
{
	CTrail_Line* pInstance = new CTrail_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail_Line");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTrail_Line::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}

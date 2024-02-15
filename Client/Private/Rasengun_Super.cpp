#include "Rasengun_Super.h"
#include "GameInstance.h"

CRasengun_Super::CRasengun_Super(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CRasengun_Super::CRasengun_Super(const CRasengun_Super& rhs)
	:CSkill(rhs)
{
}

HRESULT CRasengun_Super::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRasengun_Super::Initialize(void* pArg)
{
	SKILL_RASENGUN_SUPER_DESC* pRasengun_Super_Desc = (SKILL_RASENGUN_SUPER_DESC*)pArg;

	m_pSocketMatrix = pRasengun_Super_Desc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_pTransformCom->Set_World(m_WorldMatrix);
	
	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	//m_pTransformCom->Rotation(XMVectorSet(, , , 0.f), XMConvertToRadians());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	m_OriginalMat = m_pTransformCom->Get_WorldMatrix();

	m_fSkill_Power = 10.f;

	return S_OK;
}

void CRasengun_Super::Priority_Tick(_float fTimeDelta)
{
}

void CRasengun_Super::Tick(_float fTimeDelta)
{
	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	XMStoreFloat4x4(&m_WorldMatrix, m_OriginalMat * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
	m_pTransformCom->Set_World(m_WorldMatrix);

	State_Control(fTimeDelta);
}

void CRasengun_Super::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (myState == STATE_HIT)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderMain);
	}
#endif  
}

HRESULT CRasengun_Super::Render()
{
	return S_OK;
}

void CRasengun_Super::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CRasengun_Super::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CRasengun_Super::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CRasengun_Super::State_Control(_float fTimeDelta)
{
	if (myState == STATE_MAKING)
	{
		// 이펙트 채워넣기
	}

	else if (myState == STATE_HIT)
	{
		m_fDurTime += fTimeDelta;

		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

		if (m_User_Type == USER_PLAYER)
			m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Super_Collider");
		else if (m_User_Type == USER_MONSTER)
			m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Super_Collider");
	
		if (m_fDurTime > 1.2f)
			Set_Next_State();
	
		// 이펙트 채워넣기
	}
}

void CRasengun_Super::Set_Next_State()
{
	if (myState != STATE_END)
		myState = (RASENGUN_SUPER_STATE)((_uint)myState + 1);

	if (myState == STATE_HIT)
	{
		m_pColliderMain->Set_Radius(2.f);
		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
		m_fDurTime = 0;
	}
	else if (myState == STATE_FINISH)
	{
		m_pColliderMain->Set_Radius(0.f);
		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());

		if (m_User_Type == USER_PLAYER)
			m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Rasengun_Super_Collider");
		else if (m_User_Type == USER_MONSTER)
			m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Player_Main_Collider", L"Rasengun_Super_Collider");

		m_fDurTime = 0;
	}
}

void CRasengun_Super::Set_State()
{
	myState = STATE_MAKING;
}

HRESULT CRasengun_Super::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_"),
	//    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//    return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
	BoundingDesc.fRadius = 0.f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
		return E_FAIL;
	
	m_pGameInstance->Add_Collider(TEXT("Rasengun_Super_Collider"), m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);
	
	return S_OK;
}

CRasengun_Super* CRasengun_Super::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasengun_Super* pInstance = new CRasengun_Super(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasengun_Super");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRasengun_Super::Clone(void* pArg)
{
	CRasengun_Super* pInstance = new CRasengun_Super(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRasengun_Super");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasengun_Super::Free()
{
	Safe_Release(m_pColliderMain);

	__super::Free();
}

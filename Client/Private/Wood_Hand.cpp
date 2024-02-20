#include "Wood_Hand.h"

CWood_Hand::CWood_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CWood_Hand::CWood_Hand(const CWood_Hand& rhs)
	:CSkill(rhs)
{
}

HRESULT CWood_Hand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWood_Hand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CWood_Hand::Priority_Tick(_float fTimeDelta)
{
	for (auto pModel : m_vModels)
		pModel->Play_Animation(fTimeDelta, 1.f);
}

void CWood_Hand::Tick(_float fTimeDelta)
{ 
	State_Control(fTimeDelta);
}

void CWood_Hand::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (myState == STATE_HIT)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderMain);
	}
#endif  
}

HRESULT CWood_Hand::Render()
{
	if (myState == STATE_DETECTING || myState == STATE_HIT)
	{
		for (auto pModel : m_vModels)
		{ 
			 _vector	vRight		= m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			 _vector	vPosition	= m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			 _float4x4	WorldMatrix;
			 XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix());

			 if (pModel == m_vModels[0])
			 {
				 vPosition -= (XMVector3Normalize(vRight) * 3);
				 _float4		vTmp;
				 XMStoreFloat4(&vTmp, vPosition);
				 memcpy(&WorldMatrix.m[3], &vTmp, sizeof(_float4));
			 }
			 else if (pModel == m_vModels[1])
			 {
				 vPosition += (XMVector3Normalize(vRight) * 3);				
				 _float4		vTmp;
				 XMStoreFloat4(&vTmp, vPosition);
				 memcpy(&WorldMatrix.m[3], &vTmp, sizeof(_float4));
			 }

			 if (FAILED(Bind_ShaderResources(WorldMatrix)))
			 	return E_FAIL;
		   
			_uint	iNumMeshes = pModel->Get_NumMeshes();

			for (_uint i = 0; i < iNumMeshes; i++)
			{
				if (FAILED(pModel->Bind_Material_ShaderResource(m_pShaderCom, i, 1, "g_DiffuseTexture")))
					return E_FAIL;
			
				if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
					return E_FAIL;
			
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
			
				if (FAILED(pModel->Render(i)))
					return E_FAIL;
			}
		}

	}
	return S_OK;
}

void CWood_Hand::Collider_Event_Enter(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Hand::Collider_Event_Stay(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Hand::Collider_Event_Exit(const wstring& strColliderLayerTag, CCollider* pMyCollider, CCollider* pTargetCollider)
{
}

void CWood_Hand::State_Control(_float fTimeDelta)
{
	if (myState == STATE_MAKING)
	{
		for (auto pModel : m_vModels)
			pModel->Set_Animation(1, 0.f, true, 2.f, false, false);
	}
	else if (myState == STATE_DETECTING)
	{
		m_fDurTime += fTimeDelta;
		_vector vDir = { 0.f, 1.f, 0.f };
		m_pTransformCom->Go_Custom_Direction(fTimeDelta, 8.f, vDir);
		
		if (m_fDurTime > 0.5f)
			Set_Next_State();

	}
	else if (myState == STATE_HIT)
	{
		m_fDurTime += fTimeDelta;

		if (m_fDurTime > 0.7f)
		{
			if(m_vModels[0]->Get_Current_Animation()->Get_CanStop())
				Set_Next_State();

			return;
		}
		if (m_fDurTime > 0.5f)
		{
			if(m_fDurTime < 0.52f)
				m_pCamera->ShakeCamera(CCamera_Free::SHAKE_ALL, 3.f, 0.1f);

			m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Wood_Hand_Collider");
		}

	}
}

void CWood_Hand::Set_Next_State()
{
	if (myState != STATE_END)
		myState = (WOODD_HAND_STATE)((_uint)myState + 1);

	if (myState == STATE_DETECTING)
	{
		m_pTransformCom->Set_Pos(m_vTarget_Pos);	
		_vector vDir = m_pParentTransform->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_Look(vDir);
		m_fDurTime = 0;
	}
	if (myState == STATE_HIT)
	{
		for (auto pModel : m_vModels)
			pModel->Set_Animation(0, 0.f, false, 2.8f, false, false);

		m_pColliderMain->Set_Radius(5.f);
		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
		m_fDurTime = 0;
	}
	else if (myState == STATE_FINISH)
	{
		m_pColliderMain->Set_Radius(0.f);
		m_pColliderMain->Tick(m_pTransformCom->Get_WorldMatrix());
		m_pGameInstance->Check_Collision_For_TargetEvent(m_pColliderMain, L"Monster_Main_Collider", L"Wood_Hand_Collider");
		m_fDurTime = 0;
		m_bTargeting = false;
	}
}

void CWood_Hand::Set_State()
{
	myState = STATE_MAKING;
}

void CWood_Hand::Set_Targeting(_vector Target_Pos)
{
	m_bTargeting = true;
	m_vTarget_Pos = Target_Pos;
}

HRESULT CWood_Hand::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CModel* m_pModel_L;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Skill_WoodHand_L"),
		TEXT("Com_Model_L"), reinterpret_cast<CComponent**>(&m_pModel_L))))
		return E_FAIL;
	m_vModels.push_back(m_pModel_L);
	
	CModel* m_pModel_R;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Skill_WoodHand_R"),
		TEXT("Com_Model_R"), reinterpret_cast<CComponent**>(&m_pModel_R))))
		return E_FAIL;
	m_vModels.push_back(m_pModel_R);

	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};
	BoundingDesc.fRadius = 0.f;
	BoundingDesc.vCenter = _float3(0.f, 4.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Main"), reinterpret_cast<CComponent**>(&m_pColliderMain), &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider(TEXT("Wood_Hand_Collider"), m_pColliderMain);
	m_pColliderMain->Set_Collider_GameObject(this);

	return S_OK;
}

HRESULT CWood_Hand::Bind_ShaderResources(_float4x4 _WorldMatrix)
{
	_float4x4		ViewMatrix = m_pGameInstance->Get_ViewMatrix_Float();
	_float4x4		ProjMatrix = m_pGameInstance->Get_ProjMatrix_Float();
	_float4			CameraPos = m_pGameInstance->Get_CameraPos_Float();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CameraPos, sizeof(_float4))))
	//	return E_FAIL;


	return S_OK;
}

CWood_Hand* CWood_Hand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWood_Hand* pInstance = new CWood_Hand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWood_Hand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWood_Hand::Clone(void* pArg)
{
	CWood_Hand* pInstance = new CWood_Hand(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWood_Hand");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CWood_Hand::Free()
{
	Safe_Release(m_pColliderMain);

	for (auto pModel : m_vModels)
		Safe_Release(pModel);

	m_vModels.clear();

	__super::Free();
}

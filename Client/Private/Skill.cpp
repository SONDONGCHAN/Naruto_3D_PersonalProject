#include "stdafx.h"
#include "Skill.h"

CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CSkill::CSkill(const CSkill& rhs)
    :CGameObject(rhs)
{
}

HRESULT CSkill::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
    SKILL_DESC* pSkillDesc = (SKILL_DESC*)pArg;
    
    m_pParentTransform  = pSkillDesc->pParentTransform;
    m_User_Type         = pSkillDesc->User_Type;
    m_pCamera           = pSkillDesc->pCamera;
    m_Current_Level     = pSkillDesc->Current_Level;
    
    Safe_AddRef(m_pParentTransform);
    Safe_AddRef(m_pCamera);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
    
    return S_OK;
}

void CSkill::Priority_Tick(_float fTimeDelta)
{
}

void CSkill::Tick(_float fTimeDelta)
{
}

void CSkill::Late_Tick(_float fTimeDelta)
{
}

HRESULT CSkill::Render()
{

	return S_OK;
}

void CSkill::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom); 
    Safe_Release(m_pParentTransform);
    Safe_Release(m_pCamera);
}

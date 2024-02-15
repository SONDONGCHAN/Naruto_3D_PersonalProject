#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;

END

BEGIN(Client)

class CSkill abstract : public CGameObject
{
public:
	enum USER_TYPE {USER_PLAYER, USER_MONSTER, USER_END };

	struct SKILL_DESC
	{
		CTransform* pParentTransform;
		USER_TYPE	User_Type; 
	};

protected:
	CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float		Get_Skill_Power() { return m_fSkill_Power; }
	CTransform*	Get_Skill_Transform() { return m_pTransformCom; }

protected:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CTransform*		m_pParentTransform = { nullptr };

	USER_TYPE		m_User_Type = { USER_END };
	_float4x4		m_WorldMatrix;

protected:
	_float		m_fSkill_Power = { 0.f };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END
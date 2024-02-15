#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Monster_LeafNinja : public CPartObject
{
public:
	struct BODY_MONSTER_DESC : public CPartObject::PARTOBJ_DESC
	{
		_uint*			pState;
	};

private:
	CBody_Monster_LeafNinja(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Monster_LeafNinja(const CBody_Monster_LeafNinja& rhs);
	virtual ~CBody_Monster_LeafNinja() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Animation_State(_float fTimeDelta);

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

private:
	_float				m_fAnimationPlayRatio = { 1.f };
	_uint*				m_iMonsterState = { nullptr };

	_uint				m_iComboCount	= { 0 };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Monster_LeafNinja*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END



#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Player_Custom_Face: public CPartObject
{
public:
	struct BODY_PLAYER_DESC : public CPartObject::PARTOBJ_DESC
	{
		_ulonglong* pPlayerState;
		_float* pPlayerGravity;
		_float* pPlayerGForcePersec;
		LEVEL	Current_LEVEL;
	};

private:
	CBody_Player_Custom_Face(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player_Custom_Face(const CBody_Player_Custom_Face& rhs);
	virtual ~CBody_Player_Custom_Face() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Animation_State(_float fTimeDelta);
	void	Set_Model(_bool Updown);
	virtual CModel* Get_CurrentModel() override;
	_int	Get_Num_Models() { return m_Num_Models; }
	_int* Get_Current_Index() { return &m_CurrentModel_Index; }

private:
	CShader*		m_pShaderCom = { nullptr };
	CShader*		m_pShaderOutLine = { nullptr };

	vector<CModel*> m_FaceModels;


private:
	_ulonglong* m_pPlayerState = { nullptr };
	_float* m_pPlayerGravity = { nullptr };
	_float* m_pPlayerGForcePerSec = { nullptr };
	_float	m_fAnimationPlayRatio = { 1.f };

	_int	m_CurrentModel_Index = { 0 };
	_int	m_Num_Models = { 0 };

private:
	HRESULT Add_Component(LEVEL Current_LEVEL);
	HRESULT Add_Models(LEVEL Current_LEVEL);
	HRESULT Bind_ShaderResources();


public:
	static CBody_Player_Custom_Face*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END

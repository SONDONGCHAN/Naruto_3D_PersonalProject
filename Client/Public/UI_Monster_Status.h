#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Monster_Status : public CUI
{
public:
	enum MONSTER_TYPE { MONSTER_NORMAL, MONSTER_BOSS, MONSTER_END};

	struct UI_Monster_Hp_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float*			pCurrentHp;
		_float*			pMaxHp;
		_float4x4*		pWorldMatrix;
		MONSTER_TYPE	eMyType;
	};
private:
	struct UI_Monster_Status_DESC
	{
		_float2 vSize;
	};

private:
	CUI_Monster_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Monster_Status(const CUI& rhs);
	virtual ~CUI_Monster_Status() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureBase = { nullptr };
	CTexture* m_pTextureHp = { nullptr };

private:
	vector<UI_Monster_Status_DESC>	m_UI_Descs;
	vector<CTexture*>				m_Textures;
	_float*			m_pCurrent_Hp;
	_float*			m_pMax_Hp;
	_float4x4*		m_pWorldMatrix;
	MONSTER_TYPE	m_eMyType = { MONSTER_END };

private:
	HRESULT Add_Component();

public:
	static CUI_Monster_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END